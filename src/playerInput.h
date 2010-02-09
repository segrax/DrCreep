struct sPlayerInput {
	bool		 mRight, mLeft, mUp, mDown;
	bool		 mButton;

	sPlayerInput() {
		clear();
	}

	void clear() {
		mRight = mLeft = mUp = mDown = mButton = false;
	}
};

class cPlayerInput {
private:
	SDL_Event			 mEvent;
	sPlayerInput		 mInput[2];
	bool				 mRunStop,	mRestore;

	void		 KeyboardCheck();
	void		 KeyboardInputSet1( sPlayerInput *pInput );
	void		 KeyboardInputSet2( sPlayerInput *pInput );

public:
				 cPlayerInput();

	void		 inputCheck( bool pClearAll = false );
	
	sPlayerInput		*inputGet( byte pNumber ) { 

		if( pNumber > 2 )
			return 0;

		return& mInput[ pNumber ];
	}

	bool		 runStopGet() { return mRunStop; }
	bool		 restoreGet() { return mRestore; }
};
