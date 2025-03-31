cmake_minimum_required(VERSION 3.5)
project(tcp_socket)
set(CMAKE_CXX_STANDARD 11)

add_executable(server server_tcp.cc)
add_executable(client client_tcp.cc)
target_link_libraries(server pthread)
target_link_libraries(client pthread)