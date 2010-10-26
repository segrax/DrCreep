class cRoom;

class cObjectLightning : public cObject {
public:
	bool		mMachineState;
	bool		mSwitchState;
	byte		mMachine;

public:
	cObjectLightning( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectLightning;
		
		mParts[0].mDragDirection = eDirectionDown;
		mParts[0].mDrags = true;
		mParts[0].mLength = 0;

		mPartCount = 2;

		mMachine = 0;
		mMachineState = false;
		mSwitchState = false;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		if( mMachineState == true )
			*(*pBuffer)++ = 0x40;
		else
			*(*pBuffer)++ = 0x00;

		size_t strSize = cObject::objectSave( pBuffer );
		
		*(*pBuffer)++ = mParts[0].mLength;

		*(*pBuffer)++ = 0x00; *(*pBuffer)++ = 0x00;
		*(*pBuffer)++ = 0x00; *(*pBuffer)++ = 0x00;

		if( mSwitchState == true )
			*(*pBuffer)++ = 0x80;
		else
			*(*pBuffer)++ = 0xC0;

		strSize += objectSave2( pBuffer );
		*(*pBuffer)++ = 0x00;

		*(*pBuffer)++ = mMachine;
		*(*pBuffer)++ = mMachine;
		*(*pBuffer)++ = mMachine;
		*(*pBuffer)++ = mMachine;

		return (strSize + 12);
	}

};
