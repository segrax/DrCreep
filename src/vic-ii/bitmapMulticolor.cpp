/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2015 Robert Crossfield
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
 *  Commodore 64 VIC-II Bitmap Mode Simulation
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "graphics/screenSurface.h"
#include "bitmapMulticolor.h"

cBitmapMulticolor::cBitmapMulticolor() {
	mSurface = new cScreenSurface( 320, 200 );
}

cBitmapMulticolor::~cBitmapMulticolor() {
	delete mSurface;
}

void cBitmapMulticolor::load( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	dword		color = 0;
	ePriority	priority;
	dword		data = 0;

	// Draw 200 Rows
	for( size_t Y = 0; Y < 200; Y += 8) {
		size_t drawY = Y;

		for( size_t X = 0; X < 320; ) {
			// Read byte row
			data = *pBuffer++;

			// Lets draw 8 bits
			for( size_t drawX = X; drawX < (X + 8); drawX += 2 ) {

				switch( data & 0xC0 ) {

					case 0x00:	// Background  (Register BackgroundColor 0)
						priority = ePriority_Background;
						color = pBackgroundColor0;
						break;

					case 0x40:	// Background  (Color Data Upper 4 Bits)
						priority = ePriority_Background;
						color = (*pColorData & 0xF0) >> 4;
						break;

					case 0x80:	// Foreground  (Color Data Lower 4 bits)
						priority = ePriority_Foreground;
						color = *pColorData & 0xF;
						break;

					case 0xC0:	// Foreground  (Color Ram Lower 4 bits)
						priority = ePriority_Foreground;
						color = *pColorRam & 0x0F;
						break;

				}

				mSurface->pixelDraw(drawX, drawY, color, priority, 2);
				data <<= 2;
			} // X

			// Reached last row for current data?
			if(++drawY == (Y + 8)) {
				drawY = Y;
				X += 0x08;

				++pColorData;
				++pColorRam;
			}

		}

	}	// Y

}
