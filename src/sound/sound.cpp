#include "stdafx.h"
#include "sound.h"
#include "creep.h"
#include "resid-0.16/sid.h"

cSound *g_Sound;

/* Prototype of our callback function */
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	int ticks = 1;

	for( int i = 0; i < len; ++i ) {
		
		if( ticks <= 0 ) {
			ticks =* g_Sound->creepGet()->memory(0xDC05) * 380;

			g_Sound->creepGet()->musicBufferFeed();
		}
		
		if(ticks==0)
			return;

		--ticks;

		g_Sound->sidGet()->clock(10);
		int ss = g_Sound->sidGet()->output(8);
		*stream++ = (byte) ss;
	}

}

cSound::cSound( cCreep *pCreep ) {

	mCreep = pCreep;

	mSID = new cSID();
  	//mSID->set_sampling_parameters(14318180.0, SAMPLE_FAST, 4 , -1, 0.97);
	mSID->set_sampling_parameters(1000000, SAMPLE_FAST, 4 , -1, 0.97);
  	mSID->enable_filter(true);
  
  	mSID->reset();
  	// Synchronize the waveform generators (must occur after reset)
  	mSID->write( 4, 0x08);
  	mSID->write(11, 0x08);
  	mSID->write(18, 0x08);
  	mSID->write( 4, 0x00);
  	mSID->write(11, 0x00);
  	mSID->write(18, 0x00);

	devicePrepare();
}

cSound::~cSound() {

	delete mAudioSpec;
}

void cSound::sidWrite( byte pRegister, byte pValue ) {
	mSID->write( pRegister, pValue );
}

void cSound::devicePrepare() {
	/* Open the audio device */
	SDL_AudioSpec *desired, *obtained;

	g_Sound = this;

	/* Allocate a desired SDL_AudioSpec */
	desired = new SDL_AudioSpec();

	/* Allocate space for the obtained SDL_AudioSpec */
	obtained = new SDL_AudioSpec();

	/* 22050Hz - FM Radio quality */
	desired->freq=22050;

	/* 16-bit signed audio */
	desired->format=AUDIO_S16LSB;

	/* Mono */
	desired->channels=0;

	/* Large audio buffer reduces risk of dropouts but increases response time */
	desired->samples=4096;

	/* Our callback function */
	desired->callback=my_audio_callback;

	desired->userdata=NULL;

	/* Open the audio device */
	if ( SDL_OpenAudio(desired, obtained) < 0 ){
	  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	  exit(-1);
	}

	mAudioSpec = obtained;

	/* desired spec is no longer needed */
	delete desired;

	SDL_PauseAudio(0);
}
