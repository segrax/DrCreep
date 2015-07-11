/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2015 Robert Crossfield
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


cPlayerInput::cPlayerInput() {

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

void cPlayerInput::inputCheck( bool pClearAll, cEvent pEvent ) {

	if(pClearAll)
		inputClear();

	mEvent = pEvent;

		KeyboardCheck();
        if( mInput[ 0 ].mJoystick == 0 )
		    KeyboardInputSet1( &mInput[ 0 ] );
        else
            JoystickInputSet( &mInput[0] );

        if( mInput[ 1 ].mJoystick == 0 )
    		KeyboardInputSet2( &mInput[ 1 ] );
        else
            JoystickInputSet( &mInput[1] );

	return;
}

void cPlayerInput::KeyboardCheck() {

	// Key Released
	if( mEvent.mType == eEvent_KeyUp ) {

		switch( mEvent.mButton ) {
			case SDL_SCANCODE_F1:
				mRunStop = false;
				break;

			case SDL_SCANCODE_F2:
				mF2 = false;
				break;

			case SDL_SCANCODE_F3:
				mF3 = false;
				break;

			case SDL_SCANCODE_F4:
				mF4 = false;
				break;

			case SDL_SCANCODE_F5:
				mF5 = false;
				break;

            case SDL_SCANCODE_F6:
                joystickSet(0, 0);
                break;

            case SDL_SCANCODE_F7:
                joystickSet(1, 1);
                break;

			case SDL_SCANCODE_ESCAPE:
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
	if( mEvent.mType == eEvent_KeyDown ) {

		switch( mEvent.mButton ) {
			case SDL_SCANCODE_F1:
				mRunStop = true;
				break;

			case SDL_SCANCODE_F2:
				mF2 = true;
				break;

			case SDL_SCANCODE_F3:
				mF3 = true;
				break;
			
			case SDL_SCANCODE_F4:
				mF4 = true;
				break;

			case SDL_SCANCODE_F5:
				mF5 = true;
				break;

			case SDL_SCANCODE_ESCAPE:
				mRestore = true;
				break;

			case SDL_SCANCODE_F10:
				g_Creep.screenGet()->fullscreenToggle();
				break;

			default:
				mKeyPressed = mEvent.mButton;
				mKeyPressedRaw = mEvent.mButtonRaw;
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

    if( mEvent.mType == eEvent_KeyDown )
        pressed = true;

	switch( mEvent.mButton ) {
#ifndef _MACOSX
		case SDL_SCANCODE_RCTRL:
#else
        case SDL_SCANCODE_SPACE:
#endif
                pInput->mButton = pressed;

			break;

		case SDL_SCANCODE_LEFT:
				pInput->mLeft = pressed;

			break;

		case SDL_SCANCODE_RIGHT:
				pInput->mRight = pressed;

			break;

		case SDL_SCANCODE_DOWN:
				pInput->mDown = pressed;
			break;

		case SDL_SCANCODE_UP:
				pInput->mUp = pressed;

			break;

		default:
			break;
	}
}

void cPlayerInput::KeyboardInputSet2( sPlayerInput *pInput ) {
	switch( mEvent.mButton ) {
		case SDL_SCANCODE_KP_0:
			if( mEvent.mType == eEvent_KeyDown )
				pInput->mButton = true;
			else
				pInput->mButton = false;

			break;

		case SDL_SCANCODE_KP_4:

			if( mEvent.mType == eEvent_KeyDown )
				pInput->mLeft = true;
			else
				pInput->mLeft = false;

			break;

		case SDL_SCANCODE_KP_6:
			if( mEvent.mType == eEvent_KeyDown )
				pInput->mRight = true;
			else
				pInput->mRight = false;

			break;

		case SDL_SCANCODE_KP_2:
			if( mEvent.mType == eEvent_KeyDown )
				pInput->mDown = true;
			else
				pInput->mDown = false;
			break;

		case SDL_SCANCODE_KP_8:
			if( mEvent.mType == eEvent_KeyDown )
				pInput->mUp = true;
			else
				pInput->mUp = false;

			break;

		default:
			break;
	}
}
