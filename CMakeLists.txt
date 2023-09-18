cmake_minimum_required(VERSION 3.21)
project(NetworkProcedure C)

#set(CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 17)

#add_executable(Tiny csapp.h csapp.c Tiny.c )
#add_executable(echoClient  csapp.h csapp.c  echoClient.c)

##demo
##add_executable(adder include/csapp.h network/csapp.c network/adder.c network/multipleIO.c network/echoServers.c include/macro.h)

##Unit test
#add_executable(main include/csapp.h main.c)
add_executable(main include/csapp.h network/Client/echoClient.c network/csapp.c)


