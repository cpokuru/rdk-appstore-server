#ifndef APPSTOREHEADER_H
#define APPSTOREHEADER_H

#include <jansson.h>

json_t *readJsonFromFile(const char *filename);
void processCurlCommand(const char *appId, const char *outputFileName);
char *get_interface_ip(const char *interface_name);
void convert_oci_path(char *ociurl);
void invokeScript(const char *filename, const char *platform_value);
char *read_data_from_file();
int write_data_to_file(const char *data, size_t data_size);
const char *get_server_address();
const char *get_output_file_path();

#endif /* APPSTOREHEADER_H */

