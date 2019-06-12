/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2016 Robert Crossfield
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
 *  SDL Surface Wrapper, using Commodore 64 Palette
 *  ------------------------------------------
 */

#include "../stdafx.h"

const int g_MaxColors = 16;

void cScreenSurface::palettePrepare() {
	const byte C64pal[g_MaxColors][3] = {
		{ 0x00, 0x00, 0x00 },
		{ 0xFF, 0xFF, 0xFF },
		{ 0x68, 0x37, 0x2b }, 
		{ 0x70, 0xa4, 0xb2 }, 
		{ 0x6f, 0x3d, 0x86 }, 
		{ 0x58, 0x8d, 0x43 }, 
		{ 0x35, 0x28, 0x79 }, 
		{ 0xb8, 0xc7, 0x6f }, 
		{ 0x6f, 0x4f, 0x25 },
		{ 0x43, 0x39, 0x00 },
		{ 0x9a, 0x67, 0x59 },
		{ 0x44, 0x44, 0x44 },
		{ 0x6c, 0x6c, 0x6c },
		{ 0x9a, 0xd2, 0x84 },
		{ 0x6c, 0x5e, 0xb5 },
		{ 0x95, 0x95, 0x95 }
	};

	for(int color=0; color < 16; color++)
		mPalette[ color ] = SDL_MapRGB (	mSDLSurface->format , C64pal[color][0], C64pal[color][1], C64pal[color][2] ) ;
}

cScreenSurface::cScreenSurface( int pWidth, int pHeight ) {
	mWidth = pWidth; 
	mHeight = pHeight;

	// Create the screen buffer
	mSDLSurface = SDL_CreateRGBSurface(0, (int)pWidth, (int)pHeight, 32, 0xFF << 16, 0xFF << 8, 0xFF, 0);
	mTexture = SDL_CreateTexture(g_Window.GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, pWidth, pHeight);
	
	mSurfaceBuffer = new byte[ mWidth * mHeight ];
	mSurfaceBufferSize = mWidth * mHeight;
	mScreenPieces = new sScreenPiece[ mSurfaceBufferSize ];
	for (size_t i = 0; i < mSurfaceBufferSize; ++i) {
		mSurfaceBuffer[i] = 0;
	}

	palettePrepare();
	Wipe();
}

cScreenSurface::cScreenSurface(const std::string& pFile) {

	mSDLSurface = SDL_LoadBMP(pFile.c_str());
	mTexture = SDL_CreateTextureFromSurface(g_Window.GetRenderer(), mSDLSurface);

	mWidth = mSDLSurface->w;
	mHeight = mSDLSurface->h;

	mSurfaceBuffer = 0;
	mSurfaceBufferSize = 0;
	mScreenPieces = 0;
}

cScreenSurface::~cScreenSurface() {
	delete[] mSurfaceBuffer;
	delete[] mScreenPieces;

	SDL_FreeSurface( mSDLSurface );
	SDL_DestroyTexture( mTexture );
}

void cScreenSurface::Wipe( byte pColor ) {

	SDL_FillRect( mSDLSurface, 0, pColor );
}

void cScreenSurface::WipeBuffer( byte pColor ) {
	sScreenPiece *piece = &mScreenPieces[ 0 ];
	memset( mSurfaceBuffer, 0, mSurfaceBufferSize );
	
	for( size_t count = 0; count < mSurfaceBufferSize; ++count, ++piece ) {
		piece->mPriority = ePriority_None;
		piece->mSprite = piece->mSprite2 = 0;
	}
}

dword *cScreenSurface::pixelGet( const size_t x, const size_t y ) {
	register dword *position = (dword*) mSDLSurface->pixels ;
		
	// Offset by Y
	position +=  (y * (mSDLSurface->pitch / 4) );		// 4 Bytes per Pixel

	// Offset by X
	position +=  x;

	return position;
}

void cScreenSurface::pixelDraw( size_t pX, size_t pY, dword pPaletteIndex, ePriority pPriority, size_t pCount ) {
	sScreenPiece *piece = screenPieceGet(pX,pY);
	byte	*buffer = screenBufferGet( pX, pY );

	for( size_t count = 0; count < pCount; ++count ) {

		*buffer = pPaletteIndex;

		piece->mPriority = pPriority;

		++buffer;
		++piece;
	}
}

void cScreenSurface::draw( size_t pX, size_t pY ) {

	Wipe();

	byte *bufferCurrent = mSurfaceBuffer;
	byte *bufferCurrentMax = (mSurfaceBuffer + mSurfaceBufferSize);

	dword *bufferTarget = (dword*) pixelGet( pX, pY );
	dword *bufferTargetMax = (dword*) (((byte*) mSDLSurface->pixels) + (mSDLSurface->h * mSDLSurface->pitch));

	while( bufferTarget < bufferTargetMax ) {
			
		if( bufferCurrent >= bufferCurrentMax )
			break;

		if (*bufferCurrent && *bufferCurrent != 0xFF) {
			if (*bufferCurrent < g_MaxColors)
				*bufferTarget = mPalette[*bufferCurrent];
			else
				*bufferTarget = 0;
		}

		++bufferTarget; 
		++bufferCurrent;
	}

	SDL_UpdateTexture(mTexture, NULL, mSDLSurface->pixels, mSDLSurface->pitch);
}
