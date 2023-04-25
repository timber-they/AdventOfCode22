#!/bin/bash

gcc $1.c -Ofast -Werror -Wextra -Wall -static-libasan -fsanitize=address -g && ./a.out
