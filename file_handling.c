#include "server.h"
#include "file_handling.h"

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

int write_data_to_file(const char *data, size_t data_size) {
        FILE *file = fopen(DATA_FILE, "w"); 
	if (!file) { 
    	return 0; // Error opening file 
	} 
 
	// Write data to file 
	size_t bytes_written = fwrite(data, 1, data_size, file); 
	fclose(file); 
 
	return bytes_written == data_size; // Return success if all data was written 

}

