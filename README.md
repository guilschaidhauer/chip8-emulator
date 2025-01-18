# chip8-emulator

Simple Chip8 Emulator

To build:
g++ -std=c++11 -o emulator main.cpp -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit -lpthread -ldl

To build with cmake

mkdir build
cd build

cmake ..

make

./emulator