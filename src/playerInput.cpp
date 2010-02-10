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

cPlayerInput::cPlayerInput() {

	mRunStop = false;
	mRestore = false;
}

void cPlayerInput::inputCheck( bool pClearAll ) {
	
	mRunStop = false;
	mRestore = false;

	if(pClearAll) {
		mInput[0].clear();
		mInput[1].clear();
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

	if( mEvent.type == SDL_KEYDOWN ) {

		if( mEvent.key.keysym.sym == SDLK_F1 )
			mRunStop = true;

		if( mEvent.key.keysym.sym == SDLK_ESCAPE )
			mRestore = true;
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
	}
}
