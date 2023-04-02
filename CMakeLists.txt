cmake_minimum_required(VERSION 3.2)

project(hl_lab_01 C CXX)

SET (EXAMPLE_BINARY "server")

include(./common.cmake)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data)


if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

# Proto file
get_filename_component(au_proto "./user.proto" ABSOLUTE)
get_filename_component(au_proto_path "${au_proto}" PATH)

# Generated sources
set(au_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/user.pb.cc")
set(au_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/user.pb.h")
set(au_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/user.grpc.pb.cc")
set(au_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/user.grpc.pb.h")

message(STATUS "Execute custom command ")
add_custom_command(
      OUTPUT "${au_proto_srcs}" "${au_proto_hdrs}" "${au_grpc_srcs}" "${au_grpc_hdrs}"
      COMMAND protoc
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${au_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${au_proto}"
      DEPENDS "${au_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(au_grpc_proto
  ${au_grpc_srcs}
  ${au_grpc_hdrs}
  ${au_proto_srcs}
  ${au_proto_hdrs})
target_link_libraries(au_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})


add_executable(client client.cpp)
add_executable(${EXAMPLE_BINARY} main.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/user.cpp)


target_include_directories(${EXAMPLE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${EXAMPLE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )

target_link_libraries(client PRIVATE 
                             au_grpc_proto
                             ${_REFLECTION}
                             ${_GRPC_GRPCPP}
                             ${_PROTOBUF_LIBPROTOBUF})

target_link_libraries(${EXAMPLE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             au_grpc_proto
                             ${_REFLECTION}
                             ${_GRPC_GRPCPP}
                             ${_PROTOBUF_LIBPROTOBUF})


set_target_properties(${EXAMPLE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${EXAMPLE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

