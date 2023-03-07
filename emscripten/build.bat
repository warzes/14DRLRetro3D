@call emcc                                                                  ^
	-sUSE_GLFW=3 -sFULL_ES3=1 -sSTB_IMAGE=1 -sWASM=1 -sFORCE_FILESYSTEM=1   ^
	-std=c++2b -O3 -Wall -fno-rtti -fno-exceptions                          ^
	-o ../bin/game.html                                                     ^
	--preload-file ../bin/data/@data/                                       ^
	-I../src/3rdparty/                                                      ^
		../src/Game/Engine.cpp                                              ^
		../src/Game/GameApp.cpp                                             ^
		../src/Game/main.cpp

erase "../bin/index.html"
ren "../bin/game.html" "../bin/index.html"