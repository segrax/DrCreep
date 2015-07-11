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

struct sPlayerInput {
    SDL_Joystick    *mJoystick;
	bool		 mRight, mLeft, mUp, mDown;
	bool		 mButton;

	sPlayerInput() {
        mJoystick = 0;
		clear();
	}

	void clear() {
		mRight = mLeft = mUp = mDown = mButton = false;
	}
};

class cCreep;

class cPlayerInput {
private:
	cEvent				 mEvent;
	sPlayerInput		 mInput[2];
	bool				 mRunStop,	mRestore, mF2, mF3, mF4, mF5;
	bool				 mFullscreen;
	byte				 mKeyPressed, mKeyPressedRaw;

	void		 KeyboardCheck();
	void		 KeyboardInputSet1( sPlayerInput *pInput );
	void		 KeyboardInputSet2( sPlayerInput *pInput );

    void         JoystickInputSet( sPlayerInput *pInput );

public:
				 cPlayerInput();
				~cPlayerInput();

	void		 inputCheck( bool pClearAll, cEvent pEvent );
	void		 inputClear();

	sPlayerInput		*inputGet( byte pNumber ) { 
		if( pNumber > 1 )
			return 0;

		return &mInput[ pNumber ];
	}

    void joystickSet( size_t pPlayer, int pJoyNumber );

#define retVal( C, X ) C ret = X; X = 0; return ret; 

	inline byte  keyGet()		 { retVal( byte, mKeyPressed ); mKeyPressedRaw = 0; }
	inline byte  keyRawGet()	 { retVal( byte, mKeyPressedRaw ); mKeyPressed = 0; }
	inline bool	 f2Get()		 { retVal( bool, mF2) }
	inline bool	 f3Get()		 { retVal( bool, mF3) }
	inline bool	 f4Get()		 { retVal( bool, mF4) }
	inline bool	 f5Get()		 { retVal( bool, mF5) }
	inline bool	 runStopGet()	 { retVal( bool, mRunStop) }
	inline bool	 restoreGet()	 { retVal( bool, mRestore) }
	inline bool	 fullscreenGet() { retVal( bool, mFullscreen ) }
};
