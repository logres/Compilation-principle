#!/bin/bash

make clean

make

echo -e "Test Start!\n\n"

echo "Test1"
./parser test_example/test1.c
echo -e "\n\n"

echo "Test2"
./parser test_example/test2.c
echo -e "\n\n"

echo "Test3"
./parser test_example/test3.c
echo -e "\n\n"

echo "Test4"
./parser test_example/test4.c
echo -e "\n\n"

make clean
