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
 *  Sound
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "sound.h"
#include "creep.h"
#include "resid-0.16/sid.h"

// Call back from Audio Device to fill audio output buffer
void cSound_AudioCallback(void *userdata, Uint8 *stream, int len) {
	cSound  *sound = (cSound*) userdata;

	sound->audioBufferFill( (short*) stream, len );
}

// Constructor, prepare the SID
cSound::cSound( cCreep *pCreep ) {

	mCreep = pCreep;

	mCyclesRemaining = 0;
	mFinalCount = 0;
	mTicks = 0;

	// Prepare the SID
	mSID = new cSID();

	// Set the sampling parameters, PAL, at a rate of 44100Hz
	//mSID->set_sampling_parameters(985248, SAMPLE_FAST, 22050 );
	// 1022727.1428571428
	//
  	mSID->enable_filter(false);
  
  	mSID->reset();

  	// Synchronize the waveform generators
  	mSID->write( 4, 0x08);
  	mSID->write(11, 0x08);
  	mSID->write(18, 0x08);
  	mSID->write( 4, 0x00);
  	mSID->write(11, 0x00);
  	mSID->write(18, 0x00);

	mVal = 0;

	devicePrepare();
}

cSound::~cSound() {

	SDL_CloseAudio();
	delete mAudioSpec;
}

void cSound::audioBufferFill( short *pBuffer, int pBufferSize ) {
	byte *musicBuffer = mCreep->musicBufferGet();
	memset( pBuffer, 0, pBufferSize );

	// Convert buffer size in bytes, to the size in words (each sample is 1 word)
	int samplesRemaining = (pBufferSize / 2);

	// Loop for required number of samples to fill buffer
	while (samplesRemaining > 0) {

		// CIA Timer Fired?
		//if(mTicks <= 0) {

			// buffer to load?
			if( musicBuffer ) {
				// Update the music buffer feed and update the SID
				mCreep->musicBufferFeed();
			} else
				if(!mCyclesRemaining)
					++mFinalCount;

			// Reload the new ptr
			musicBuffer = mCreep->musicBufferGet();

			// Reload the CIA Timer
		//	mTicks = (*mCreep->memory(0xDC05));
		//}

		// Time for video frame update?
  		if (mCyclesRemaining <= 0) {
			mCyclesRemaining =  (g_Creep.mTimerGet() * 60 ) * 6;

			/*mCyclesRemaining = 0x4CC8 * 2;*/
  		}

  		// Clock the SID for 'samplesRemaining', only will do all if there is enough cpu cycles remaining
  		int sampleCount = mSID->clock(mCyclesRemaining, pBuffer, samplesRemaining);
	
		//mTicks -= sampleCount;

		// Decrease number of samples remaining by the number of samples just calculated
  		samplesRemaining -= sampleCount;

		// Increase buffer by the number of samples just calculated
  		pBuffer += sampleCount;
  	}
}

// Write to the SID Registers
void cSound::sidWrite( byte pRegister, byte pValue ) {
	mSID->write( pRegister, pValue );
}

// Prepare the local audio device
bool cSound::devicePrepare() {
	SDL_AudioSpec *desired;

	desired = new SDL_AudioSpec();
	mAudioSpec = new SDL_AudioSpec();

	// FM Quality, 16Bit Signed, Mono
	desired->freq=22050;
	desired->format=AUDIO_S16LSB;
	desired->channels=2;

	// 2048 Samples, at 2 bytes per sample
	desired->samples=0x400;

	// Function to call when the audio playback buffer is empty
	desired->callback = cSound_AudioCallback;

	// Pass a ptr to this class
	desired->userdata = this;

	// Open the audio device
	mVal = SDL_OpenAudio(desired, mAudioSpec);

	delete desired;

	if(mVal < 0 ) {
		cout << "Audio Device Initialization failed: " << SDL_GetError();
		cout << endl;
		return false;
	}

	return true;
}

void cSound::playback( bool pStart ) {
	
	if( pStart && mVal >= 0 ) {
		// Start
		SDL_PauseAudio(0);
		mFinalCount = 0;
	} else
		// Stop
		SDL_PauseAudio(1);

}
