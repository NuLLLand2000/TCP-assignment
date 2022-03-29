cmake_minimum_required(VERSION 3.19)


project(assignment C)

set(CMAKE_C_STANDARD 99)

add_executable( main.c server.c client.c sendmetcp.c receivemetcp.c md5.c)

