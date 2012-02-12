/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2012 Robert Crossfield
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

#define MAX_WIIMOTES				4

cPlayerInput::cPlayerInput( cCreep *pCreep ) {
	mCreep = pCreep;

	mKeyPressed = 0;
	mKeyPressedRaw = 0;

	mRunStop = false;
	mRestore = false;
	mF2 = false;
	mF3 = false;
	mF4 = false;
	mF5 = false;

}

cPlayerInput::~cPlayerInput() {

}

void cPlayerInput::inputClear() {
	mInput[0].clear();
	mInput[1].clear();

	mKeyPressed = 0;
	mKeyPressedRaw = 0;

	mF2 = false;
	mF3 = false;
	mF4 = false;
	mF5 = false;
	mRunStop = false;
	mRestore = false;
}

void cPlayerInput::joystickSet( size_t pPlayer, int pJoystickNumber ) {

    if( pJoystickNumber == -1 ) {
        mInput[pPlayer].mJoystick = 0;
        return;
    }

    if( mInput[pPlayer].mJoystick != 0 ) {
        mInput[pPlayer].mJoystick = 0;
        return;
    }

    if( pJoystickNumber >= SDL_NumJoysticks() )
        return;

    SDL_JoystickEventState(SDL_ENABLE);
    mInput[pPlayer].mJoystick = SDL_JoystickOpen(pJoystickNumber);

}

void cPlayerInput::inputCheck( bool pClearAll ) {

	if(pClearAll)
		inputClear();

	while( SDL_PollEvent(&mEvent) ) {

		if( mEvent.type == SDL_QUIT )
			exit(0);

#ifndef _WII
		KeyboardCheck();
        if( mInput[ 0 ].mJoystick == 0 )
		    KeyboardInputSet1( &mInput[ 0 ] );
        else
            JoystickInputSet( &mInput[0] );

        if( mInput[ 1 ].mJoystick == 0 )
    		KeyboardInputSet2( &mInput[ 1 ] );
        else
            JoystickInputSet( &mInput[1] );
#endif
	}

#ifdef _WII
	wiiInputCheck();

	wiiInputSet( &mInput[ 0 ], WPAD_CHAN_0 );
	wiiInputSet( &mInput[ 1 ], WPAD_CHAN_1 );
#endif

	return;
}

#ifdef _WII

void cPlayerInput::wiiInputCheck() {

	WPAD_ScanPads();

	u32 held = WPAD_ButtonsHeld(0); 
	
	held +=  WPAD_ButtonsHeld(1);

	if( held & WPAD_BUTTON_HOME )
		exit(0);

	//case SDLK_F1:
	if( held & WPAD_BUTTON_PLUS )
		mRunStop = true;
	else
		mRunStop = false;

	//case SDLK_F2:
	if( held & WPAD_BUTTON_MINUS )
		mF2 = true;
	else
		mF2 = false;

	//case SDLK_F3:
	if( held & WPAD_BUTTON_1 )
		mF3 = true;
	else
		mF3 = false;

	//case SDLK_F4:
	if( held & WPAD_BUTTON_B )
		mF4 = true;
	else
		mF4 = false;

	//case SDLK_ESCAPE:
	if( held & WPAD_BUTTON_A )
		mRestore = true;
	else
		mRestore = false;

}

void cPlayerInput::wiiInputSet ( sPlayerInput *pInput, dword pChannel ) {
	u32 held = WPAD_ButtonsHeld(pChannel); 

	if( held & WPAD_BUTTON_2 )
		pInput->mButton = true;
	else
		pInput->mButton = false;

	if( held & WPAD_BUTTON_LEFT )
		pInput->mDown = true;
	else
		pInput->mDown = false;

	if( held & WPAD_BUTTON_RIGHT )
		pInput->mUp = true;
	else
		pInput->mUp = false;

	if( held & WPAD_BUTTON_UP )
		pInput->mLeft = true;
	else
		pInput->mLeft = false;

	if( held & WPAD_BUTTON_DOWN )
		pInput->mRight = true;
	else
		pInput->mRight = false;
}


#endif

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

			case SDLK_F4:
				mF4 = false;
				break;

			case SDLK_F5:
				mF5 = false;
				break;

            case SDLK_F6:
                joystickSet(0, 0);
                break;

            case SDLK_F7:
                joystickSet(1, 1);
                break;

			case SDLK_ESCAPE:
				mRestore = false;
				break;

			default:
				mKeyPressed = 0;
				mKeyPressedRaw = 0;
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
			
			case SDLK_F4:
				mF4 = true;
				break;

			case SDLK_F5:
				mF5 = true;
				break;

			case SDLK_ESCAPE:
				mRestore = true;
				break;

			case SDLK_F10:
				mCreep->screenGet()->fullscreenToggle();
				break;

			default:
				mKeyPressed = mEvent.key.keysym.scancode;
				mKeyPressedRaw = mEvent.key.keysym.sym;
		}
	}

}

void cPlayerInput::JoystickInputSet( sPlayerInput *pInput ) {
    Uint8 button = SDL_JoystickGetButton( pInput->mJoystick, 0 );
    Uint8 hatEvent = SDL_JoystickGetHat( pInput->mJoystick, 0 );
	Uint8 hatNums = SDL_JoystickNumHats( pInput->mJoystick );

	Sint16 axisX = SDL_JoystickGetAxis( pInput->mJoystick, 0 )  / 32767;
	Sint16 axisY = SDL_JoystickGetAxis( pInput->mJoystick, 1 )  / 32767;

    if( button == 1 )
        pInput->mButton = true;

    if( button == 0 )
        pInput->mButton = false;

    if ( axisX == 0 ) {
        pInput->mLeft = false;
        pInput->mRight = false;
    }

	if( axisY == 0 ) {
		pInput->mDown = false;
        pInput->mUp = false;
	}

    if ( axisY == -1 )
        pInput->mUp = true;
    else
        pInput->mUp = false;

    if ( axisX == -1 )
        pInput->mLeft = true;
    else
        pInput->mLeft = false;

    if ( axisX == 1 )
        pInput->mRight = true;
    else
        pInput->mRight = false;

    if ( axisY == 1 )
        pInput->mDown = true;       
    else
        pInput->mDown = false;

    
}

void cPlayerInput::KeyboardInputSet1( sPlayerInput *pInput ) {

    bool pressed = false;

    if( mEvent.type == SDL_KEYDOWN )
        pressed = true;

	switch( mEvent.key.keysym.sym ) {
		case SDLK_RCTRL:
                pInput->mButton = pressed;

			break;

		case SDLK_LEFT:
				pInput->mLeft = pressed;

			break;

		case SDLK_RIGHT:
				pInput->mRight = pressed;

			break;

		case SDLK_DOWN:
				pInput->mDown = pressed;
			break;

		case SDLK_UP:
				pInput->mUp = pressed;

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
