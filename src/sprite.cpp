#include "stdafx.h"
#include "graphics/surface.h"
#include "sprite.h"

cSprite::cSprite( ) {

	_X = _Y = _color = 0;

	_multiColor0 = 0x0A;
	_multiColor1 = 0x0D;

	_rEnabled = _rMultiColored = _rDoubleWidth = _rDoubleHeight = false;
	_surface = new cVideoSurface<dword>(40, 46);
	_buffer = 0;
}

cSprite::~cSprite() {
	
	delete _surface;
}

void cSprite::drawMulti( byte *pBuffer ) {
	byte	currentByte = *pBuffer++, count = 0;
	_surface->surfaceWipe(0xFF);
	
	byte maxY = 20;
	byte maxX = 23;

	if( _rDoubleHeight )
		maxY *= 2;
	if( _rDoubleWidth )
		maxX *= 2;

	for( word Y = 0; Y <= maxY; ++Y ) {
	
		for( word X = 0; X <= maxX; ++X ) {

			byte pixel = (currentByte & 0xC0) >> 6;
			// 3 Bytes per row
			
			if(pixel) {
				
				if(pixel == 1)
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
			if( _rDoubleWidth )
				++X;

			currentByte <<= 1;
			currentByte <<= 1;
			if(++count == 4) {
				currentByte = *pBuffer++;
				count = 0;
			}
		}

		if( _rDoubleHeight )
			++Y;
	}
}

void cSprite::drawSingle( byte *pBuffer ) {
	byte	currentByte = *pBuffer++, count = 0;
	byte	byteCount = 1;

	_surface->surfaceWipe(0xFF);

	byte maxY = 20;
	byte maxX = 23;

	if( _rDoubleHeight )
		maxY *= 2;
	if( _rDoubleWidth )
		maxX *= 2;

	for( word Y = 0; Y <= maxY; ++Y ) {
	
		for( word X = 0; X <= maxX; ++X ) {
			
			if(currentByte & 0x80) {
				_surface->pixelDraw(X, Y, _color);

				if( _rDoubleHeight )
					_surface->pixelDraw(X, Y+1, _color);	

			}

			currentByte <<= 1;
			if(++count == 8) {
				currentByte = *pBuffer++;
				count = 0;
				++byteCount;
			}
			if( _rDoubleWidth )
				++X;

		}

		if( _rDoubleHeight )
			++Y;
	}
}

void cSprite::streamLoad( byte *pBuffer ) {
	if( !pBuffer )
		pBuffer = _buffer;
	else
		_buffer = pBuffer;

	_surface->surfaceWipe(0xFF);


	if( _rMultiColored )
		drawMulti( pBuffer );
	else
		drawSingle( pBuffer );
}
