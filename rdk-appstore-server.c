#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define PORT 8999
#define DATA_FILE "received_data.txt"
 
static const char *data_received = ""; // Variable to store received data
 
void invokeScript(const char *filename) {
	char scriptCmd[256];
	snprintf(scriptCmd, sizeof(scriptCmd), "/home/rdkm/ociimages/bundle.sh %s", filename);
	int resp = system(scriptCmd);
	if (resp == -1) {
    	printf("Error: Failed to execute script\n");
	} else {
    	printf("Script executed successfully\n");
    	// Handle further logic after script execution if needed
	}
}
 
// Function to read data from file
char *read_data_from_file() {
    FILE *file = fopen(DATA_FILE, "r");
    if (!file) {
   	 return NULL; // File does not exist
    }
 
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
 
    // Allocate memory for data
    char *data = (char *)malloc(file_size + 1);
    if (!data) {
   	 fclose(file);
   	 return NULL; // Memory allocation failed
    }
 
    // Read data from file
    size_t bytes_read = fread(data, 1, file_size, file);
    if (bytes_read != file_size) {
   	 free(data);
   	 fclose(file);
   	 return NULL; // Error reading data
    }
 
    // Null-terminate the string
    data[file_size] = '\0';
 
    fclose(file);
    return data;
}
 
