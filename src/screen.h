class cVideoWindow;
class cBitmapMulticolor;
class cSprite;
class cScreenSurface;
struct sScreenPiece;

class cScreen {

	cVideoWindow			*mWindow;
	cBitmapMulticolor		*mBitmap;
	cScreenSurface			*mSurface;
	SDL_Surface				*mSDLSurface;
	SDL_Surface				*mSDLSurfaceScaled;
	
	vector< sScreenPiece* >  mCollisions;
	cSprite					*mSprites[8];

	size_t					 mScale;

	void					 blit( cSprite *pSprite, byte pSpriteNo );
	void					 blit( cScreenSurface *pSurface, size_t pDestX, size_t pDestY, bool pPriority, byte pSpriteNo);
	SDL_Surface				*scaleUp();
	void					 SDLSurfaceSet();

public:

							 cScreen( string pWindowTitle );
							~cScreen();
	
	void					 bitmapLoad( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );
	void					 clear( byte pColor );

	void					 scaleSet( byte pScale );

	void					 spriteDisable();
	void					 spriteDraw();
	cSprite					*spriteGet( byte pCount );
	void					 refresh();

	inline vector< sScreenPiece* > *collisionsGet() { return &mCollisions; }
};
