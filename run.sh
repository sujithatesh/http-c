#!/bin/bash

clang server.c -g -o server &&
clang client.c -g -o client 
 


# clang -fPIC -shared -o libserver.so server.c

# clang -o main main.c &&
# ./main
