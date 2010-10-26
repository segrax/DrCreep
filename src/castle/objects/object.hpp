class cRoom;

class cObject {
protected:
	eRoomObjects	 mObjectID;
public:
	bool			 mParts;
	byte			 mPart;

	bool			 mDrags;					// object will be dragged when being placed
	eDirection		 mDragDirection;
	size_t			 mLength;

public:
	byte			 mPositionX, mPositionY, mPosition2X, mPosition2Y;
	cRoom			*mRoom;

	cObject( cRoom *pRoom, byte pPosX, byte pPosY ) {
		mPositionX = pPosX;
		mPositionY = pPosY;
		mPosition2X = pPosX;
		mPosition2Y = pPosY;
		mLength = 0;
		
		mPart = 0;
		mParts = false;

		mDrags = false;
		mDragDirection = eDirectionNone;
		mObjectID = eObjectsFinished;

		mRoom = pRoom;
	}
	
	eRoomObjects	objectTypeGet() { return mObjectID; }
	virtual size_t	objectLoad( byte **pBuffer ) = 0;	// Load object from room stream
	virtual size_t	objectSave( byte **pBuffer ) {	// Save object to room stream
		*(*pBuffer)++ = mPositionX;
		*(*pBuffer)++ = mPositionY;

		return 2;
	}
	virtual size_t	objectSave2( byte **pBuffer ) {	// Save object to room stream
		*(*pBuffer)++ = mPosition2X;
		*(*pBuffer)++ = mPosition2Y;

		return 2;
	}
};
