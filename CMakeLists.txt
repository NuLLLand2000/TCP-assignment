cmake_minimum_required(VERSION 3.19)


project(assignment C)

set(CMAKE_C_STANDARD 99)

add_executable(sendmetcp.c receivemetcp.c md5.c)

