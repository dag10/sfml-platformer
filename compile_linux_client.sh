#!/bin/sh
# SUPER-DUPER PRIMITIVE MAKEFILE HOORAY!

mkdir obj
mkdir bin

cd obj

echo "COMPILING"
g++ -Wall -c ../main_client.cpp ../src/*.cpp ../src/cpGUI/*.cpp -I../include/ -I../include/cpGUI/ -DPLATFORMER_CLIENT
#g++ -Wall -c ../main_client.cpp ../src/*.cpp -I../include/

echo "LINKING"
g++ *.o -o ../bin/Platformer_Client -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

echo "CLEANING"
cd ..
rm obj -rf

if [ "$1" = "run" ]
then
echo "RUNNING"
bin/Platformer_Client
fi
