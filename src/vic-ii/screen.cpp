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
 *  Video Screen
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "sprite.h"
#include "bitmapMulticolor.h"
#include "screen.h"
#include "creep.h"
#include <time.h>

const word gWidth = 366, gHeight = 272;

cScreen::cScreen( string pWindowTitle ) {

	for(byte Y = 0; Y < 8; ++Y ) {
		mSprites[Y] = new cSprite();
		mSprites[Y]->_multiColor0 = 0x0A;
		mSprites[Y]->_multiColor1 = 0x0D;
	}

	mBitmapBuffer = 0;
	mBitmapRedraw		= false;
	mSpriteRedraw		= false;
	mTextRedraw			= false;
	mFPSTotal			= 0;
	mFPSSeconds			= 0;
	mFPS				= 0;
	mWindow				= 0;
	mWindowTitle		= pWindowTitle;

	mRoomNumber			= 0;

	mCursorOn						= false;
	mCursorWidth = mCursorHeight	= 1;
	mCursorX = mCursorY				= 0;

	mDrawDestX = mDrawDestY = mDrawSrcX = mDrawSrcY = 0;
	
	mWindow = new cWindow();
	mWindow->InitWindow( pWindowTitle );

	mSurface	= new cScreenSurface( gWidth, gHeight );
	mBitmap		= new cBitmapMulticolor();

	// Create the SDL surfaces 
	mSDLCursorSurface = 0;

	windowTitleUpdate();
}

cScreen::~cScreen() {

	for(byte Y = 0; Y < 8; ++Y ) 
		delete mSprites[Y];

	delete mSurface;
	delete mBitmap;
	delete mWindow;
	
	SDL_FreeSurface( mSDLCursorSurface );
}

void cScreen::clear(  byte pColor = 0 ) {
	
	mSurface->WipeBuffer( pColor );
	mBitmapRedraw = true;
	mSpriteRedraw = true;
}

void cScreen::cursorEnabled( bool pOn ) {
	size_t width = (8 * mScale) * mCursorWidth;
	size_t height = (8 * mScale) * mCursorHeight;
	
	mCursorOn = pOn;

	SDL_FreeSurface( mSDLCursorSurface );
	mSDLCursorSurface = SDL_CreateRGBSurface(	SDL_SWSURFACE,	width,	height,	 32, 0, 0, 0,255 );

	// Draw the red for the border
	SDL_Rect	rect;
	rect.x = rect.y = 0;
	rect.w = (int) width;
	rect.h = (int) height;
	SDL_FillRect (mSDLCursorSurface, &rect, SDL_MapRGB(mSDLCursorSurface->format, 255, 0, 0)); 

	// Make the middle transparent
	rect.x = rect.y = 2;
	rect.w -= 2;
	rect.h -= 2;
	SDL_FillRect (mSDLCursorSurface, &rect, SDL_MapRGB(mSDLCursorSurface->format, 0, 0, 0)); 
}

void cScreen::cursorSize( const size_t pWidth, const size_t pHeight ) {
	mCursorWidth = pWidth;
	mCursorHeight = pHeight;

	// Refresh the cursor surface
	cursorEnabled( mCursorOn );
}

void cScreen::cursorSet( word pPosX, word pPosY ) {
	mCursorX = pPosX;
	mCursorY = pPosY;
}

void cScreen::bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	
	mBitmapBuffer = pBuffer;
	mBitmapColorData = pColorData;
	mBitmapColorRam = pColorRam;
	mBitmapBackgroundColor = pBackgroundColor0;

	bitmapRedrawSet();
}

void cScreen::bitmapRefresh() {
	mSurface->WipeBuffer();

	if (!mBitmapBuffer)
		return;

	if(mBitmapRedraw) {
		clear();
		mBitmap->load( mBitmapBuffer, mBitmapColorData, mBitmapColorRam, mBitmapBackgroundColor );
		mBitmapRedraw = false;
	}
	blit( mBitmap->mSurface, 24, 50, false, -1 );
}

void cScreen::blit( cSprite *pSprite, int pSpriteNo ) {
	
	blit( pSprite->_surface, pSprite->mX, pSprite->mY, pSprite->_rPriority, pSpriteNo );
}

