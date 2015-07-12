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

#include "stdafx.h"
#include "creep.h"
cWindow::cWindow( size_t pWidth, size_t pHeight ) {

	mDimensionWindow.mWidth = pWidth;
	mDimensionWindow.mHeight = pHeight;

	mDimensionPlayfield.mWidth = mDimensionWindow.mWidth;
	mDimensionPlayfield.mHeight = mDimensionWindow.mHeight;
}

cWindow::~cWindow() {

	SDL_DestroyRenderer( mRenderer );
	SDL_DestroyWindow( mWindow );

	SDL_Quit();
}

bool cWindow::InitWindow( const std::string& pWindowTitle ) {
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		// TODO: Log error
		//SDL_GetError();
		return false;
	}

	mWindow = SDL_CreateWindow(pWindowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mDimensionWindow.mWidth, mDimensionWindow.mHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!mWindow) {
		// TODO: Log Error
		//SDL_GetError();
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) {
		// TODO: Log Error
		//SDL_GetError()
		return false;
	}

	SDL_RenderSetLogicalSize(mRenderer, gWidth, gHeight);

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	//SetCursor();
	return true;
}

void cWindow::EventCheck() {
	

	SDL_Event SysEvent;
	
	while (SDL_PollEvent( &SysEvent )) {

		cEvent Event;

		switch (SysEvent.type) {
			case SDL_KEYDOWN:
				Event.mType = eEvent_KeyDown;
				Event.mButton = SysEvent.key.keysym.scancode;
				Event.mButtonRaw = SysEvent.key.keysym.sym;
				break;

			case SDL_KEYUP:
				Event.mType = eEvent_KeyUp;
				Event.mButton = SysEvent.key.keysym.scancode;
				Event.mButtonRaw = SysEvent.key.keysym.sym;
				break;

			case SDL_MOUSEMOTION:
				Event.mType =eEvent_MouseMove;
				Event.mPosition = cPosition( SysEvent.motion.x, SysEvent.motion.y );
				break;

			case SDL_MOUSEBUTTONDOWN:

				switch (SysEvent.button.button) {

					case 1:
						Event.mType = eEvent_MouseLeftDown;
						Event.mButton = 1;
						break;

					case 3:
						Event.mType = eEvent_MouseRightDown;
						Event.mButton = 3;
						break;
				}

				Event.mPosition = cPosition( SysEvent.motion.x, SysEvent.motion.y );
				Event.mButtonCount = SysEvent.button.clicks;
				break;

			case SDL_MOUSEBUTTONUP:

				switch (SysEvent.button.button) {

					case 1:
						Event.mType = eEvent_MouseLeftUp;
						Event.mButton = 1;
						break;

					case 3:
						Event.mType = eEvent_MouseRightUp;
						Event.mButton = 3;
						break;
				}

				Event.mPosition = cPosition( SysEvent.motion.x, SysEvent.motion.y );
				Event.mButtonCount = SysEvent.button.clicks;
				break;

			case SDL_QUIT:
				Event.mType = eEvent_Quit;
				break;

			case SDL_WINDOWEVENT: {
				switch (SysEvent.window.event) {
				case  SDL_WINDOWEVENT_SIZE_CHANGED:
					Event.mType = eEvent_Redraw;
					break;
				}
			}

		}

		if ( Event.mType != eEvent_None )
			g_Creep.EventAdd( Event );
	}

}

void cWindow::TitleSet( std::string pWindowText ) {

	SDL_SetWindowTitle( mWindow, pWindowText.c_str() );
}

void cWindow::RenderAt( cScreenSurface* pImage, cPosition pSource ) {
	SDL_Rect Src;
	Src.w = mDimensionPlayfield.mWidth;
	Src.h = mDimensionPlayfield.mHeight;
	Src.x = pSource.mX;
	Src.y = pSource.mY;

	//Draw the texture
	SDL_RenderCopy( mRenderer, pImage->GetTexture(), &Src, NULL);
}

void cWindow::FrameEnd() {

	SDL_RenderPresent( mRenderer );

	SDL_RenderClear( mRenderer );
}

void cWindow::SetCursor() {
	SDL_Cursor* cursor;

	cursor = 	SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_CROSSHAIR );
	SDL_SetCursor(cursor);

}

void cWindow::SetMousePosition( const cPosition& pPosition ) {

	SDL_WarpMouseInWindow( mWindow, pPosition.mX, pPosition.mY );
}

SDL_Renderer* cWindow::GetRenderer() const {

	return mRenderer;
}

SDL_Window*	cWindow::GetWindow() const {
	return mWindow;
}
