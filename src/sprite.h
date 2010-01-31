
class cSprite {
public:
	bool					 _rEnabled,		_rMultiColored;
	bool					 _rDoubleWidth,	_rDoubleHeight;
	
	cVideoSurface<dword>	*_surface;

	byte					 _X,		_Y,		_color,		_multiColor0,	_multiColor1;
public:

							 cSprite();
							~cSprite();

	inline cVideoSurface<dword>	 *getSurface()			{ return _surface; }
	void						  streamLoad( byte *pBuffer );					// Take a stream, and draw our surface based on it
};
