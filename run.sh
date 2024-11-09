#!/bin/bash

clang server.c -Wall -g -o server &&
clang client.c -Wall -g -o client 
 


# clang -fPIC -shared -o libserver.so server.c

# clang -o main main.c &&
# ./main
