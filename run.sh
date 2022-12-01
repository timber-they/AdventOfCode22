#!/bin/bash

gcc $1.c -Ofast -Werror -Wextra -Wall -fsanitize=address -g -lssl -lcrypto && ./a.out
