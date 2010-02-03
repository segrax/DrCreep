
class cSprite {
public:
	byte					*_buffer;
	bool					 _rEnabled,		_rMultiColored;
	bool					 _rDoubleWidth,	_rDoubleHeight;
	
	cVideoSurface<dword>	*_surface;

	word					 _X,		_Y;
	byte					 _color,		_multiColor0,	_multiColor1;

	void					 drawMulti( byte *pBuffer );
	void					 drawSingle( byte *pBuffer );

public:

							 cSprite();
							~cSprite();

	inline cVideoSurface<dword>	 *getSurface()			{ return _surface; }
	void						  streamLoad( byte *pBuffer );					// Take a stream, and draw our surface based on it
	
};
