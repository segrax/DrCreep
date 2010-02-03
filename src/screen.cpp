#include "stdafx.h"
#include "graphics/window.h"
#include "graphics/surface.h"
#include "sprite.h"
#include "bitmapMulticolor.h"
#include "screen.h"

enum eScreen_Priority {
	eScreen_Background = 8,
};

cScreen::cScreen() {
	
	for(char Y = 0; Y < 8; ++Y ) {
		mSprites[Y] = new cSprite();
		mSprites[Y]->_multiColor0 = 0x0A;
		mSprites[Y]->_multiColor1 = 0x0D;
	}

	mWindow = new cVideoWindow( 640, 400, 4 );
	mBitmap = new cBitmapMulticolor();
	mSurface = new cVideoSurface<dword>( 640, 400 );
	mSurfacePriority = new byte[640 * 400];
}

cScreen::~cScreen() {

	for(char Y = 0; Y < 8; ++Y ) 
		delete mSprites[Y];
}

void cScreen::clear(  byte pColor ) {
	
	mSurface->surfaceWipe( pColor );
}

void cScreen::bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	
	mBitmap->load( pBuffer, pColorData, pColorRam, pBackgroundColor0 );
	
	blit( mBitmap->mSurface, 0, 0, eScreen_Background );
}

void cScreen::blit( cSprite *pSprite, byte pOwner ) {
	
	blit( pSprite->_surface, pSprite->_X, pSprite->_Y, pOwner );
}

void cScreen::blit( cVideoSurface<dword> *pSurface, word pDestX, word pDestY, byte pOwner) {
	dword *pixel;
	dword *source = (dword*) pSurface->pointGet(0, 0);

	mSurface->_changed = true;

	for( word y = 0; y < pSurface->_height; ++y ) {
		pixel = mSurface->pointGet(pDestX, pDestY + y);

		for( word x = 0; x < pSurface->_width; ++x ) {
			if(*source != 0xFF)
				*pixel++ = *source++;
			else {
				pixel++;
				source++;
			}

		}

	}
	
}

cSprite *cScreen::spriteGet( byte pCount ) {
	if(pCount > 7)
		return 0;

	return mSprites[pCount];
}

void cScreen::refresh() {
	mSurface->surfaceSet();
	SDL_Surface *surface = mSurface->scaleTo(2);
	
	mWindow->clear(0);
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

		blit( sprite, Y );
	}
}