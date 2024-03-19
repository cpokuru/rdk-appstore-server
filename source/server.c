#include "server.h"

#include "file_handling.h"
#include "request_handling.h"

char *get_interface_ip(const char *interface_name) {
	char command[256];
	FILE *pipe;
	char *ip_address = NULL;

	// Construct the command to get IP address of the interface
	snprintf(command, sizeof(command), "ip addr show %s | grep -Po 'inet \\K[\\d.]+'", interface_name);

	// Open the command for reading
	pipe = popen(command, "r");
	if (pipe == NULL) {
    	perror("popen failed");
    	return NULL;
	}

	// Read the output of the command
	ip_address = (char *)malloc(16 * sizeof(char)); // IPv4 address maximum length is 15 characters
	if (ip_address == NULL) {
    	perror("malloc failed");
    	pclose(pipe);
    	return NULL;
	}

	if (fgets(ip_address, 16, pipe) == NULL) {
    	perror("fgets failed");
    	free(ip_address);
    	pclose(pipe);
    	return NULL;
	}

	// Remove newline character from the end of the IP address string
	ip_address[strcspn(ip_address, "\n")] = '\0';

	// Close the pipe and return the IP address
	pclose(pipe);
	return ip_address;

}


void start_server() {
    struct MHD_Daemon *daemon;
    char *host_ip;
    char *interface_name = "eno2"; // Specify the interface name here
    host_ip = get_interface_ip(interface_name);
    if (host_ip == NULL) {
    	printf("Error: Failed to get IP address of interface %s\n", interface_name);
    	return 1;
    }

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                               &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        printf("Error: Failed to start the server\n");
        exit(EXIT_FAILURE);
    }
    printf("Server running at IP address %s on port %d\n", host_ip, PORT);
    //printf("Server running on port %d\n", PORT);
    printf("Press enter to stop the server\n");
    getchar();  // wait for user input to stop the server

    MHD_stop_daemon(daemon);
}

