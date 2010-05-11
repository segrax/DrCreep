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
 *  Player Input
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "playerInput.h"
#include "creep.h"
#include "vic-ii/screen.h"

cPlayerInput::cPlayerInput( cCreep *pCreep ) {
	mCreep = pCreep;

	mKeyPressed = 0;
	mRunStop = false;
	mRestore = false;
	mF2 = false;
	mF3 = false;
}

void cPlayerInput::inputCheck( bool pClearAll ) {

	if(pClearAll) {
		mInput[0].clear();
		mInput[1].clear();

		mF2 = false;
		mF3 = false;
		mRunStop = false;
		mRestore = false;
	}

	while( SDL_PollEvent(&mEvent) ) {

		if( mEvent.type == SDL_QUIT )
			exit(0);

		KeyboardCheck();
		KeyboardInputSet1( &mInput[ 0 ] );
		KeyboardInputSet2( &mInput[ 1 ] );
	}

	return;
}

void cPlayerInput::KeyboardCheck() {

	// Key Released
	if( mEvent.type == SDL_KEYUP ) {

		switch( mEvent.key.keysym.sym ) {
			case SDLK_F1:
				mRunStop = false;
				break;

			case SDLK_F2:
				mF2 = false;
				break;

			case SDLK_F3:
				mF3 = false;
				break;

			case SDLK_ESCAPE:
				mRestore = false;
				break;

			default:
				mKeyPressed = 0;
				break;
		}

		return;
	}

	// Key Pressed
	if( mEvent.type == SDL_KEYDOWN ) {

		switch( mEvent.key.keysym.sym ) {
			case SDLK_F1:
				mRunStop = true;
				break;

			case SDLK_F2:
				mF2 = true;
				break;

			case SDLK_F3:
				mF3 = true;
				break;

			case SDLK_ESCAPE:
				mRestore = true;
				break;

			case SDLK_F10:
				mCreep->screenGet()->fullscreenToggle();
				break;

			default:
				mKeyPressed = mEvent.key.keysym.sym;
		}
	}

}

void cPlayerInput::KeyboardInputSet1( sPlayerInput *pInput ) {

	switch( mEvent.key.keysym.sym ) {
		case SDLK_RCTRL:

			if( mEvent.type == SDL_KEYDOWN )
				pInput->mButton = true;
			else
				pInput->mButton = false;

			break;

		case SDLK_LEFT:

			if( mEvent.type == SDL_KEYDOWN )
				pInput->mLeft = true;
			else
				pInput->mLeft = false;

			break;

		case SDLK_RIGHT:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mRight = true;
			else
				pInput->mRight = false;

			break;

		case SDLK_DOWN:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mDown = true;
			else
				pInput->mDown = false;
			break;

		case SDLK_UP:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mUp = true;
			else
				pInput->mUp = false;

			break;

		default:
			break;
	}
}

void cPlayerInput::KeyboardInputSet2( sPlayerInput *pInput ) {
	switch( mEvent.key.keysym.sym ) {
		case SDLK_KP0:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mButton = true;
			else
				pInput->mButton = false;

			break;

		case SDLK_KP4:

			if( mEvent.type == SDL_KEYDOWN )
				pInput->mLeft = true;
			else
				pInput->mLeft = false;

			break;

		case SDLK_KP6:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mRight = true;
			else
				pInput->mRight = false;

			break;

		case SDLK_KP2:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mDown = true;
			else
				pInput->mDown = false;
			break;

		case SDLK_KP8:
			if( mEvent.type == SDL_KEYDOWN )
				pInput->mUp = true;
			else
				pInput->mUp = false;

			break;

		default:
			break;
	}
}