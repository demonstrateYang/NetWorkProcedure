cmake_minimum_required(VERSION 3.21)
project(NetworkProgramingInC C)

set(CMAKE_C_STANDARD 99)

#add_executable(Tiny csapp.h csapp.c Tiny.c )
#add_executable(echoClient  csapp.h csapp.c  echoClient.c)
add_executable(adder include/csapp.h network/csapp.c network/adder.c network/multipleIO.c network/echoServers.c)


