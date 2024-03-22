#ifndef MICROHTTPD_WRAPPER_H
#define MICROHTTPD_WRAPPER_H

#include <microhttpd.h>


#define SERVER_ADDRESS "http://192.168.0.1:8585"
#define PORT 8999 
#define DATA_FILE "received_data.txt" 
#define OUTPUT_FILE "/home/rdkm/ckp/dacwh/newc/output1.json"
#define MAX_BUFFER_SIZE 4096


int start_microhttpd_server();

#endif /* MICROHTTPD_WRAPPER_H */

