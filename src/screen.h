class cVideoWindow;
class cBitmapMulticolor;
class cSprite;

struct sScreenPiece {
	byte mPriority;
};


class cScreen {

	cVideoWindow			*mWindow;
	cBitmapMulticolor		*mBitmap;
	cVideoSurface<dword>	*mSurface;
	byte					*mSurfacePriority;

	cSprite					*mSprites[8];

public:

	cScreen();
	~cScreen();
	

	void					 bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );
	void					 blit( cSprite *pSprite, byte pOwner );
	void					 blit( cVideoSurface<dword> *pSurface, word pDestX, word pDestY, byte pOwner);
	void					 clear( byte pColor );
	void					 spriteDisable();
	void					 spriteDraw();
	cSprite					*spriteGet( byte pCount );
	void					 refresh();
};
