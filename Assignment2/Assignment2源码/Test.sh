#!/bin/bash

make clean

make

echo -e "Test Start!\n\n"

echo "Test1"
./parser test/test1.c
echo -e "\n\n"

echo "Test2"
./parser test/test2.c
echo -e "\n\n"

echo "Test3"
./parser test/test3.c
echo -e "\n\n"

echo "Test4"
./parser test/test4.c
echo -e "\n\n"

echo "Test5"
./parser test/test5.c
echo -e "\n\n"

echo "Test6"
./parser test/test6.c
echo -e "\n\n"

echo "Test7"
./parser test/test7.c
echo -e "\n\n"

echo "Test8"
./parser test/test8.c
echo -e "\n\n"

echo "Test9"
./parser test/test9.c
echo -e "\n\n"

echo "Test10"
./parser test/test10.c
echo -e "\n\n"

echo "Test11"
./parser test/test11.c
echo -e "\n\n"

echo "Test12"
./parser test/test12.c
echo -e "\n\n"

echo "Test13"
./parser test/test13.c
echo -e "\n\n"

echo "Test14"
./parser test/test14.c
echo -e "\n\n"

echo "Test15"
./parser test/test15.c
echo -e "\n\n"

echo "Test16"
./parser test/test16.c
echo -e "\n\n"

echo "Test17"
./parser test/test17.c
echo -e "\n\n"

echo -e "END\n\n"

make clean
