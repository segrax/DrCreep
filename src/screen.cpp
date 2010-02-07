#include "stdafx.h"
#include "graphics/window.h"
#include "graphics/screenSurface.h"
#include "sprite.h"
#include "bitmapMulticolor.h"
#include "screen.h"

cScreen::cScreen() {
	
	for(byte Y = 0; Y < 8; ++Y ) {
		mSprites[Y] = new cSprite();
		mSprites[Y]->_multiColor0 = 0x0A;
		mSprites[Y]->_multiColor1 = 0x0D;
	}

	mWindow = new cVideoWindow( 640, 400, 4 );
	mBitmap = new cBitmapMulticolor();
	mSurface = new cScreenSurface( 640, 400 );

	// Default scale
	mScale = 0x02;
	
	// Create the SDL surfaces 
	mSDLSurface = SDL_CreateRGBSurface(	SDL_SWSURFACE,	640,	400,	 32, 0, 0, 0, 0);
	mSDLSurfaceScaled =	SDL_CreateRGBSurface(	SDL_SWSURFACE,	640 * mScale, 400 * mScale,	 32, 0, 0, 0, 0);

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
}

void cScreen::bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	clear();

	mBitmap->load( pBuffer, pColorData, pColorRam, pBackgroundColor0 );
	blit( mBitmap->mSurface, 0, 0, false, false );
}

void cScreen::blit( cSprite *pSprite, byte pSpriteNo ) {
	
	blit( pSprite->_surface, pSprite->_X, pSprite->_Y, pSprite->_rPriority, pSpriteNo );
}



void cScreen::blit( cScreenSurface *pSurface, size_t pDestX, size_t pDestY, bool pPriority, byte pSpriteNo) {
	bool		  col1 = false, col2 = false;
	sScreenPiece *dest;
	sScreenPiece *source = pSurface->screenPiecesGet();

	for( word y = 0; y < pSurface->heightGet(); ++y, ++pDestY ) {
		dest = mSurface->screenPieceGet(pDestX, pDestY);

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

void cScreen::refresh() {
	SDL_Surface *surface = scaleTo(2);
	
//	mWindow->clear(0);
	mWindow->blit( surface, 0, 0 );
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

SDL_Surface	*cScreen::scaleTo( size_t pScale ) {
	if( mScale != pScale ) {
		mScale = pScale;

		delete mSDLSurfaceScaled;
		mSDLSurfaceScaled =	SDL_CreateRGBSurface(	SDL_SWSURFACE,	mSurface->widthGet() * mScale,	mSurface->heightGet() * mScale,	 32, 0, 0, 0, 0);
	}

	SDLSurfaceSet();

	if( mScale < 2 || mScale > 4 )
		return mSDLSurface;

	SDL_SetColorKey(mSDLSurfaceScaled, SDL_SRCCOLORKEY, SDL_MapRGB(mSDLSurfaceScaled->format, 0, 0, 0)	);

	// Do the scale
	scale(mScale, mSDLSurfaceScaled->pixels, mSDLSurfaceScaled->pitch, mSDLSurface->pixels, mSDLSurface->pitch, mSDLSurface->format->BytesPerPixel, mSDLSurface->w, mSDLSurface->h);

	return mSDLSurfaceScaled;
}
