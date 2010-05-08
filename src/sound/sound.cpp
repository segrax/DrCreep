#include "stdafx.h"
#include "sound.h"
#include "creep.h"
#include "resid-0.16/sid.h"

cSound *g_Sound;

/* Audio stream callback */
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	cSID	*sid = g_Sound->sidGet();
	cCreep	*creep = g_Sound->creepGet();

	short	*buff = (short*)stream;
	static	cycle_count cyclesLeft = 0;

	int		samplesLeft = len / 0x2;
	static	int ticks = 0;

	while (samplesLeft > 0) {

		if(ticks<=0) {
			ticks = (*creep->memory(0xDC05)) / 2;
			creep->musicBufferFeed(); 
		}

  		if (cyclesLeft <= 0) {
  			cyclesLeft = 0x42C7;	// NTSC
			--ticks;
  		}

  		// 
  		int sampleCount = sid->clock(cyclesLeft, buff, samplesLeft);

  		samplesLeft -= sampleCount;
  		buff += sampleCount;
  	}
}

cSound::cSound( cCreep *pCreep ) {

	mCreep = pCreep;

	mSID = new cSID();
  	mSID->set_sampling_parameters(1022727.1428571428, SAMPLE_FAST, 0x5622);
	//mSID->set_sampling_parameters(1000000, SAMPLE_FAST, 0x5622 , -1, 0.97);
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
	desired->samples=8192;

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
