pathInc = -I/usr/include/directfb/direct -I/usr/include/directfb -I./src
Libs = `sdl-config --cflags --libs` -L/usr/local/lib -lresid
CC = g++ -c -Wall $(pathInc) $(Libs) 
LD = g++ -o creep -Wall $(Libs)

all : vic sid graphics castle main creep

vic: 

	$(CC) src/vic-ii/bitmapMulticolor.cpp src/vic-ii/screen.cpp src/vic-ii/sprite.cpp

sid:
	$(CC) src/sound/sound.cpp src/resid-0.16/*.cc 
	
creep : main
	mv *.o obj/
	$(LD) -o creep.o obj/*.o


graphics : 
	$(CC) src/graphics/screenSurface.cpp src/graphics/window.cpp
	$(CC) src/graphics/scale/scale2x.c src/graphics/scale/scale3x.c src/graphics/scale/scalebit.c	

castle :
	$(CC) src/castle/castle.cpp src/castle/image.cpp src/castle/room.cpp src/castle/object/*.cpp

main :
	$(CC) src/castleManager.cpp src/stdafx.cpp src/creep.cpp src/d64.cpp src/playerInput.cpp  
 
clean :
	rm obj/*.o

