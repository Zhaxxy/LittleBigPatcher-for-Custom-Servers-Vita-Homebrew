/*
https://github.com/vitasdk/samples/blob/2ec8ca811b782ddc472d0f18d93c929030df4520/net_http/src/main.c
with a few modifications by myself
*/

#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>

#include <psp2/io/fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include <psp2/kernel/clib.h> // for sceClibPrintf
#define psvDebugScreenPrintf sceClibPrintf

#define HTTP_ALLOW_SMALLER_BUFFER_SIZE 0
#define HTTP_ERROR_IF_SMALLER_BUFFER_SIZE 1

void netInit() {
	psvDebugScreenPrintf("Loading module SCE_SYSMODULE_NET\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

	psvDebugScreenPrintf("Running sceNetInit\n");
	SceNetInitParam netInitParam;
	int size = 1*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	psvDebugScreenPrintf("Running sceNetCtlInit\n");
	sceNetCtlInit();
}

void netTerm() {
	psvDebugScreenPrintf("Running sceNetCtlTerm\n");
	sceNetCtlTerm();

	psvDebugScreenPrintf("Running sceNetTerm\n");
	sceNetTerm();

	psvDebugScreenPrintf("Unloading module SCE_SYSMODULE_NET\n");
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

void httpInit() {
	psvDebugScreenPrintf("Loading module SCE_SYSMODULE_HTTPS\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);
	
	psvDebugScreenPrintf("Running sceHttpInit\n");
	sceHttpInit(1*1024*1024);
	
	psvDebugScreenPrintf("Running sceSslInit\n");
	int res = sceSslInit(1*1024*1024);
	
	psvDebugScreenPrintf("0x%08X sceSslInit\n",res);
}

void httpTerm() {
	psvDebugScreenPrintf("Running sceHttpTerm\n");
	sceHttpTerm();
	sceSslTerm();

	psvDebugScreenPrintf("Unloading module SCE_SYSMODULE_HTTP\n");
	sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}

int http_download(const char *url, const char *dest) {
	int http_status_code;
	
	psvDebugScreenPrintf("\n\nDownloading %s to %s\n", url, dest);

	// Create template with user agend "PS Vita Sample App"
	int tpl = sceHttpCreateTemplate("PS Vita Sample App", SCE_HTTP_VERSION_1_1, 1);
	psvDebugScreenPrintf("0x%08X sceHttpCreateTemplate\n", tpl);
	if (tpl < 0) {
		return tpl;
	}

	// set url on the template
	int conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateConnectionWithURL\n", conn);
	if (conn < 0) {
		return conn;
	}

	// create the request with the correct method
	int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateRequestWithURL\n", request);
	if (request < 0) {
		return request;
	}

	// send the actual request. Second parameter would be POST data, third would be length of it.
	int handle = sceHttpSendRequest(request, NULL, 0);
	psvDebugScreenPrintf("0x%08X sceHttpSendRequest\n", handle);
	if (handle < 0) {
		return handle;
	}

	int http_status_res = sceHttpGetStatusCode(request,&http_status_code);
	psvDebugScreenPrintf("0x%08X sceHttpGetStatusCode\n", http_status_res);
	if (http_status_res < 0) {
		return http_status_res;
	}
	
	if (http_status_code != 200) {
		psvDebugScreenPrintf("Non http status 200, got %d instead", http_status_code);
		return -http_status_code;
	}
	
	FILE *file = fopen(dest, "wb");
	
	if (file == 0) {
		psvDebugScreenPrintf("Failed to open %s\n", dest);
		return -1;
	}
	
	unsigned char data[16*1024];
	
	while (1) {
		int bytes_read = sceHttpReadData(request, &data, sizeof(data));
		
		if (bytes_read < 0) {
			psvDebugScreenPrintf("0x%08X sceHttpReadData\n", bytes_read);
			fclose(file);
			return bytes_read;
		}
		
		if (bytes_read == 0) {
			break;
		}
		
		fwrite(data, 1, bytes_read, file);
	}
	
	fclose(file);
	return 0;
}

int http_download_to_buffer(const char* url, void * out_buffer, uint64_t out_buffer_size, bool allow_smaller_buffer_size) {
	int http_status_code;
	unsigned long long length = 0;
	
	psvDebugScreenPrintf("\n\nDownloading %s to (0x%llx)\n", url, (unsigned long long)out_buffer);

	// Create template with user agend "PS Vita Sample App"
	psvDebugScreenPrintf("buggin im ready\n");
	int tpl = sceHttpCreateTemplate("PS Vita Sample App", SCE_HTTP_VERSION_1_1, 1);
	psvDebugScreenPrintf("buggin tis done\n");
	psvDebugScreenPrintf("0x%08X sceHttpCreateTemplate\n", tpl);
	if (tpl < 0) {
		return tpl;
	}
	psvDebugScreenPrintf("buggin template\n");
	// set url on the template
	int conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateConnectionWithURL\n", conn);
	if (conn < 0) {
		return conn;
	}

	// create the request with the correct method
	int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateRequestWithURL\n", request);
	if (request < 0) {
		return request;
	}

	// send the actual request. Second parameter would be POST data, third would be length of it.
	int handle = sceHttpSendRequest(request, NULL, 0);
	psvDebugScreenPrintf("0x%08X sceHttpSendRequest\n", handle);
	if (handle < 0) {
		return handle;
	}

	int http_status_res = sceHttpGetStatusCode(request,&http_status_code);
	psvDebugScreenPrintf("0x%08X sceHttpGetStatusCode\n", http_status_res);
	if (http_status_res < 0) {
		return http_status_res;
	}
	
	if (http_status_code != 200) {
		psvDebugScreenPrintf("Non http status 200, got %d instead", http_status_code);
		return -http_status_code;
	}
	psvDebugScreenPrintf("buggin COTNENT LENGJHT\n");
	//GET SIZE
	sceHttpGetResponseContentLength(request, &length);
	
	if (allow_smaller_buffer_size == HTTP_ERROR_IF_SMALLER_BUFFER_SIZE) {
		if (length > out_buffer_size) {
			psvDebugScreenPrintf("Error : ContentLength is too big, %llu > %llu", length,out_buffer_size);
			return -1;
		}
	}
	psvDebugScreenPrintf("buggin GUHHH\n");

	uint64_t total_written = 0;
	unsigned char data[16*1024];
	psvDebugScreenPrintf("buggin\n");
	while (1) {
		int bytes_read = sceHttpReadData(request, &data, sizeof(data));
		if (bytes_read < 0) {
			psvDebugScreenPrintf("0x%08X sceHttpReadData\n", bytes_read);
			return bytes_read;
		}

		if (bytes_read == 0) {
			break;
		}

		uint64_t space_left = out_buffer_size - total_written;

		uint64_t to_copy = (bytes_read < space_left) ? bytes_read : space_left;

		if (to_copy > 0) {
			memcpy(out_buffer + total_written, data, to_copy);
			total_written += to_copy;
		}

		if (total_written >= out_buffer_size) {
			break;
		}
	}
	
	psvDebugScreenPrintf("sucersasd\n");
	return 0;
}

/*
int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	psvDebugScreenPrintf("HTTP Sample v.1.0 by barooney\n\n");

	netInit();
	httpInit();

	http_download("http://barooney.com/", "ux0:data/index.html");

	httpTerm();
	netTerm();

	psvDebugScreenPrintf("This app will close in 10 seconds!\n");
	sceKernelDelayThread(10*1000*1000);

	return 0;
}
*/