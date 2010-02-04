#include "stdafx.h"
#include "graphics/screenSurface.h"
#include "bitmapMulticolor.h"


cBitmapMulticolor::cBitmapMulticolor() {
	mSurface = new cScreenSurface( 321, 201 );
}

cBitmapMulticolor::~cBitmapMulticolor() {
	delete mSurface;
}

void cBitmapMulticolor::load( byte *pBuffer, byte *pColorData, byte *pColorRam, byte pBackgroundColor0 ) {
	dword		color = 0;
	byte		data;
	byte		pixel;
	ePriority	priority;

	mSurface->Wipe(0xFF);

	// Draw 160 Rows
	for( size_t Y = 0; Y < 200; Y += 8) {
		size_t drawY = Y;

		for( size_t X = 0; X < 320; ) {
			// Read char row
			data = *pBuffer++;

			// Lets draw 8 bits
			for( size_t drawX = X; drawX < (X + 8); ++drawX ) {

				pixel = (data & 0xC0) >> 6;

				if(pixel == 0 || pixel == 1)
					priority = ePriority_Background;
				else
					priority = ePriority_Foreground;


				if(pixel == 0)
					color = pBackgroundColor0;

				else if(pixel == 1)
					color = (*pColorData & 0xF0) >> 4;

				else if(pixel == 2)
					color = *pColorData & 0xF;

				else if(pixel == 3)
					color = *pColorRam & 0x0F;

				mSurface->pixelDraw(drawX++, drawY, color, priority);
				mSurface->pixelDraw(drawX, drawY, color, priority);
				data <<= 2;
			} // X

			if(++drawY == (Y + 8)) {
				drawY = Y;
				X += 0x8;

				++pColorData;
				++pColorRam;
			}

		}

	}	// Y
}
