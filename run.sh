#!/bin/bash

gcc $1.c -Ofast -Werror -Wextra -Wall -fsanitize=address -g && ./a.out
