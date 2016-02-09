#!/bin/sh

valgrind --log-file=valgrind-%p.log --trace-children=yes --leak-check=full --show-leak-kinds=all bin/httpd -k start -X
