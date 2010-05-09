class cSID;

class cSound {

private:
	cSID			*mSID;
	cCreep			*mCreep;

	SDL_AudioSpec	*mAudioSpec;
	int				 mVal;
	int				 mCyclesRemaining;				// Number of cycles before frame update is complete
	int				 mTicks;						// Number of ticks before CIA timer fires

	bool			 devicePrepare();

public:

	 cSound( cCreep *pCreep );
	~cSound();

	void			 audioBufferFill( short *pBuffer, int pBufferSize );
	void			 sidWrite( byte pRegister, byte pValue );

	void			 playback( bool pStart );
	
	inline cSID		*sidGet() { return mSID; }
	inline cCreep	*creepGet() { return mCreep; }

};
