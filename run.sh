#!/bin/bash

gcc $1.c -Ofast -Werror -Wextra -Wall -static-libasan -g && ./a.out
