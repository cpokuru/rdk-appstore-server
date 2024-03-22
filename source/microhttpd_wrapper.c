#include "microhttpd_wrapper.h"
#include "appstoreserver.h"

#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8999

static const char *data_received = ""; // Variable to store received data 

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    struct MHD_Response *response = NULL; // Declare response variable 
    char converted_path[256];
    if (0 == strcmp(method, MHD_HTTP_METHOD_POST)) {
        printf("Received a POST request\n");

        // Allocate memory for the new data 
        data_received = (const char *)malloc(*upload_data_size + 1);
        if (!data_received) {
            printf("Error: Memory allocation failed\n");
            return MHD_NO;
        }

        // Copy the new data 
        strncpy((char *)data_received, upload_data, *upload_data_size);
        ((char *)data_received)[*upload_data_size] = '\0'; // Null-terminate the string 

        printf("Before file write \n");
        if (!write_data_to_file(upload_data, *upload_data_size)) {
            printf("Error: Failed to write data to file\n");
            return MHD_NO;
        }

        printf("Data received in POST request: %s\n", data_received);
        return MHD_YES;
    } else if (0 == strcmp(method, MHD_HTTP_METHOD_GET)) {
        printf("Received a GET request\n");
        
        // Extract parameters from URL
        const char *id = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "id");
        const char *platform = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "platform");

        printf("ID: %s\n", id);
        printf("Platform: %s\n", platform);
        const char *platform_value = strdup(platform);

        const char *outputFileName = OUTPUT_FILE;
        processCurlCommand(id, outputFileName);
        json_t *root = readJsonFromFile(outputFileName);
        if (!root) {
            printf("Failed to read JSON from file\n");
            return MHD_NO;
        }

        json_t *name = json_object_get(root, "name");
        if (!json_is_string(name)) {
            printf("Failed to extract name from JSON\n");
            json_decref(root);
            return MHD_NO;
        }

        printf("Name of bundle based on id is : %s\n", json_string_value(name));
        const char *name_value = json_string_value(name);
        char *idname = NULL;

        if (name_value != NULL) {
            idname = strdup(name_value);
            if (idname == NULL) {
                printf("Error: Memory allocation failed\n");
            } else {
                printf("Copied value: %s\n", idname);
            }
        } else {
            printf("Error: Null value in name\n");
        }
        json_decref(root);

        if (!id || strlen(id) == 0) {
            printf("Error: id parameter is missing\n");

            const char *error_message = "id parameter is missing";
            response = MHD_create_response_from_buffer(strlen(error_message), (void *)error_message,
                                                        MHD_RESPMEM_PERSISTENT);
            if (!response)
                return MHD_NO;

            int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
            MHD_destroy_response(response);
            return ret;
        }

        char file_path[256];
        char file_path_bundle[256];
        snprintf(file_path, sizeof(file_path), "/home/rdkm/%s.tar.gz", idname);
        snprintf(file_path_bundle, sizeof(file_path_bundle), "/home/rdkm/bundle/BundleGen/bundle2/%s.tar", idname);

        printf("file path  %s\n",file_path);
        if (access(file_path, F_OK) != -1) {
            printf("File %s exists\n", file_path);
            char full_url[512];
            snprintf(full_url,sizeof(full_url),"%s%s",SERVER_ADDRESS,file_path);
            printf("Resp is %s\n",full_url);

            response = MHD_create_response_from_buffer(strlen(full_url), (void *)full_url,
                                                        MHD_RESPMEM_PERSISTENT);
            if (!response)
                return MHD_NO;

            int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);
            return ret;
        }

        printf("file path bundle %s\n",file_path_bundle);
        if (access(file_path_bundle, F_OK) != -1) {
            printf("File %s exists\n", file_path_bundle);

            const char *success_message = "/home/rdkm/bundle/BundleGen/bundle2/";
            response = MHD_create_response_from_buffer(strlen(success_message), (void *)success_message,
                                                        MHD_RESPMEM_PERSISTENT);
            if (!response)
                return MHD_NO;

            int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);
            return ret;
        }
        else{
            printf("File not found, invoking script to generate bundle\n");
            invokeScript(idname, platform_value);
        }

        const char *pending_message = "Bundle will be created soon";
        response = MHD_create_response_from_buffer(strlen(pending_message), (void *)pending_message,
                                                    MHD_RESPMEM_PERSISTENT);
        if (!response)
            return MHD_NO;

        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    } else {
        printf("Error: Unsupported method\n");
        return MHD_NO;
    }
}

int start_microhttpd_server() {
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                            (MHD_AccessHandlerCallback) &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        printf("Error: Failed to start the server\n");
        return 1;
    }
    printf("Server running on port %d\n", PORT);
    printf("Press enter to stop the server\n");
    getchar();  // wait for user input to stop the server
    MHD_stop_daemon(daemon);
    return 0;
}

