class cSID;

class cSound {

private:
	cSID			*mSID;
	cCreep			*mCreep;

	SDL_AudioSpec	*mAudioSpec;

	bool			 devicePrepare();

public:

	 cSound( cCreep *pCreep );
	~cSound();

	cSID			*sidGet() { return mSID; }

};
