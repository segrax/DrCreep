#include "stdafx.h"
#include "graphics/surface.h"
#include "sprite.h"

cSprite::cSprite( word pColor0, word pColor1) {

	_X = _Y = _color = 0;

	_multiColor0 = pColor0;
	_multiColor1 = pColor1;

	_rEnabled = _rMultiColored = _rDoubleWidth = _rDoubleHeight = false;
	_surface = new cVideoSurface<dword>(25, 28);
}

cSprite::~cSprite() {
	
	delete _surface;
}

void cSprite::streamLoad( byte *pBuffer ) {
	byte	currentByte = *pBuffer++, count = 0;

	_surface->surfaceWipe(0xFF);

	for( word Y = 0; Y < 27; ++Y ) {
	
		for( word X = 0; X < 24; ++X ) {
		
			if(_rMultiColored) {
				byte pixel = (currentByte & 0xC0) >> 6;
				// 3 Bytes per row
				
				if(pixel) {
					if( pixel == 0 )
						pixel = 0xFF;

					else if(pixel == 1)
						pixel = _multiColor0;

					else if(pixel == 2)
						pixel = _color;

					else if(pixel == 3)
						pixel = _multiColor1;
					
					_surface->pixelDraw(X+1, Y, pixel);
					_surface->pixelDraw(X, Y, pixel);

					if( _rDoubleHeight ) {
						_surface->pixelDraw(X+1, Y+1, pixel);
						_surface->pixelDraw(X, Y+1, pixel);	
					}

				} 
				
				++X;
				currentByte <<= 1;
				currentByte <<= 1;
				if(++count == 4) {
					currentByte = *pBuffer++;
					count = 0;
				}

			} else {
				// 3 Bytes per row
				
				if(currentByte & 0x80) {
					_surface->pixelDraw(X, Y, _color);

					if( _rDoubleHeight )
						_surface->pixelDraw(X, Y+1, _color);	

				}

				currentByte <<= 1;
				if(++count == 8) {
					currentByte = *pBuffer++;
					count = 0;
				}
			}
		}

		if( _rDoubleHeight )
			++Y;
	}
}
