enum ePriority {
	ePriority_Background = 0,
	ePriority_Foreground = 1
};

struct sScreenPiece {
	ePriority	mPriority;
	dword		mPixel;
};

class cScreenSurface {
private:
	sScreenPiece	*mScreenPieces;				// Screen buffer
	size_t			 mScreenSize;

	size_t			 mWidth, mHeight;

	dword			 mPalette[16];
	void			 palettePrepare();

public:

					 cScreenSurface( size_t pWidth, size_t pHeight );
					~cScreenSurface( );

	void			 pixelDraw( size_t pX, size_t pY, dword pPaletteIndex, ePriority pPriority );
	void			 Wipe( dword pColor = 0xFF ) ;


	inline size_t		 heightGet() { return mHeight; }
	inline size_t		 widthGet()  { return mWidth; }

	inline sScreenPiece *screenPiecesGet() { return mScreenPieces; }
	inline sScreenPiece *screenPieceGet( size_t &pX, size_t &pY ) {
		return &mScreenPieces[ ((pY * mWidth) + pX) ];
	}
};
