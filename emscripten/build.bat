@call emcc -sUSE_GLFW=3 -sFULL_ES3=1 -sSTB_IMAGE=1 -sWASM=1 -std=c++2b -Os -Wall -fno-rtti -fno-exceptions -s -s FORCE_FILESYSTEM=1 -o ../bin/game.html --preload-file ../bin/data/@data/ -I. -I../src/3rdparty/ ../src/Game/Engine.cpp ../src/Game/GameApp.cpp ../src/Game/main.cpp

erase "../bin/index.html"
ren "../bin/game.html" "../bin/index.html"