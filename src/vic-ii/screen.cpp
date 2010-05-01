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
 *  Video Screen
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "graphics/window.h"
#include "graphics/screenSurface.h"
#include "sprite.h"
#include "bitmapMulticolor.h"
#include "screen.h"
#include "creep.h"

const word gWidth = 360, gHeight = 265;

cScreen::cScreen( cCreep *pCreep, string pWindowTitle ) {

	mCreep = pCreep;

	for(byte Y = 0; Y < 8; ++Y ) {
		mSprites[Y] = new cSprite();
		mSprites[Y]->_multiColor0 = 0x0A;
		mSprites[Y]->_multiColor1 = 0x0D;
	}

	mFullScreen			= false;
	mScreenRedraw		= false;
	mSDLSurfaceScaled	= 0;
	mWindow				= 0;
	mWindowTitle		= pWindowTitle;

	mSurface	= new cScreenSurface( gWidth, gHeight );
	mBitmap		= new cBitmapMulticolor();

	// Create the SDL surfaces 
	mSDLSurface = SDL_CreateRGBSurface(	SDL_SWSURFACE,	gWidth,	gHeight,	 32, 0, 0, 0, 0);
}

cScreen::~cScreen() {

	for(byte Y = 0; Y < 8; ++Y ) 
		delete mSprites[Y];

	delete mSurface;
	delete mBitmap;
	delete mWindow;
	
	SDL_FreeSurface( mSDLSurface );
	SDL_FreeSurface( mSDLSurfaceScaled );
}

void cScreen::clear(  byte pColor = 0xFF ) {
	
	mCollisions.clear();
	mSurface->Wipe( pColor );
	mScreenRedraw = true;
}

void cScreen::scaleSet( byte pScale ) {
	word width, height;

	if(pScale > 4)
		return;

	SDL_FreeSurface( mSDLSurfaceScaled );
	delete mWindow;

	mScale = pScale;

	width = gWidth * mScale;
	height = gHeight * mScale;

	mWindow = new cVideoWindow( width, height, 4, mFullScreen );
	mSDLSurfaceScaled =	SDL_CreateRGBSurface(	SDL_SWSURFACE,	width, height,	 32, 0, 0, 0, 0);

	levelNameSet("");
}

void cScreen::bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	
	mBitmapBuffer = pBuffer;
	mBitmapColorData = pColorData;
	mBitmapColorRam = pColorRam;
	mBitmapBackgroundColor = pBackgroundColor0;

	screenRedrawSet();
}

void cScreen::bitmapRefresh() {
	clear();

	mBitmap->load( mBitmapBuffer, mBitmapColorData, mBitmapColorRam, mBitmapBackgroundColor );
	blit( mBitmap->mSurface, 24, 50, false, false );
}

void cScreen::blit( cSprite *pSprite, byte pSpriteNo ) {
	
	blit( pSprite->_surface, pSprite->_X, pSprite->_Y, pSprite->_rPriority, pSpriteNo );
}

void cScreen::blit( cScreenSurface *pSurface, size_t pDestX, size_t pDestY, bool pPriority, byte pSpriteNo) {
	bool		  col1 = false, col2 = false;
	sScreenPiece *dest;
	sScreenPiece *source = pSurface->screenPiecesGet();

	mScreenRedraw = true;
	for( word y = 0; y < pSurface->heightGet(); ++y, ++pDestY ) {
		dest = mSurface->screenPieceGet( pDestX, (byte) pDestY);

		for( word x = 0; x < pSurface->widthGet(); ++x ) {
			
			// Transparent?
			if( source->mPixel != 0xFF ) {

				// Check for any collisions
				if( dest->mPriority == ePriority_Foreground || dest->mSprite ) {
					if(dest->mSprite) {
						dest->mSprite2 = pSpriteNo; 
						if(!col1) {
							mCollisions.push_back( dest );
							col1 = true;
						}
					} else {
						dest->mSprite = pSpriteNo;
						if(!col2) {
							mCollisions.push_back( dest );
							col2 = true;
						}
					}
				}

				if( pSpriteNo && !dest->mSprite )
					dest->mSprite = pSpriteNo;
				else if( pSpriteNo != dest->mSprite )
					dest->mSprite2 = pSpriteNo;

				// Does this sprite have priority over the background?
				if( !pPriority || (dest->mPriority == ePriority_Background && pPriority) ) {
						dest->mPixel = source->mPixel;
						dest->mPriority = source->mPriority;
				}



			}

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
	stringstream windowTitle;

	windowTitle << mWindowTitle;
	windowTitle << ". '";
	windowTitle << pName;
	windowTitle << "' (SVN: " << SVNREV;
	windowTitle << "-" << SVNDATE << ")";

	mWindow->titleSet( windowTitle.str() );
}

void cScreen::refresh() {
	if( mScreenRedraw ) {
		bitmapRefresh();
		spriteDraw();

		mScreenRedraw = false;

		SDL_Surface *surface = scaleUp();
	
		mWindow->clear(0);
		mWindow->blit( surface, 0, 0, 15, 30 );
	}
}

void cScreen::spriteDisable() {
	for( char Y = 7; Y >= 0; --Y )
		mSprites[Y]->_rEnabled = false;
}

void cScreen::spriteDraw() {
	cSprite *sprite;

	// Draw from sprite 7
	for( char Y = 7; Y >= 0; --Y ) {
		sprite = mSprites[Y];

		if(!sprite->_rEnabled)
			continue;

		sprite->streamLoad( 0 );

		blit( sprite, Y + 1 );
	}
}

void cScreen::SDLSurfaceSet(){
	dword			*pixel  = (dword*) mSDLSurface->pixels;
	sScreenPiece	*source = mSurface->screenPiecesGet();

	for( unsigned int y = 0; y < mSurface->heightGet(); y++ ) {
		
		for( unsigned int x = 0; x < mSurface->widthGet(); x++ ) {

			*pixel++ = source->mPixel;
			++source;

		}
	}

}

SDL_Surface	*cScreen::scaleUp( ) {
	SDLSurfaceSet();

	if( mScale < 2 || mScale > 4 )
		return mSDLSurface;

	SDL_SetColorKey(mSDLSurfaceScaled, SDL_SRCCOLORKEY, SDL_MapRGB(mSDLSurfaceScaled->format, 0, 0, 0)	);

	// Do the scale
	scale(mScale, mSDLSurfaceScaled->pixels, mSDLSurfaceScaled->pitch, mSDLSurface->pixels, mSDLSurface->pitch, mSDLSurface->format->BytesPerPixel, mSDLSurface->w, mSDLSurface->h);

	return mSDLSurfaceScaled;
}
