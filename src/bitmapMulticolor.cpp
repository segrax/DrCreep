#include "stdafx.h"
#include "graphics/surface.h"
#include "bitmapMulticolor.h"


cBitmapMulticolor::cBitmapMulticolor() {
	_surface = new cVideoSurface<dword>( 160, 200 );
}

cBitmapMulticolor::~cBitmapMulticolor() {
	delete _surface;
}

void cBitmapMulticolor::load( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	dword	color = 0;
	byte	data;
	byte	pixel;

	_surface->surfaceWipe(0xFF);

	// Draw 8 Rows
	for( size_t Y = 0; Y < 8; ++Y ) {
		// Read char row
		data = *pBuffer++;
		
		// Lets draw 8 bits
		for( size_t X = 0; X < 8; ++X ) {
			pixel = (data & 0xC0) >> 6;

			if(pixel == 0)
				color = pBackgroundColor0;

			else if(pixel == 1)
				color = *pColorData & 0xF;

			else if(pixel == 2)
				color = *pColorData & 0xF0;

			else if(pixel == 3)
				color = *pColorRam & 0xF;

			_surface->pixelDraw(X++, Y, color);
			_surface->pixelDraw(X, Y, color);
			data <<= 2;
		} // X

		++pColorData;
		++pColorRam;
	}	// Y

}
