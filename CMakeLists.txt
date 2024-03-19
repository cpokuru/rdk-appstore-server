cmake_minimum_required(VERSION 3.0)
project(RDK_Appstore_Server)

# Set the list of source files
set(SOURCE_FILES 
    main.c
    server.c
    file_handling.c
    request_handling.c
)

# Set the list of header files
set(HEADER_FILES 
    server.h
    file_handling.h
    request_handling.h
)

# Create an executable from the source files
add_executable(rdk-appstore-server ${SOURCE_FILES} ${HEADER_FILES})

# Link against the Microhttpd library
target_link_libraries(rdk-appstore-server microhttpd)

