
class cBitmapMulticolor {
public:

	cScreenSurface		*mSurface;

	void	 load( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );

			 cBitmapMulticolor();
			~cBitmapMulticolor();
};
