#!/bin/bash

gcc $1.c -Ofast -Werror -Wall -lssl -lcrypto && time ./a.out
