/*
 *  Cannon Fodder
 *  ------------------------
 *
 *  Copyright (C) 2008-2015 Robert Crossfield <robcrossfield@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

class cScreenSurface;
const extern word gWidth, gHeight;

class cWindow : public cSingleton<cWindow> {
	
	private:
		SDL_Window*			mWindow;
		SDL_Renderer*		mRenderer;
		cDimension			mDimensionWindow, mDimensionPlayfield;

	public:

							cWindow( size_t pWidth, size_t pHeight );
							~cWindow();

		bool				InitWindow( const std::string& pWindowTitle );
		void				EventCheck();

		void				FrameEnd();

		void				RenderAt( cScreenSurface* pImage, cPosition pSource );
	/*	void				RenderAt( cImage* pImage, cPosition pSource, cDimension pSourceDimension, cPosition pDestination, cDimension pDestinationDimension );
		cImage*				RenderText( const std::wstring& pString );*/

		void				SetCursor();
		void				SetMousePosition( const cPosition& pPosition );

		void				TitleSet( std::string pWindowText );

		SDL_Renderer*		GetRenderer() const;
		SDL_Window*			GetWindow() const;
};
