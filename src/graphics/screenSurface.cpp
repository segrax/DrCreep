/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2011 Robert Crossfield
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
#include "screenSurface.h"

cScreenSurface::cScreenSurface( size_t pWidth, size_t pHeight ) {
	mWidth = pWidth; 
	mHeight = pHeight;

	// Create the screen buffer
	mScreenSize = (mWidth * mHeight);
	mScreenPieces = new sScreenPiece[ mScreenSize ];
	mScreenBuffer = new dword[ mScreenSize ];

	palettePrepare();
	Wipe();
}

cScreenSurface::~cScreenSurface( ) {

	delete mScreenBuffer;
	delete mScreenPieces;
}

void cScreenSurface::palettePrepare() {
	const byte C64pal[16][3] = {
		0x00, 0x00, 0x00,
		0xFF, 0xFF, 0xFF, 
		0x68, 0x37, 0x2b, 
		0x70, 0xa4, 0xb2, 
		0x6f, 0x3d, 0x86, 
		0x58, 0x8d, 0x43, 
		0x35, 0x28, 0x79, 
		0xb8, 0xc7, 0x6f, 
		0x6f, 0x4f, 0x25,
		0x43, 0x39, 0x00,
		0x9a, 0x67, 0x59,
		0x44, 0x44, 0x44,
		0x6c, 0x6c, 0x6c,
		0x9a, 0xd2, 0x84,
		0x6c, 0x5e, 0xb5,
		0x95, 0x95, 0x95
	};
	SDL_Surface *surface = SDL_CreateRGBSurface(	SDL_SWSURFACE,	1,	1,	 32, 0, 0, 0, 0);

	for(int color=0; color < 16; color++)
		mPalette[ color ] = SDL_MapRGB (	surface->format , C64pal[color][0], C64pal[color][1], C64pal[color][2] ) ;

	SDL_FreeSurface( surface );
}

void cScreenSurface::Wipe( dword pColor ) {
	sScreenPiece *piece = &mScreenPieces[ 0 ];
	dword		 *buffer = screenBufferGet();

	for( size_t count = 0; count < mScreenSize; ++count, ++piece, ++buffer ) {
		*buffer = pColor;
		piece->mPriority = ePriority_Background;
		piece->mSprite = piece->mSprite2 = 0;
	}

}

void cScreenSurface::pixelDraw( size_t pX, size_t pY, dword pPaletteIndex, ePriority pPriority, size_t pCount ) {
	sScreenPiece *piece = screenPieceGet(pX,pY);
	dword	*buffer = screenBufferGet( pX, pY );

	for( size_t count = 0; count < pCount; ++count ) {
		if( pPaletteIndex > 15 )
			*buffer = 0;
		else
			*buffer = mPalette[ pPaletteIndex ];

		piece->mPriority = pPriority;

		++buffer;
		++piece;
	}
}
