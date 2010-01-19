
class cBitmapMulticolor {
public:

	cVideoSurface<dword>	*mSurface;

	void	 load( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );

			 cBitmapMulticolor();
			~cBitmapMulticolor();
};
