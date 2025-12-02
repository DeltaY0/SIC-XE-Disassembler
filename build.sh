output=bin/ysicxe
includes=-Isrc/*/

flags="--std=c++17"

g++ $includes $flags src/*.cpp src/*/*.cpp -o $output