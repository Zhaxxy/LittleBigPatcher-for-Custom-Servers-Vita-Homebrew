#ifndef HTTP_H_   /* Include guard */
#define HTTP_H_

#define HTTP_ALLOW_SMALLER_BUFFER_SIZE 0
#define HTTP_ERROR_IF_SMALLER_BUFFER_SIZE 1

void netInit();
void httpInit();

void httpTerm();
void netTerm();

int http_download(const char *url, const char *dest);

int http_download_to_buffer(const char* url, void * out_buffer, uint64_t out_buffer_size, bool allow_smaller_buffer_size);


#endif // HTTP_H_