class cSID;

class cSound {

private:
	cSID			*mSID;
	cCreep			*mCreep;

	SDL_AudioSpec	*mAudioSpec;

	void			 devicePrepare();

public:

	 cSound( cCreep *pCreep );
	~cSound();

	void			 sidWrite( byte pRegister, byte pValue );

	cSID			*sidGet() { return mSID; }
	cCreep			*creepGet() { return mCreep; }

};
