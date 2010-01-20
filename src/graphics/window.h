
	class cVideoWindow {
	protected:
		SDL_Surface		*_surface;
		size_t			 _width, _height;
		size_t			 _pixelBytes;

	public:
						 cVideoWindow( size_t pWidth, size_t pHeight, size_t pBytesPixel );
						~cVideoWindow();

		void clear( byte color ) {
		
			SDL_FillRect( _surface, NULL, color );
		}

		void blit( SDL_Surface *source, word x, word y, word pSourceX = 0, word pSourceY = 0) {
			SDL_Rect srcRect, destRect;

			srcRect.x = pSourceX;
			srcRect.y = pSourceY;

			srcRect.h = source->h;
			srcRect.w = source->w;

			destRect.x = x;
			destRect.y = y;
			destRect.h = source->h;
			destRect.w = source->w;

			SDL_SetColorKey( source, SDL_SRCCOLORKEY, 0xFF);
			SDL_BlitSurface( source, &srcRect, _surface, &destRect);

			SDL_UpdateRect(  _surface, 0,0,0,0);
		}
	};

