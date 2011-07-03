#pathInc = -I/usr/include/directfb/direct -I/usr/include/directfb -I./src
pathInc = -I./src
Libs = `sdl-config --cflags` -L/usr/local/lib
LibsS = `sdl-config --libs` `sdl-config --cflags` -L/usr/local/lib -L/usr/lib

CC = g++ -Wall $(pathInc) $(LibsS) 
LD = g++ -Wall $(LibsS)

VIC = src/vic-ii/bitmapMulticolor.cpp src/vic-ii/screen.cpp src/vic-ii/sprite.cpp
SID = src/sound/sound.cpp src/resid-0.16/*.cpp

GFX1 = src/graphics/screenSurface.cpp src/graphics/window.cpp
GFX2 = src/graphics/scale/scale2x.cpp src/graphics/scale/scale3x.cpp src/graphics/scale/scalebit.cpp

CASTLE = src/castle/castle.cpp src/castle/room.cpp src/castle/objects/*.cpp

MAIN = src/castleManager.cpp src/stdafx.cpp src/creep.cpp src/d64.cpp src/playerInput.cpp src/debug.cpp src/builder.cpp


all : main creep

main :

	$(CC) $(VIC) $(SID) $(GFX1) $(GFX2) $(CASTLE) $(MAIN) -o run/creep -lcompat

creep : main
        #mv *.o obj/
		
#        $(LD) -o run/creep obj/*.o

