#!/bin/sh
# SUPER-DUPER PRIMITIVE MAKEFILE HOORAY!

mkdir obj
mkdir bin

cd obj

echo "COMPILING"
g++ -Wall -c ../main.cpp ../src/*.cpp -I../include/

echo "LINKING"
g++ *.o -o ../bin/Platformer -lsfml-graphics -lsfml-window -lsfml-system

echo "CLEANING"
cd ..
rm obj -rf

if [ "$1" = "run" ]
then
echo "RUNNING"
bin/Platformer
fi
