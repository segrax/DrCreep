class cRoom;

struct sObjectPart {
	byte		mX, mY;
	bool		mSet;
	bool		mDrags;					// object will be dragged when being placed
	eDirection	mDragDirection;
	size_t		mLength;
	bool		mPlaced;

	sObjectPart() { mX = mY = 0; 
				  mSet = false;
				  mDrags = false;
				  mPlaced = false;
				  mLength = 0;
				  mDragDirection = eDirectionNone;
				}
};

class cObject {
protected:
	eRoomObjects	 mObjectID;
	sObjectPart		 mParts[16];
	
	cRoom			*mRoom;
	
	virtual void partSetPosition( byte pPosX, byte pPosY, byte pPart ) {
		mParts[pPart].mX = pPosX;
		mParts[pPart].mY = pPosY;

		mParts[pPart].mSet = true;
	}

	bool			 mPartsAdd;
	bool			 mPlaced;
	byte			 mPart,		mPartCount;

public:

	inline bool  isPlaced()			{ return mPlaced; }

	void		 mPartAdd()			{ ++mPart; }
	sObjectPart	*partGet()			{ return &mParts[ mPart ]; }
	sObjectPart	*partGet( size_t pPart )			{ return &mParts[ pPart ]; }
	void partSetPosition( byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, mPart );
	}

	virtual void partPlace() {
		mParts[mPart].mPlaced = true;

		++mPart;

		if(mPart >= mPartCount)
			mPlaced = true;
	}

	virtual void partDel() {
		return;
	}

	cObject( cRoom *pRoom, byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, 0 );
		partSetPosition( pPosX, pPosY, 1 );
		partSetPosition( pPosX, pPosY, 2 );

		mPlaced = false;
		mPart = 0;
		mPartCount = 1;
		mPartsAdd = false;

		mObjectID = eObjectsFinished;

		mRoom = pRoom;
	}
	
	eRoomObjects	objectTypeGet() { return mObjectID; }
	virtual void	partAdd() {
		mPlaced = false;
		++mPartCount;
		return;
	}

	virtual size_t	objectLoad( byte **pBuffer ) = 0;	// Load object from room stream
	virtual size_t	objectSave( byte **pBuffer, size_t pPart ) {	// Save object to room stream
		*(*pBuffer)++ = mParts[pPart].mX;
		*(*pBuffer)++ = mParts[pPart].mY;

		return 2;
	}
};
