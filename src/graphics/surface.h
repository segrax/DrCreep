struct SDL_Surface;

	struct paletteColor {
		byte	red, green, blue;

		paletteColor() : red(0), green(0), blue(0) { }
	};

	template <class sizeData> class cVideoSurface {
	private:
		bool				 _scaled;

	protected:
		byte				*_buffer;					// Raw image data
		size_t				 _bufferSize;				// Raw image data size
		
		size_t				 _width,	_height;		// Width / Height
		size_t				 _maxWidth, _maxHeight;	

		size_t				 _pixelBytes;
		size_t				 _pitch;					// Bytes per row
		paletteColor		 _palette[16];

	public:
		
							 cVideoSurface<sizeData>( size_t pWidth, size_t pHeight ) {
								_width = pWidth; 
								_height = pHeight;
	
								_pixelBytes = sizeof( sizeData );
								_pitch = _width * _pixelBytes;

								_bufferSize = (_width * _height) * _pixelBytes;
								_buffer = new byte[_bufferSize];

								_scaled = false;

								surfaceWipe();

								const byte C64pal[16][3] = {
									0x00, 0x00, 0x00,
									0xFF, 0xFF, 0xFF, 
									0x68, 0x37, 0x2b, 
									0x70, 0xa4, 0xb2, 
									0x6f, 0x3d, 0x86, 
									0x58, 0x8d, 0x43, 
									0x35, 0x28, 0x79, 
									0xb8, 0xc7, 0x6f, 
									0x6f, 0x4f, 0x25,
									0x43, 0x39, 0x00,
									0x9a, 0x67, 0x59,
									0x44, 0x44, 0x44,
									0x6c, 0x6c, 0x6c,
									0x9a, 0xd2, 0x84,
									0x6c, 0x5e, 0xb5,
									0x95, 0x95, 0x95
								};

								for(int color=0; color < 16; color++) {
									paletteColorSet(color, C64pal[color][0], C64pal[color][1], C64pal[color][2] );
								}
							}

							~cVideoSurface() {
		
								delete _buffer;
							}

		inline	byte		*bufferGet()						{ return _buffer; }
		inline void			 surfaceWipe( dword _pValue = 0x00)	{ 
			dword *buf = (dword*) _buffer;
			for(unsigned int y=0; y < _height; ++y)
				for(unsigned int x=0; x < _width; ++x)
					*buf++ = _pValue;
		}

		inline void paletteColorSet( byte id, byte red, byte green, byte blue ) {
			if(id >= 16)
				return;

			_palette[id].red	= red;
			_palette[id].green	= green;
			_palette[id].blue	= blue;
		}

		inline sizeData		colorGet( size_t pPaletteIndex ) {
			static SDL_Surface *surface = SDL_CreateRGBSurface(	SDL_SWSURFACE,	_width,	_height,	 32, 0, 0, 0, 0);

			// Get the color from the palette
			sizeData color = SDL_MapRGB (	surface->format , _palette[pPaletteIndex].red , _palette[pPaletteIndex].green , _palette[pPaletteIndex].blue ) ;
			return color;
		}

		inline sizeData		*pointGet( size_t pX, size_t pY ) {					// Get pointer to X/Y inside surface
			return (sizeData*) (_buffer + ((pY * _pitch) + (pX * _pixelBytes))); }

		inline void pixelDraw(	size_t pX,	size_t pY,	size_t pPaletteIndex ) {
			dword *pixelPosition = pointGet( pX, pY );

			if(pPaletteIndex > 15) {
				*pixelPosition = pPaletteIndex;
				return;
			}

			// set pixel data
			*pixelPosition =  colorGet(pPaletteIndex);

		}

		SDL_Surface *scaleTo( size_t scaleLevel ) {
			bool			 scaled = _scaled;
			SDL_Surface		*surface = surfaceGet();

			SDL_Surface		*tmpScaled = 0;

			// Invalid Scale?
			if(scaleLevel < 2 || scaleLevel > 4)
				return 0;

			if(_scaled == false) {
				// Set our new dimensions
				_maxWidth	= surface->w * scaleLevel;
				_maxHeight	= surface->h * scaleLevel;
				_scaled = true;	
			}

			// Create a new surface
			tmpScaled = SDL_CreateRGBSurface(SDL_SWSURFACE, _maxWidth, _maxHeight, 32, 0, 0, 0, 0);

			SDL_SetColorKey(	tmpScaled, SDL_SRCCOLORKEY, SDL_MapRGB(tmpScaled->format, 0, 0, 0)	);

			// Do the scale
			if(scaled)
				scale(scaleLevel, tmpScaled->pixels, tmpScaled->pitch, surface->pixels, surface->pitch, surface->format->BytesPerPixel, surface->w, surface->h);
			else
				scale(scaleLevel, tmpScaled->pixels, tmpScaled->pitch, surface->pixels, surface->pitch, surface->format->BytesPerPixel, surface->w, surface->h);

			// Free the old surface
			SDL_FreeSurface( surface );

			return tmpScaled;
		}

		SDL_Surface *surfaceGet(){
			SDL_Surface *surface = SDL_CreateRGBSurface(	SDL_SWSURFACE,	_width,	_height,	 32, 0, 0, 0, 0);
			
			sizeData		*pixel  = (sizeData*) surface->pixels;
			sizeData		*buf	= (sizeData*) _buffer;

			for( unsigned int y = 0; y < _height; y++ ) {
				
				for( unsigned int x = 0; x < _width; x++ ) {

					*pixel++ = *buf++;

				}
			}
			
			return surface;
		}

		void surfacePut( cVideoSurface<sizeData> *pSource, word pX, word pY, word pSourceX = 0, word pSourceY = 0 ) {
			sizeData *pixel;
			sizeData *source = (sizeData*) pSource->pointGet(pSourceX, pSourceY);

			for( word y = 0; y < pSource->_height; ++y ) {
				pixel = pointGet(pX, pY + y);
				for( word x = 0; x < pSource->_width; ++x ) {
					if(*source != 0xFF)
						*pixel++ = *source++;
					else {
						pixel++;
						source++;
					}

				}

			}

		}

	};
