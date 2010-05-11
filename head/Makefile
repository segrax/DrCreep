pathInc = -I/usr/include/SDL -I/usr/include/directfb/direct -I/usr/include/directfb -I./src
CC = g++ -c -Wall $(pathInc) 



vic: 

	$(CC) src/vic-ii/bitmapMulticolor.cpp src/vic-ii/screen.cpp src/vic-ii/sprite.cpp

sid:
	$(CC) src/sound/sound.cpp src/resid-0.16/*.cc
	
creep : main
	mv *.o obj/
	g++ -o creep *.o

graphics : 
	$(CC) src/graphics/screenSurface.cpp src/graphics/window.cpp
	

castle :
	$(CC) src/castle/castle.cpp src/castle/image.cpp src/castle/room.cpp src/castle/object/*.cpp

main :
	$(CC) src/castleManager.cpp src/creep.cpp src/d64.cpp src/playerInput.cpp src/stdafx.cpp 
all : vic sid graphics castle main creep
 
clean :
