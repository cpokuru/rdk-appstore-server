#include "microhttpd_wrapper.h"
#include "appstoreserver.h"

#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define PORT 8999


// Buffer to store the response
struct MemoryStruct {
    char *memory;
    size_t size;
};

static const char *data_received = ""; // Variable to store received data 

// Callback function to write the response into the buffer
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
// Function to fetch the app name from the given app ID
char* fetch_app_name(const char *app_id) {
    char url[256];
    snprintf(url, sizeof(url), "http://127.0.0.1:5000/maintainers/rdk/apps/%s", app_id);

    CURL *curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Will be grown as needed by the realloc above
    chunk.size = 0;    // No data at this point

    curl_global_init(CURL_GLOBAL_ALL);

    // Initialize a CURL session
    curl_handle = curl_easy_init();

    if(!curl_handle) {
        fprintf(stderr, "Failed to initialize CURL\n");
        free(chunk.memory);
        curl_global_cleanup();
        return NULL;
    }

    // Set the URL
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    // Send all data to this function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    // Pass the chunk struct to the callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    // Perform the request, res will get the return code
    res = curl_easy_perform(curl_handle);

    // Check for errors
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);
        curl_global_cleanup();
        return NULL;
    }

    // Parse the JSON response
    cJSON *json = cJSON_Parse(chunk.memory);
    char *name = NULL;

    if(json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    } else {
        // Navigate to the "name" field
        cJSON *header = cJSON_GetObjectItem(json, "header");
        if (header != NULL) {
            cJSON *name_item = cJSON_GetObjectItem(header, "name");
            if (cJSON_IsString(name_item) && (name_item->valuestring != NULL)) {
                name = strdup(name_item->valuestring); // Duplicate the name string to return
            } else {
                fprintf(stderr, "No valid 'name' field found.\n");
            }
        } else {
            fprintf(stderr, "No 'header' field found.\n");
        }
        // Clean up JSON object
        cJSON_Delete(json);
    }

    // Clean up CURL and memory
    curl_easy_cleanup(curl_handle);
    free(chunk.memory);

    // Clean up CURL globally
    curl_global_cleanup();

    return name;
}


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
	char *app_name = fetch_app_name(id);
        printf("app_name is %s\n",app_name);
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
        snprintf(file_path, sizeof(file_path), "/home/rdkm/%s.tar.gz", app_name);
        snprintf(file_path_bundle, sizeof(file_path_bundle), "/home/rdkm/BundleGen/%s/%s.tar", app_name,app_name);

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

            const char *success_message = "File path exists ckp";
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
            invokeScript(app_name, platform_value);
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

