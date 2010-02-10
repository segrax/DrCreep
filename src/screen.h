/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2010 Robert Crossfield
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
 *  Video Screen Header
 *  ------------------------------------------
 */

class cVideoWindow;
class cBitmapMulticolor;
class cSprite;
class cScreenSurface;
struct sScreenPiece;

class cScreen {
	byte					 *mBitmapBuffer, *mBitmapColorData, *mBitmapColorRam, mBitmapBackgroundColor;

	cVideoWindow			*mWindow;
	cBitmapMulticolor		*mBitmap;
	cScreenSurface			*mSurface;
	SDL_Surface				*mSDLSurface;
	SDL_Surface				*mSDLSurfaceScaled;
	
	vector< sScreenPiece* >  mCollisions;
	cSprite					*mSprites[8];

	size_t					 mScale;
	string					 mWindowTitle;

	void					 bitmapRefresh();
	void					 blit( cSprite *pSprite, byte pSpriteNo );
	void					 blit( cScreenSurface *pSurface, size_t pDestX, size_t pDestY, bool pPriority, byte pSpriteNo);
	SDL_Surface				*scaleUp();
	void					 SDLSurfaceSet();

public:

							 cScreen( string pWindowTitle );
							~cScreen();
	
	void					 bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );

	void					 clear( byte pColor );

	void					 scaleSet( byte pScale );

	void					 spriteDisable();
	void					 spriteDraw();
	cSprite					*spriteGet( byte pCount );

	void					 levelNameSet( string pName );
	void					 refresh();

	inline vector< sScreenPiece* > *collisionsGet() { return &mCollisions; }
};
