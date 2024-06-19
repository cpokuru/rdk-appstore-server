#include "appstoreserver.h"
#include "microhttpd_wrapper.h"

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

void processCurlCommand(const char *appId, const char *outputFileName) {
    FILE *fp;
    FILE *outputFile;
    char path[MAX_BUFFER_SIZE];

    // Construct the command with the provided application ID
    char command[MAX_BUFFER_SIZE];
    //ckpX snprintf(command, sizeof(command), "curl --location --request GET 'http://localhost:8082/asms/apps?id=%s&maintainerName=Liberty%%20Global1' --header 'Content-Type: application/json' --header 'Authorization: Basic c3RiOnN0Yg==' --data-raw '' | jq '.applications[] | select(.id == \"%s\")'", appId, appId);
    snprintf(command, sizeof(command), "curl -X GET http://127.0.0.1:5000/maintainers/rdk/apps/%s", appId);


    // Open pipe to execute command
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute command\n" );
        exit(1);
    }

    // Open the output file for writing
    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        printf("Failed to open output file\n");
        exit(1);
    }

    // Read the output of the command and write it to the output file
    while (fgets(path, sizeof(path), fp) != NULL) {
        fprintf(outputFile, "%s", path);
    }

    // Close the output file
    fclose(outputFile);

    // Close the pipe
    pclose(fp);
}

