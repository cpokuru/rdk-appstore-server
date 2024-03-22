#ifndef REQUEST_HANDLING_H
#define REQUEST_HANDLING_H

#include <microhttpd.h>

void convert_oci_path(char *ociurl);
void invokeScript(const char *filename,const char *platform_value);
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls);

#endif /* REQUEST_HANDLING_H */

