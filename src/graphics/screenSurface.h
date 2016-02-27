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

enum ePriority {
	ePriority_Background = 0,
	ePriority_Foreground = 1,
	ePriority_None = 2,
};

struct sScreenPiece {
	ePriority	mPriority;
	byte		mSprite, mSprite2;

	sScreenPiece() {
		mPriority = ePriority_None;
		mSprite = mSprite2 = 0;
	}
};

class cScreenSurface {
protected:
	dword			 mPalette[16];

	sScreenPiece	*mScreenPieces;

	byte*			mSurfaceBuffer;								// Loaded Image (uses palette indexs)
	size_t			mSurfaceBufferSize;

	SDL_Surface*	mSDLSurface;
	SDL_Texture*	mTexture;

	int				mWidth, mHeight;

	void			 palettePrepare();
	void			 Wipe( byte pColor = 0 );						// Clear the surface

public:
	
					 cScreenSurface( int pWidth, int pHeight );
					~cScreenSurface();


	void			 draw( size_t pX = 0 , size_t pY = 0);					// Draw image to SDL Surface



	void			 WipeBuffer( byte pColor = 0 );

	dword			*pixelGet( word pX = 0,	word pY = 0 );
	void			 pixelDraw( size_t pX, size_t pY, dword pPaletteIndex, ePriority pPriority, size_t pCount = 1);

	inline SDL_Surface* GetSDLSurface() const { return mSDLSurface; }
	inline SDL_Texture* GetTexture() const { return mTexture; };
	inline byte*		GetSurfaceBuffer() const { return mSurfaceBuffer; }
	inline byte*		GetSurfaceBuffer( word pDestX, word pDestY ) const { return mSurfaceBuffer + (mWidth * pDestY) + pDestX; }
	
	inline byte *screenBufferGet() { return mSurfaceBuffer; }
	inline byte *screenBufferGet( size_t pX, size_t pY ) {
		return &mSurfaceBuffer[ ((pY * mWidth) + pX) ];
	}

	inline sScreenPiece *screenPiecesGet() { return mScreenPieces; }
	inline sScreenPiece *screenPieceGet( size_t pX, size_t pY ) {
	return &mScreenPieces[ ((pY * mWidth) + pX) ];
	}

	inline size_t		GetWidth() const { return mWidth;  }
	inline size_t		GetHeight() const { return mHeight; }
};
