#include "../stdafx.h"
#include "window.h"

cVideoWindow::cVideoWindow( size_t pWidth, size_t pHeight, size_t pBytesPixel) {
	
	_width = pWidth;
	_height = pHeight;
	_pixelBytes = pBytesPixel;

	SDL_Init ( SDL_INIT_VIDEO ); 
	atexit ( SDL_Quit ) ;

	_surface = SDL_SetVideoMode ( pWidth , pHeight , pBytesPixel * 8 , SDL_ANYFORMAT ) ;
	
	if(!_surface)
		return;
}

cVideoWindow::~cVideoWindow() {

	SDL_FreeSurface( _surface );

}

