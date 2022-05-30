#!/bin/bash

make clean

make

echo -e "Start\n\n"

echo -e "Test 1 to code1.ir\n\n"

./parser test/test1.c > test/code1.ir

echo -e "Test 2 to code2.ir\n\n"

./parser test/test2.c > test/code2.ir

echo -e "END\n\n"

make clean
