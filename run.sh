#!/bin/bash

clang server.c -Wall -g -o server &&
clang ./tpool.h -o tpool
 


# clang -fPIC -shared -o libserver.so server.c

# clang -o main main.c &&
# ./main
