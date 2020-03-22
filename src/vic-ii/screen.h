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
 *  Video Screen Header
 *  ------------------------------------------
 */

class cCreep;
class cWindow;
class cBitmapMulticolor;
class cSprite;
class cScreenSurface;
struct sScreenPiece;

class cScreen {
	byte					 *mBitmapBuffer, *mBitmapColorData, *mBitmapColorRam, mBitmapBackgroundColor;
	dword					  mFPS, mFPSTotal, mFPSSeconds;

	cWindow					*mWindow;
	cBitmapMulticolor		*mBitmap;
	cScreenSurface			*mSurface;

	SDL_Surface				*mSDLCursorSurface;

	vector< sScreenPiece* >  mCollisions;
	cSprite					*mSprites[8];

	bool					 mBitmapRedraw, mSpriteRedraw, mTextRedraw;
	size_t					 mScale, mDrawDestX, mDrawDestY, mDrawSrcX, mDrawSrcY;
	string					 mWindowTitle, mLevelName;
	size_t					 mRoomNumber;

	bool					 mCursorOn;
	word					 mCursorX, mCursorY;
	size_t					 mCursorWidth, mCursorHeight;

	void					 bitmapRefresh();
	void					 blit( cSprite *pSprite, int pSpriteNo );
	void					 blit( cScreenSurface *pSurface, size_t pDestX, size_t pDestY, bool pPriority, int pSpriteNo);
	
	void					 SDLSurfaceSet();

public:

							 cScreen( string pWindowTitle );
							~cScreen();
	
	void					 bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );

	void					 clear( byte pColor );

	void					 cursorEnabled( bool pOn = true );
	void					 cursorSize( const size_t pWidth, const size_t pHeight );
	void					 cursorSet( word pPosX, word pPosY );

	void					 drawStandardText(byte *pTextData, word pTextChar, byte *pColorData);

	void					 spriteDisable();
	void					 spriteDraw();
	cSprite					*spriteGet( byte pCount );
	
	void					 levelNameSet( string pName );
	inline string			 levelNameGet() { return mLevelName; }

	void					 refresh();

	void					 windowTitleUpdate();

	inline void				 roomNumberSet( size_t pValue ) { mRoomNumber = (pValue + 1); windowTitleUpdate(); }

	inline vector< sScreenPiece* > *collisionsGet() { return &mCollisions; }
	inline void						fullscreenToggle() {
															mWindow->SetFullScreen();
															refresh();
														}

	inline void				 bitmapRedrawSet( bool pVal = true ) { mBitmapRedraw = pVal; }
	inline void				 spriteRedrawSet() { mSpriteRedraw = true; }

	inline dword			 fpsGet()			{ return mFPS; }
	inline dword			 fpsAverageGet()	{ if(!mFPSSeconds || !mFPSTotal)
													return 0;
												else
													return (mFPSTotal / mFPSSeconds); }

	inline void				 windowTitleSet( string mTitle ) { mWindowTitle = mTitle; windowTitleUpdate(); }
};
