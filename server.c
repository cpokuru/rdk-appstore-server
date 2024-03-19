#include "server.h"

#include "file_handling.h"
#include "request_handling.h"

void start_server() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                               &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        printf("Error: Failed to start the server\n");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d\n", PORT);
    printf("Press enter to stop the server\n");
    getchar();  // wait for user input to stop the server

    MHD_stop_daemon(daemon);
}

