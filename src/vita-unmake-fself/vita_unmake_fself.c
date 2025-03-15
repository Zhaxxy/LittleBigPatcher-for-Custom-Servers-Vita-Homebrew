//vita-unmake-fself
//@dots_tb @CelesteBlue123
// Thanks to: team_molecule, theflow
// Modifed by LittleBigPatcherTeam to make it easier to use in the vita homebrew

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <zlib.h>

#include <psp2/kernel/clib.h> // for sceClibPrintf

#include "self.h"
#include "elf.h"


int unmake_fself(const char * input_eboot_bin, const char * output_path)
{
	FILE *fin = NULL;
	FILE *fout = NULL;
	fin = fopen(input_eboot_bin, "rb");
	
	if (!fin) {
		sceClibPrintf("Failed to open input file");
		goto error;
	}
	fseek(fin, 0, SEEK_END);
	size_t sz = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	uint8_t *input = calloc(1, sz);	
	if (!input) {
		sceClibPrintf("Failed to allocate buffer for input file");
		goto error;
	}
	if (fread(input, sz, 1, fin) != 1) {
		static const char s[] = "Failed to read input file";
		if (feof(fin))
			sceClibPrintf("%s: unexpected end of file\n", s);
		else
			sceClibPrintf(s);
		goto error;
	}
	fclose(fin);
	fin = NULL;
	
	SCE_header *shdr = (SCE_header*)(input);
	if(shdr->magic != 0x454353) {
		sceClibPrintf("Not a self");
		goto error;
	}
	segment_info *sinfo = (segment_info *)(input + shdr->section_info_offset);
	ELF_header *ehdr = (ELF_header *)(input + shdr->elf_offset);
	fout = fopen(output_path, "wb");
	if (!fout) {
		sceClibPrintf("Failed to open output file");
		goto error;
	}
	fseek(fout, 0, SEEK_SET);
	Elf32_Phdr *phdr;
	if(memcmp(input + shdr->header_len,"\177ELF\1\1\1",8)==0) {
		ehdr = (ELF_header *)(input + shdr->header_len);
		ehdr->e_shoff = 0;
		ehdr->e_shnum = 0;
		ehdr->e_shstrndx = 0;
		fwrite(input + shdr->header_len, ehdr->e_ehsize, 1, fout);	
		phdr = (Elf32_Phdr *)(input + shdr->header_len + ehdr->e_phoff);
		sceClibPrintf("Using original elf header\n");
	} else {
		fwrite(input + shdr->elf_offset, ehdr->e_ehsize, 1, fout);	
		phdr = (Elf32_Phdr *)(input + shdr->phdr_offset);

	}
	
	for(int i = 0; i < ehdr->e_phnum; i++) {
		uint8_t *destination = (uint8_t *)(input + sinfo[i].offset);
		if(sinfo[i].compression == 2) {
			size_t sz = phdr[i].p_filesz;
			destination = (uint8_t *)calloc(1,sz);
			if (!destination) {
				sceClibPrintf("Error could not allocate memory.");
				goto error;
			}
			int ret = uncompress(destination, (long unsigned int*)&sz, input + sinfo[i].offset, sinfo[i].length);
			if(ret != Z_OK) {
			
				sceClibPrintf("Warning: could not decompress segment %d, (No segment?), will copy segment: %d",i, ret);
				destination = input + sinfo[i].offset;
			
			}
		}
		ELF_header *ehdr = (ELF_header *)(destination);
		if(memcmp(destination,"\177ELF\1\1\1",8)==0) {
			ehdr->e_shoff = 0;
			ehdr->e_shnum = 0;
			ehdr->e_shstrndx = 0;
			sceClibPrintf("2nd elf header");
		}
		fseek(fout, phdr[i].p_offset, SEEK_SET);
		fwrite(destination, phdr[i].p_filesz, 1, fout);
	}
	fseek(fout, ehdr->e_phoff, SEEK_SET);
	fwrite(phdr,  ehdr->e_phentsize, ehdr->e_phnum, fout);

	fclose(fout);

	return 0;
error:
	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);
	return 1;
	return 0;
}
