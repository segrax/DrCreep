/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2013 Robert Crossfield
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
 *  Sound
 *  ------------------------------------------
 */

class cSID;

class cSound {

private:
	cSID			*mSID;
	cCreep			*mCreep;

	SDL_AudioSpec	*mAudioSpec;
	int				 mVal;
	int				 mCyclesRemaining;				// Number of cycles before frame update is complete
	int				 mTicks;						// Number of ticks before CIA timer fires
	int				 mFinalCount;

	bool			 devicePrepare();

public:

	 cSound( cCreep *pCreep );
	~cSound();

	void			 audioBufferFill( short *pBuffer, int pBufferSize );
	void			 sidWrite( byte pRegister, byte pValue );

	void			 playback( bool pStart );
	
	inline cSID		*sidGet() { return mSID; }
	inline cCreep	*creepGet() { return mCreep; }
	inline void		 finalCountZero() { 	mFinalCount = 0; }
};
