cmake_minimum_required(VERSION 3.0)
project(RDK_Appstore_Server)

# Set the list of source files
set(SOURCE_FILES 
    source/main.c
    source/file_operations.c
    source/json_operations.c
    source/microhttpd_wrapper.c
    source/network_operations.c
)

# Set the list of header files
#set(HEADER_FILES 
#    server.h
#    file_handling.h
#    request_handling.h
#)

# Specify the include directories
include_directories(include)

# Create an executable from the source files
add_executable(rdk-appstore-server ${SOURCE_FILES} ${HEADER_FILES})

# Link against the Microhttpd library
#target_link_libraries(rdk-appstore-server microhttpd)
#target_link_libraries(rdk-appstore-server jansson)

# Link with CURL library
target_link_libraries(rdk-appstore-server ${CURL_LIBRARIES})

# Add cJSON library (assuming you have cJSON installed)
find_package(cJSON REQUIRED)

# Include cJSON directory
include_directories(${CJSON_INCLUDE_DIRS})


# Link with cJSON library
target_link_libraries(rdk-appstore-server ${CJSON_LIBRARIES})
target_link_libraries(rdk-appstore-server microhttpd)
target_link_libraries(rdk-appstore-server curl)
target_link_libraries(rdk-appstore-server jansson)
