cmake_minimum_required(VERSION 3.0)
project(RDK_Appstore_Server)

# Add your source file
set(SOURCE_FILES rdk-appstore-server.c)

# Create an executable from the source file
add_executable(rdk-appstore-server ${SOURCE_FILES})
# Link against the Microhttpd library
target_link_libraries(rdk-appstore-server microhttpd)

