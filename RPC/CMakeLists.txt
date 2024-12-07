cmake_minimum_required(VERSION 3.10)
project(RPC)

# Setări generale
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directoarele pentru header-uri
include_directories(${CMAKE_SOURCE_DIR}/include)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

set(PROTO_FILE src/proto/procedure_format.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})

# Colectarea fișierelor sursă
file(GLOB_RECURSE SOURCES
    src/client/rpc_client.cpp
    src/common/errors.cpp
    src/server/rpc_server.cpp
    src/transport/socket_connection.cpp
    src/transport/server_socket.cpp 
    src/transport/client_socket.cpp
    src/client/base_client.cpp 
    src/client/base_client_async.cpp 
    src/client/rpc_client_async.cpp
    src/common/encryption.cpp 
    src/server/server_functions.cpp
    ${PROTO_SRCS}
)

file(GLOB_RECURSE HEADERS
    include/RPC/errors.h
    include/RPC/rpc_client.h
    include/RPC/rpc_server.h
    include/RPC/socket_connection.h
    include/RPC/server_socket.h 
    include/RPC/client_socket.h
    include/RPC/base_client.h 
    include/RPC/functions.h 
    include/RPC/procedure_format.pb.h
    include/RPC/encryption.h 
    ${PROTO_HDRS}
)

# Crearea unei biblioteci dinamice (shared library)
add_library(RPC SHARED ${SOURCES} ${HEADERS})

# Setarea directoarelor de destinație pentru biblioteca dinamică și header-uri
set_target_properties(RPC PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Specificarea header-urilor publice pentru utilizatorii bibliotecii
target_include_directories(RPC PUBLIC include ${Protobuf_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS})

target_link_libraries(RPC PUBLIC ${Protobuf_LIBRARIES} ${OPENSSL_LIBRARIES})