void cScreen::blit( cScreenSurface *pSurface, size_t pDestX, size_t pDestY, bool pPriority, int pSpriteNo) {
	bool			 col1 = false, col2 = false;

	sScreenPiece	*dest = 0;
	byte			*destBuffer = 0;
	sScreenPiece	*source = pSurface->screenPiecesGet();
	byte			*sourceBuffer = pSurface->GetSurfaceBuffer();

	size_t			 height = pSurface->GetHeight();
	size_t			 width = pSurface->GetWidth();

	// Loop height
	for( word y = 0; y < height; ++y, ++pDestY ) {

		dest = mSurface->screenPieceGet( pDestX, pDestY);
		destBuffer = mSurface->GetSurfaceBuffer( pDestX, pDestY );

		// Loop width
		for( word x = 0; x < width; ++x ) {
			
			if( dest->mPriority == ePriority_None )
				dest->mPriority = source->mPriority;

			if (*sourceBuffer != 0 && *sourceBuffer != 0xFF) {
				// Check for any collisions
				if( source->mPriority != ePriority_None ) {

					if (dest->mPriority == ePriority_Foreground || dest->mSprite >= 0) {
 
						if (dest->mSprite >= 0 && dest->mSprite2 != pSpriteNo) {
							dest->mSprite2 = pSpriteNo;
							if (!col1) {
								mCollisions.push_back(dest);
								col1 = true;
							}
						}
						else {
							dest->mSprite = pSpriteNo;
							if (!col2 && pSpriteNo != -1) {
								mCollisions.push_back(dest);
								col2 = true;
							}
						}
					}

					if (pSpriteNo >= 0 && dest->mSprite == -1)
						dest->mSprite = pSpriteNo;
					else if (pSpriteNo != dest->mSprite && dest->mSprite2 == -1)
						dest->mSprite2 = pSpriteNo;
				}


				// Does this sprite have priority over the background?
				if( !pPriority || ((dest->mPriority == ePriority_Background) && pPriority) ) {
					*destBuffer = *sourceBuffer;

					dest->mPriority = source->mPriority;
				}
			}

			++sourceBuffer;
			++destBuffer;
			++dest;
			++source;
		}
	}
}

cSprite *cScreen::spriteGet( byte pCount ) {
	if(pCount > 7)
		return 0;

	return mSprites[pCount];
}

void cScreen::levelNameSet( string pName ) {
	mLevelName = pName;
	windowTitleUpdate();
}

void cScreen::windowTitleUpdate() {
	stringstream windowTitle;

	windowTitle << mWindowTitle;


	// No level yet?
	if( mLevelName.size() ) {

		windowTitle << ": " << mLevelName;
	}

	mWindow->SetWindowTitle( windowTitle.str() );
}

void cScreen::refresh() {

	if(( mBitmapRedraw || mSpriteRedraw) && !mTextRedraw) {
		bitmapRefresh();
		spriteDraw();
	}

	if( mTextRedraw ) {
		mTextRedraw = false;
	}

	mWindow->RenderAt(mSurface, cPosition(8, 15));

	if(mCursorOn) {
		size_t x =  ((mCursorX) * 2 ) * mScale;
		size_t y =  70 + (mCursorY) * mScale;
			
		//TODO
		//mWindow->RenderAt( mSDLCursorSurface, cPosition(x,y) );
	}
	
	mWindow->FrameEnd();
}

void cScreen::spriteDisable() {

	for( signed char Y = 7; Y >= 0; --Y )
		mSprites[Y]->_rEnabled = false;
}

void cScreen::spriteDraw() {
	cSprite *sprite;
	mCollisions.clear();

	// Draw from sprite 7
	for( signed char Y = 7; Y >= 0; --Y ) {
		sprite = mSprites[Y];

		if(!sprite->_rEnabled)
			continue;

		if( mSpriteRedraw )
			sprite->streamLoad( 0 );

		blit( sprite, Y + 1 );
	}

	mSpriteRedraw = false;
}

void cScreen::drawStandardText(byte *pTextData, word pTextChar, byte *pColorData) {
	dword	data;
	
	mTextRedraw = true;
	
	mSurface->WipeBuffer();

	for( unsigned int y = 0; y < 200; y += 8 ) {

		for( unsigned int X = 0; X < 320; X += 8 ) {

			// Read char pointer from screen text pointer
			data = *pTextData << 3;
			++pTextData;

			// Get memory address in char rom
			word _vidChar = pTextChar + data;

			// Lets draw a character
			for( size_t charY = 0 ; charY < 8; ++charY ) {

				// Read char row
				data = g_Creep.charRom( 0xD000 + (_vidChar + charY & 0x0FFF) );

				// Lets draw 8 bits
				for( size_t bit = 0, charX = X; bit < 8; bit++, charX++ ) {
					if( data & 0x80 )
						mSurface->pixelDraw( charX + 24, (y + charY) + 50, *pColorData, ePriority_Background );

					data <<= 1;
				}
				
			}

			++pColorData;
		}
	}

}
