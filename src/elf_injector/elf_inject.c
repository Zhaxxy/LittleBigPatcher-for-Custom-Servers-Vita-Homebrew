//vita-inject-elf
//@dots_tb @CelesteBlue123
// Thanks to: Motoharu, team_molecule, theflow
// Modifed by LittleBigPatcherTeam to make it easier to use in the vita homebrew

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <zlib.h>

#include <psp2/kernel/clib.h> // for sceClibPrintf

#include "self.h"
#include "elf.h"
#include "sha256.h"


int elf_inject(const char * input_eboot_elf, const char * output_eboot_bin)
{
	FILE *files[2] = {NULL, NULL};
	FILE *fout = NULL;
	uint8_t *elf_file = NULL,
		*self_file = NULL;
	
	char *argv[] = {"L", input_eboot_elf, output_eboot_bin, NULL};

	files[0] = fopen(argv[1], "rb");
	files[1]  = fopen(argv[2], "rb");
	
	if (!files[0] || !files[1]) {
		sceClibPrintf("Failed to open file");
		goto error;
	}
	
	char output_path[1024];
		
	size_t self_sz,
			elf_sz;
	for(int i = 0; i < 2; i++) {
		fseek(files[i], 0, SEEK_END);
		size_t sz = ftell(files[i]);
		fseek(files[i], 0, SEEK_SET);
		uint8_t *input = calloc(1, sz);	
		if (!input) {
			sceClibPrintf("Failed to allocate buffer for input file");
			goto error;
		}
		if (fread(input, sz, 1, files[i]) != 1) {
			static const char s[] = "Failed to read input file";
			if (feof(files[i]))
				sceClibPrintf("%s: unexpected end of file\n", s);
			else
				sceClibPrintf(s);
			goto error;
		}
		
		fclose(files[i]);
		files[i] = NULL;
		SCE_header *shdr = (SCE_header*)(input);
		if(shdr->magic != 0x454353) {
			if(memcmp(input, "\177ELF\1\1\1",8)==0) {
				elf_file = input;
				elf_sz = sz;
			} else {
				sceClibPrintf("Unkown magic\n");
				free(input);
				goto error;
			}
		} else {
			strncpy(output_path, argv[i + 1], sizeof(output_path) - 1);
			self_file = input;
			self_sz = sz;
		}
	}

	if(!self_file || !elf_file) {
		sceClibPrintf("Could not find elf or self\n");
		goto error;
	}
	
	fout = fopen(output_path, "wb");
	if (!fout) {
		sceClibPrintf("Failed to open output file");
		goto error;
	}
	
	SCE_header *shdr = (SCE_header*)(self_file);
	Elf32_Ehdr *ehdr_self = (Elf32_Ehdr*)(self_file + shdr->elf_offset);
	Elf32_Ehdr *ehdr = (Elf32_Ehdr*)(elf_file);

	if(ehdr->e_phnum!=ehdr_self->e_phnum) {
		sceClibPrintf("Section headers number mismatch\n");
		goto error;
	}
	

	
	Elf32_Phdr *phdrs = (Elf32_Phdr*)(elf_file + ehdr->e_phoff);
	segment_info *sinfo = (segment_info *)(self_file + shdr->section_info_offset);
	
	size_t offset_correction = 0;
	for(int i = 0; i < ehdr->e_phnum; i++) {
		size_t seg_sz = phdrs[i].p_filesz; 
		uint8_t *dest = (uint8_t *)malloc(phdrs[i].p_filesz);
		sinfo[i].offset += offset_correction;
		if(!dest) {
			sceClibPrintf("Could not allocate memory for compression!\n");
			goto error;
		}
		int ret = compress(dest, (long unsigned int *)&seg_sz, elf_file + phdrs[i].p_offset, phdrs[i].p_filesz);
		if(ret != Z_OK) {
			sceClibPrintf("Error compressing %i\n", ret);
			free(dest);
			continue;
		}
		if(sinfo[i].length != seg_sz) {
			sceClibPrintf("Compressed to diff size %x - %x\n", sinfo[i].length, seg_sz);
			if(sinfo[i].length < seg_sz) {
				sceClibPrintf("Compressed size too big!\n");
				offset_correction += seg_sz - sinfo[i].length;
				sceClibPrintf("Offset correction set: %x\n", offset_correction);
				sinfo[i].length = seg_sz;
			}				
		}
		fseek(fout, sinfo[i].offset, SEEK_SET);
		fwrite(dest, sinfo[i].length, 1, fout);
		free(dest);
	}
	
	uint8_t elf_digest[0x20];
	
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, elf_file, elf_sz);
	sha256_final(&ctx, elf_digest);
	
	PSVita_CONTROL_INFO *control_info = (PSVita_CONTROL_INFO *)(self_file + shdr->controlinfo_offset);
	while(control_info->next) {
		switch(control_info->type) {
			case 4:
				memcpy(control_info->PSVita_elf_digest_info.elf_digest, &elf_digest, sizeof(elf_digest));
				break;
			case 5:
				memset(&control_info->PSVita_npdrm_info, 0, sizeof(control_info->PSVita_npdrm_info));
				break;
		}
		control_info = (PSVita_CONTROL_INFO*)((char*)control_info + control_info->size);
	}
	
	fseek(fout, 0, SEEK_SET);
	fwrite(self_file, shdr->header_len, 1, fout);
	fseek(fout, shdr->header_len, SEEK_SET);
	fwrite(self_file + shdr->elf_offset, sizeof(Elf32_Ehdr), 1, fout);
	fseek(fout, shdr->header_len + ehdr->e_phoff, SEEK_SET);
	fwrite(self_file + shdr->phdr_offset, ehdr->e_phnum, ehdr->e_phentsize, fout);
	
	fclose(fout);

	return 0;
error:
	for(int i = 0; i < 2; i++) {
		if(files[i]) {
			fclose(files[i]);
		}
	}
	if(fout) 
		fclose(fout);
	if(self_file)
		free(self_file);
	if(elf_file)
		free(elf_file);
	return 1;
	return 0;
}
