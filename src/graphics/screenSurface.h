/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2013 Robert Crossfield
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

enum ePriority {
	ePriority_Background = 0,
	ePriority_Foreground = 1
};

struct sScreenPiece {
	ePriority	mPriority;
	//dword		mPixel;
	byte		mSprite, mSprite2;

	sScreenPiece() {
		mPriority = ePriority_Background;
//		mPixel = 0xFF;
		mSprite = mSprite2 = 0;
	}
};

class cScreenSurface {
private:
	dword			*mScreenBuffer;
	sScreenPiece	*mScreenPieces;				// Screen Info

	size_t			 mScreenSize;

	size_t			 mWidth, mHeight;

	dword			 mPalette[16];
	void			 palettePrepare();

public:

					 cScreenSurface( size_t pWidth, size_t pHeight );
					~cScreenSurface( );

	void			 pixelDraw( size_t pX, size_t pY, dword pPaletteIndex, ePriority pPriority, size_t pCount = 1 );
	void			 Wipe( dword pColor = 0 ) ;


	inline size_t		 heightGet() { return mHeight; }
	inline size_t		 widthGet()  { return mWidth; }

	inline dword *screenBufferGet() { return mScreenBuffer; }
	inline dword *screenBufferGet( size_t pX, size_t pY ) {
		return &mScreenBuffer[ ((pY * mWidth) + pX) ];
	}

	inline sScreenPiece *screenPiecesGet() { return mScreenPieces; }
	inline sScreenPiece *screenPieceGet( size_t pX, size_t pY ) {
		return &mScreenPieces[ ((pY * mWidth) + pX) ];
	}
};
