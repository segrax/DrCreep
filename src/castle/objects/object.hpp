class cRoom;

struct sObjectPart {
	byte		mX, mY;
	bool		mSet;
	bool		mDrags;					// object will be dragged when being placed
	eDirection	mDragDirection;
	size_t		mLength;

	sObjectPart() { mX = mY = 0; 
				  mSet = false;
				  mDrags = false;
				  mLength = 0;
				  mDragDirection = eDirectionNone;
				}
};

class cObject {
protected:
	eRoomObjects	 mObjectID;
	sObjectPart		 mParts[3];
	
	cRoom			*mRoom;
	
	virtual void partSetPosition( byte pPosX, byte pPosY, byte pPart ) {
		mParts[pPart].mX = pPosX;
		mParts[pPart].mY = pPosY;

		mParts[pPart].mSet = true;
	}

	bool			 mPlaced;
	byte			 mPart,		mPartCount;

public:

	inline bool  isPlaced()			{ return mPlaced; }

	sObjectPart	*partGet()			{ return &mParts[ mPart ]; }
	eDirection	 dragDirectionGet()	{ return mParts[ mPart ].mDragDirection; }
	
	void partSetPosition( byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, mPart );
	}

	void partPlace() {
		++mPart;

		if(mPart >= mPartCount)
			mPlaced = true;
	}

	cObject( cRoom *pRoom, byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, 0 );
		partSetPosition( pPosX, pPosY, 1 );
		partSetPosition( pPosX, pPosY, 2 );

		mPlaced = false;
		mPart = 0;
		mPartCount = 1;

		mObjectID = eObjectsFinished;

		mRoom = pRoom;
	}
	
	eRoomObjects	objectTypeGet() { return mObjectID; }
	virtual size_t	objectLoad( byte **pBuffer ) = 0;	// Load object from room stream
	virtual size_t	objectSave( byte **pBuffer ) {	// Save object to room stream
		*(*pBuffer)++ = mParts[0].mX;
		*(*pBuffer)++ = mParts[0].mY;

		return 2;
	}
	virtual size_t	objectSave2( byte **pBuffer ) {	// Save object to room stream
		*(*pBuffer)++ = mParts[1].mX;
		*(*pBuffer)++ = mParts[1].mY;

		return 2;
	}
};
