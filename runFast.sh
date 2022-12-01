#!/bin/bash

gcc $1.c -Ofast -Werror -Wall && time ./a.out