// Function to write data to file
int write_data_to_file(const char *data,size_t data_size) {
    FILE *file = fopen(DATA_FILE, "w");
    if (!file) {
   	 return 0; // Error opening file
    }
 
    // Write data to file
    size_t bytes_written = fwrite(data, 1, data_size, file);
    fclose(file);
 
    return bytes_written == data_size; // Return success if all data was written
}
 
 
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                            	const char *url, const char *method,
                            	const char *version, const char *upload_data,
                            	size_t *upload_data_size, void **con_cls) {
	struct MHD_Response *response = NULL; // Declare response variable

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
    	const char *ociurl = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ociurl");
    	const char *platform = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "platform");

    	printf("OCI URL: %s\n", ociurl);
    	printf("Platform: %s\n", platform);

    	// Check if ociurl parameter exists
    	if (!ociurl) {
        	// ociurl parameter missing
        	printf("Error: ociurl parameter is missing\n");

        	// Respond with an error message
        	const char *error_message = "ociurl parameter is missing";
        	response = MHD_create_response_from_buffer(strlen(error_message), (void *)error_message,
                                                    	MHD_RESPMEM_PERSISTENT);
        	if (!response)
            	return MHD_NO;

        	int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        	MHD_destroy_response(response);
        	return ret;
    	}

    	// Extract bundle name from ociurl
    	char *bundle_name_start = strrchr(ociurl, '/');
    	if (bundle_name_start == NULL) {
        	printf("Error: Invalid ociurl\n");

        	// Respond with an error message
        	const char *error_message = "Invalid ociurl";
        	response = MHD_create_response_from_buffer(strlen(error_message), (void *)error_message,
                                                    	MHD_RESPMEM_PERSISTENT);
        	if (!response)
            	return MHD_NO;

        	int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        	MHD_destroy_response(response);
        	return ret;
    	}

    	// Remove the .tar extension from the bundle name
    	char bundle_name[256]; // Adjust the size as per your requirements
    	strcpy(bundle_name, bundle_name_start + 1); // Skip the '/'
    	char *extension_start = strstr(bundle_name, ".tar");
    	if (extension_start == NULL) {
        	printf("Error: Invalid ociurl\n");

        	// Respond with an error message
        	const char *error_message = "Invalid ociurl";
        	response = MHD_create_response_from_buffer(strlen(error_message), (void *)error_message,
                                                    	MHD_RESPMEM_PERSISTENT);
        	if (!response)
            	return MHD_NO;

        	int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        	MHD_destroy_response(response);
        	return ret;
    	}
    	*extension_start = '\0'; // Null-terminate the bundle name

    	// Construct the file path
    	char file_path[256]; // Adjust the size as per your requirements
    	snprintf(file_path, sizeof(file_path), "/home/rdkm/%s.tar.gz", bundle_name);

    	// Check if the file exists
    	if (access(file_path, F_OK) != -1) {
        	// File exists
        	printf("%s exists\n", file_path);
#if 0// Get the directory path
	char *dir_path = strdup(file_path);
	if (dir_path == NULL) {
    	printf("Error: Memory allocation failed\n");
    	return MHD_NO;
	}
	char *last_slash = strrchr(dir_path, '/');
	if (last_slash != NULL) {
    	*last_slash = '\0'; // Null-terminate at the last slash to remove the file name
	}
	// Create response with the directory path as text
	size_t dir_path_len = strlen(dir_path);

#endif
// Find the last occurrence of '/' in the file path
	char *last_slash = strrchr(file_path, '/');
	if (last_slash == NULL) {
    	printf("Error: Invalid file path\n");
    	return MHD_NO;
	}

	printf("Last slash: %s\n", last_slash);

	// Calculate the length of the directory path
	size_t dir_path_len = last_slash - file_path + 1; // Include the last '/'

	printf("Directory path length: %zu\n", dir_path_len);

	// Allocate memory for the directory path
	char *dir_path = (char *)malloc(dir_path_len + 1); // Add 1 for null terminator
	if (dir_path == NULL) {
    	printf("Error: Memory allocation failed\n");
    	return MHD_NO;
	}

	// Copy the directory path
	strncpy(dir_path, file_path, dir_path_len);
	dir_path[dir_path_len] = '\0'; // Null-terminate the string

	printf("Directory path: %s\n", dir_path);
// Create response with the directory path
	response = MHD_create_response_from_buffer(dir_path_len, (void *)dir_path, MHD_RESPMEM_MUST_COPY);
	if (!response) {
    	printf("Error: Failed to create response\n");
    	free(dir_path);
    	return MHD_NO;
	}

	// Queue the response
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	printf("Responding to GET request with directory path: %s\n", dir_path);

	// Free memory for the directory path
	free(dir_path);

	return ret;
#if 0
// Create response with the file path as text
	response = MHD_create_response_from_buffer(strlen(file_path), (void *)file_path, MHD_RESPMEM_PERSISTENT);
	if (!response) {
    	printf("Error: Failed to create response\n");
    	return MHD_NO;
	}

	// Queue the response
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	printf("Responding to GET request with file path: %s\n", file_path);
	return ret;
#endif
#if tartoshare
       	// Read the contents of the file into a buffer
        	FILE *file = fopen(file_path, "rb");
        	if (!file) {
            	printf("Error: Failed to open file\n");
            	return MHD_NO;
        	}

        	fseek(file, 0, SEEK_END);
        	size_t file_size = ftell(file);
        	fseek(file, 0, SEEK_SET);

        	char *file_buffer = (char *)malloc(file_size);
        	if (!file_buffer) {
            	fclose(file);
            	printf("Error: Memory allocation failed\n");
            	return MHD_NO;
        	}

        	fread(file_buffer, 1, file_size, file);
        	fclose(file);

        	// Create response with the file contents
        	response = MHD_create_response_from_buffer(file_size, (void *)file_buffer, MHD_RESPMEM_MUST_COPY);
        	if (!response) {
            	printf("Error: Failed to create response\n");
            	free(file_buffer);
            	return MHD_NO;
        	}

        	// Queue the response
        	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        	MHD_destroy_response(response);

        	free(file_buffer);

        	printf("Responding to GET request with file: %s\n", file_path);
        	return ret;
#endif
#ifdef APINS
	// Create response with the file path
	response = MHD_create_response_from_file(file_path);
	if (!response) {
    	printf("Error: Failed to create response\n");
    	return MHD_NO;
	}

	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	printf("Responding to GET request with file: %s\n", file_path);
	return ret;
#endif
#if 0        	// Create response with the specified URL
        	response = MHD_create_response_from_buffer(strlen(url), (void *)url, MHD_RESPMEM_PERSISTENT);
        	if (!response)
            	return MHD_NO;

        	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        	MHD_destroy_response(response);

        	printf("Responding to GET request with URL: %s\n", url);
        	return ret;
#endif    
	} else {
        	// File does not exist
        	printf("%s does not exist\n", file_path);

        	// Respond with an error message
        	const char *error_message = "File not found try in 30 mins,we will create a bundle";
        	response = MHD_create_response_from_buffer(strlen(error_message), (void *)error_message,
                                                    	MHD_RESPMEM_PERSISTENT);
        	if (!response)
            	return MHD_NO;

        	int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
        	MHD_destroy_response(response);
 // Extract the filename from the OCI URL
	const char *filename = strrchr(ociurl, '/');
	if (filename == NULL) {
    	printf("Error: Invalid OCI URL\n");
    	return 1;
	}
	filename++; // Skip the '/'

	// Check if the file exists
	if (access(ociurl, F_OK) != -1) {
    	printf("%s exists\n", ociurl);
    	// Your logic if file exists
	} else {
    	printf("%s does not exist\n", ociurl);
    	// Invoke the script and pass the filename as an argument
    	invokeScript(filename);
   }    
#if 0
        	int resp = system("/home/rdkm/ociimages/script.sh");
        	if (resp == 0) {
   	 printf("shell script executed \n");
        	}
#endif
        	return ret;
    	}
	} else {
    	// Unsupported method
    	printf("Error: Unsupported method\n");
    	return MHD_NO;
	}
}
 
int main() {
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
 

