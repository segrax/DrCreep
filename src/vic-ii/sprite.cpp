/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2014 Robert Crossfield
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  ------------------------------------------
 *  Commodore 64 VIC-II Sprite
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "graphics/screenSurface.h"
#include "sprite.h"

cSprite::cSprite( ) {
	_maxY = 21;
	_maxX = 24;

	mX = mY = _color = 0;

	_multiColor0 = 0x0A;
	_multiColor1 = 0x0D;

	_rEnabled = _rMultiColored = _rDoubleWidth = _rDoubleHeight = false;
	_surface = 0;
	_buffer = 0;
}

cSprite::~cSprite() {
	
	delete _surface;
}

void cSprite::drawMulti( byte *pBuffer ) {
	byte		currentByte = *pBuffer++, count = 0;
	byte		color;
	ePriority	priority;
	byte		pixel;

	// Draw the sprite
	for( word Y = 0; Y < currentHeight; ++Y ) {
	
		for( word X = 0; X < currentWidth; ++X ) {

			pixel = (currentByte & 0xC0) >> 6;
			// 3 Bytes per row
			
			if(pixel == 0 || pixel == 1 )
				priority = ePriority_Background;
			else
				priority = ePriority_Foreground;

			if(pixel) {
				
				switch(pixel) {
					case 0x01:
						color = _multiColor0;
						break;

					case 0x02:
						color = _color;
						break;

					case 0x03:
						color = _multiColor1;
						break;
				}

				_surface->pixelDraw(X, Y, color, priority, 2);

				if( _rDoubleHeight )
					_surface->pixelDraw(X, Y+1, color, priority, 2);
			} 
			
			++X;
			if( _rDoubleWidth )
				++X;

			currentByte <<= 2;
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
	byte		currentByte = *pBuffer++, count = 0;
	byte		byteCount = 1;

	for( word Y = 0; Y < currentHeight; ++Y ) {
	
		for( word X = 0; X < currentWidth; ++X ) {

			if(currentByte & 0x80) {
				_surface->pixelDraw(X, Y, _color, ePriority_Foreground);

				if( _rDoubleHeight )
					_surface->pixelDraw(X, Y+1, _color, ePriority_Foreground);	

			} else {
				_surface->pixelDraw(X, Y, 0xFF, ePriority_Background);
				if( _rDoubleHeight )
					_surface->pixelDraw(X, Y+1, 0xFF, ePriority_Background);
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

	// No stream provided, then use the previous one
	if( !pBuffer )
		pBuffer = _buffer;

	else {
		// Otherwise set the buffer to the new stream
		_buffer = pBuffer;
		return;
	}

	currentWidth = _maxX;
	currentHeight = _maxY;

	if( _rDoubleHeight )
		currentHeight *= 2;
	if( _rDoubleWidth )
		currentWidth *= 2;

	delete _surface;
	_surface = new cScreenSurface(currentWidth, currentHeight);
	_surface->Wipe(0);

	if( _rMultiColored )
		drawMulti( pBuffer );
	else
		drawSingle( pBuffer );
}
