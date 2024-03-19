#ifndef SERVER_H
#define SERVER_H

#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8999
#define DATA_FILE "received_data.txt"

void start_server();

#endif /* SERVER_H */

