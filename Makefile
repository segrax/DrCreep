pathInc = -I/usr/include/directfb/direct -I/usr/include/directfb -I./src
Libs = `sdl2-config --cflags` -L/usr/local/lib
DLibs = `sdl2-config --libs` -L/usr/local/lib
 
CC = g++ -c -Wall $(pathInc) $(Libs)
LD = g++ obj/*.o $(DLibs)

all : vic sid graphics castle main creep

vic:

	$(CC) src/vic-ii/bitmapMulticolor.cpp src/vic-ii/screen.cpp src/vic-ii/sprite.cpp

sid:
	$(CC) src/sound/sound.cpp src/resid-0.16/*.cpp

creep : main
	mkdir -p obj
	mv *.o obj/
	$(LD) -o run/creep


graphics :
	$(CC) src/graphics/screenSurface.cpp src/graphics/window.cpp

castle :
	$(CC) src/castle/castle.cpp  src/castle/room.cpp src/castle/objects/*.cpp 

main :
	$(CC) src/castleManager.cpp src/stdafx.cpp src/creep.cpp src/d64.cpp src/debug.cpp src/builder.cpp src/playerInput.cpp src/Event.cpp src/Menu.cpp src/Steam.cpp


clean :
	rm obj/*.o


