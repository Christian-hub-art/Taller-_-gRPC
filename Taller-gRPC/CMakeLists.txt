cmake_minimum_required(VERSION 3.15)
project(taller_grpc CXX)

set(CMAKE_CXX_STANDARD 17)

# Configurar gRPC con FetchContent: descarga y compila gRPC desde su
# repositorio oficial (no requiere instalarlo manualmente en el sistema,
# aunque en Linux igual necesitas las herramientas de compilación y protoc).
set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(gRPC_INSTALL OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
  grpc
  GIT_REPOSITORY https://github.com/grpc/grpc.git
  GIT_TAG        v1.54.3
  GIT_SUBMODULES_RECURSE TRUE
)
FetchContent_MakeAvailable(grpc)

# --- Generación de código a partir de estudiantes.proto ---
# protoc + el plugin grpc_cpp_plugin generan 4 archivos:
#   estudiantes.pb.h / .pb.cc       -> clases de los mensajes (IdRequest, etc.)
#   estudiantes.grpc.pb.h / .pb.cc  -> stub del cliente y clase base del servicio
set(proto_file "${CMAKE_CURRENT_SOURCE_DIR}/estudiantes.proto")
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/estudiantes.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/estudiantes.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/estudiantes.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/estudiantes.grpc.pb.h")

add_custom_command(
  OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
  COMMAND protoc
  ARGS --grpc_out="${CMAKE_CURRENT_BINARY_DIR}"
       --cpp_out="${CMAKE_CURRENT_BINARY_DIR}"
       --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
       -I "${CMAKE_CURRENT_SOURCE_DIR}"
       "${proto_file}"
  DEPENDS "${proto_file}" grpc_cpp_plugin protoc
)

# --- Ejecutable del servidor ---
add_executable(servidor server.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(servidor PRIVATE grpc++ libprotobuf)
target_include_directories(servidor PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# --- Ejecutable del cliente ---
add_executable(cliente client.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(cliente PRIVATE grpc++ libprotobuf)
target_include_directories(cliente PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
