
class cBitmapMulticolor {
public:

	cVideoSurface<dword>	*_surface;

	void	 load( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 );

			 cBitmapMulticolor();
			~cBitmapMulticolor();
};
