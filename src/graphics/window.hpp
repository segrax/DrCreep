/*
*  The Castles of Dr. Creep
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

class cWindow : public cSingleton<cWindow> {

private:
	SDL_Window*			mWindow;
	SDL_Renderer*		mRenderer;

	cDimension			mOriginalResolution;

	cDimension			mScreenSize;

	byte				mWindow_Multiplier, mWindow_MultiplierPrevious;

	bool				mWindowMode;
	std::map< SDL_JoystickID, SDL_GameController* >	mGameControllers;
	std::vector< SDL_JoystickID >					mGameControllerFree;
protected:

	void				SetWindowSize(const int pMultiplier);

public:

	cWindow();
	~cWindow();

	void				CalculateWindowSize();
	int16_t				CalculateFullscreenSize();

	bool				CanChangeToMultiplier(const size_t pNewMultiplier);

	void				EventCheck();
	void				FrameEnd();

	bool				InitWindow(const std::string& pWindowTitle);

	void				PositionWindow();

	SDL_GameController *ControllerAdd(int pId);
	SDL_JoystickID		ControllerGet(int pId);
	void				ControllerRemove(int pId);

	SDL_JoystickID		ControllerGetFree();
	void				ControllerFree(SDL_JoystickID pId);
	SDL_JoystickID		ControllerIsFree(SDL_JoystickID pId);
	void				ControllerRemoveFree(SDL_JoystickID pId);
	
	void				RenderAt(cScreenSurface* pImage, cPosition pSource = cPosition(0, 0));
	void				RenderShrunk(cScreenSurface* pImage);

	void				WindowIncrease();
	void				WindowDecrease();

	void				SetCursor();
	void				SetMousePosition(const cPosition& pPosition);
	void				SetScreenSize(const cDimension& pDimension);
	void				SetOriginalRes(const cDimension& pDimension);

	void				SetWindowTitle(const std::string& pWindowTitle);

	void				SetFullScreen();

	SDL_Renderer*		GetRenderer() const { return mRenderer; };

	const cDimension	GetWindowSize() const;
	const cDimension	GetScreenSize() const { return mScreenSize; }
	const bool			GetWindowMode() const { return mWindowMode; }

	SDL_Window*			GetWindow() const {	return mWindow;	};
};
