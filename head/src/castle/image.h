class cCastle_Room;
class cCastle_Object;

class cBF00 {
public:
	byte	mField_0;		// 0x00
	byte	mPosX, mPosY;
	byte	mGfxID;			// 0x03
	byte	mField_4;		// 0x04
	byte	mWidth, mHeight;
	byte	mField_7;		// 0x07

			cBF00() {
				mField_0 = mPosX = mPosY = mGfxID = 0;
				mField_4 = mWidth = mHeight = mField_7 = 0;
			}
};

class cBE00 {
public:
	byte	mObjNumber;	// 0x00
	byte	mField_1;
	byte	mField_2;
	byte	mColor;		// 0x03
	byte	mField_4;
	byte	mField_5;
	byte	mField_6;
	byte	mField_7;

			cBE00() {
				
				mObjNumber = mField_1 = mField_2 = 0;
				mColor = mField_4 = mField_5 = 0;
				mField_6 = mField_7 = 0;
				mObject = 0;
			}

	cCastle_Object *mObject;		// replaces mObjNumber

};

class cCastle_Image {
public:
	cCastle_Object	*mObject;

	cBF00			 mBF00;
	cBE00			 mBE00;
	
	byte	mDecodeMode;
	byte	mTxtCurrentID;
	byte	mTxtX_0, mTxtY_0;

public:
			cCastle_Image( cCastle_Object *pObject );

	void	draw();
	void	update( byte pID, byte pPosX, byte pPosY );

};
