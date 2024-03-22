#include <stdio.h>
#include "microhttpd_wrapper.h"

#define SERVER_ADDRESS "http://192.168.0.1:8585"
#define PORT 8999 
#define DATA_FILE "received_data.txt" 
#define OUTPUT_FILE "/home/rdkm/ckp/dacwh/newc/output1.json"
#define MAX_BUFFER_SIZE 4096

int main() {
    char *host_ip;
    char *interface_name = "wlo1"; // Specify the interface name here

    // Get IP address of the specified interface
    host_ip = get_interface_ip(interface_name);
    if (host_ip == NULL) {
        printf("Error: Failed to get IP address of interface %s\n", interface_name);
        return 1;
    }
    return start_microhttpd_server();
}

