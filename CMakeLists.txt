cmake_minimum_required(VERSION 3.0)
project(RDK_Appstore_Server)

# Set the list of source files
set(SOURCE_FILES
	source/main.c
	source/server.c
	source/file_handling.c
	source/request_handling.c
)

# Specify the include directories
include_directories(include)

# Create an executable from the source files
add_executable(rdk-appstore-server ${SOURCE_FILES} ${HEADER_FILES})

# Link against the Microhttpd library
target_link_libraries(rdk-appstore-server microhttpd)

