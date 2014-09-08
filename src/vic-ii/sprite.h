/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2014 Robert Crossfield
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
 *  ------------------------------------------
 *  Commodore 64 VIC-II Sprite
 *  ------------------------------------------
 */

class cScreenSurface;

class cSprite {
public:
	byte					*_buffer;
	bool					 _rEnabled,		_rMultiColored;
	bool					 _rDoubleWidth,	_rDoubleHeight;
	bool					 _rPriority;

	cScreenSurface			*_surface;

	word					 mX,		mY;
	byte					 _color,		_multiColor0,	_multiColor1;

	void					 drawMulti( byte *pBuffer );
	void					 drawSingle( byte *pBuffer );

public:

							 cSprite();
							~cSprite();

	inline cScreenSurface		 *getSurface()			{ return _surface; }
	void						  streamLoad( byte *pBuffer );					// Take a stream, and draw our surface based on it
	
};
