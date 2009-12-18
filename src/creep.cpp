#include "stdafx.h"
#include "graphics/window.h"
#include "graphics/surface.h"
#include "creep.h"

cCreep::cCreep() {
	
	mDumpSize = 0;
	mDump = fileRead( "object", mDumpSize );
	mLevel = 0;
	mQuit = false;

	mWindow = new cVideoWindow( 640, 400, 4 );

	byte_11C9 = 0xA0;

}

cCreep::~cCreep() {

	delete mDump;
	delete mLevel;
}

void cCreep::run() {
	
	start();

}

//08C2
void cCreep::start() {
	byte	byte_30, byte_31, count;

	byte_30 = 0x40;
	byte_31 = 0xD7;
	count = 0xC8;
	
	for(;;) {
		
		mDump[0xBC00 + count] = byte_30;
		mDump[0xBB00 + count] = byte_31;

		++count;
		if(count == 0xC8)
			break;

		if( (count & 7) ) {
			++byte_30;
			if(byte_30)
				continue;

			++byte_31;
			
		} else {
			if((byte_30 + 0x39) > 0xFF)
				++byte_31;

			byte_30 += 0x39;
			++byte_31;
		}
	}

	// 0x08F9
	byte_30 = byte_31 = 0;

	count = 0;
	while( count < 0x20 ) {
		mDump[ 0x5CE6 + count ] = byte_30;
		mDump[ 0x5D06 + count ] = byte_31;

		if( (byte_30 + 0x28) > 0xFF)
			++byte_31;

		byte_30 += 0x28;
		++count;
	}

	// 0x091B
	//copyProtection();
	gameMenuDisplaySetup();

	if( mDump[ 0x839 ] != 1 ) {
		changeLevel(0);

	} else {
		// 0x953
		//  JSR     sub_A42
	}

	mainLoop();
}

void cCreep::changeLevel( size_t pNumber ) {
	vector<string> files = directoryList( "castles\\Z*" );
	
	size_t size = 0;
	string lvlFile = "castles\\";
	if(files.size() < pNumber)
		return;

	lvlFile.append( files[pNumber] );

	delete mLevel;
	mLevel = fileRead( lvlFile, size );
}

// 0x7572
void cCreep::gameMenuDisplaySetup() {
	word byte_30;

	byte_30 = 0x400;

	for( char X = 3; X >= 0; --X ) {
		for( word Y = 0; Y <= 0xFF; ++Y )
			mDump[ byte_30 + Y ] = 0x20;

		byte_30 += 0x100;
	}

	word byte_3E;

	mFileListingNamePtr = 0;

	byte_3E = 0x7760;

	//0x759C
	for(;;) {
		byte Y = 0;

		byte A = mDump[ byte_3E + Y ];
		if(A == 0xFF)
			break;

		A = mDump[ byte_3E + 2 ];
		if(A != 0xFF) {

			byte X = mFileListingNamePtr;

			for( Y = 0; Y < 3; ++Y ) {
				
				mDump[ 0xBA00 + X ] = mDump[ byte_3E + Y];
				++X;
			}

			mFileListingNamePtr = ++X;
		}

		// 75C0
		byte X = mDump[ byte_3E + 1 ];
		
		A = mDump[ 0x5CE6 + X ];

		byte_30 = A | (byte_30 & 0xFF00);
		
		A = mDump[ 0x5D06 + X ];
		A += 4;
		byte_30 = (A << 8) | (byte_30 & 0xFF);

		A = (byte_30 & 0xFF);

		A += mDump[ byte_3E ];
		byte_30 = (A) | (byte_30 & 0xFF00);
		
		//0x75E1
		byte_3E += 3;
		
		for(;;) {
			A = mDump[ byte_3E + Y] & 0x3F;
			mDump[ byte_30 + Y ] = A;
			A = mDump[ byte_3E + Y];

			if( A & 0x80 )
				break;
			else
				++Y;
		}

		// 75FC
		++Y;
		byte_3E += Y;
	}

	// 760A
	byte x = mDump[ 0xBA09 ];

	byte A = mDump[ 0x5CE6 + x ];
	byte_30 = (A & 0xFF) | (byte_30 & 0xFF00);

	A = 4 + mDump[ 0x5D06 + x];
	byte_30 = (A << 8) | (byte_30 & 0xFF);
	byte Y = mDump[ 0xBA08 ];
	Y -= 2;

	mDump[ byte_30 + Y ] = 0x3E;
	A = mDump[ 0x5CE6 + 7 ];
	byte_30 = (A & 0xFF) | (byte_30 & 0xFF00);

	// 762F
	byte_30 = ((4 + mDump[ 0x5D06 + x ]) << 8) | (byte_30 & 0xFF);
	
	for( byte Y = 0x17; Y < 0x1A; ++Y ) {
		A = mDump[ byte_30 + Y ] | 0x80;
		mDump[ byte_30 + Y] = A;
	}
}

// 0B84
void cCreep::mainLoop() {

	while(!mQuit) {
		
		Menu();
		Game();
	}

}

// 13F0
void cCreep::ClearScreen() {
	
	mWindow->clear(0);
	
	byte X, A;

	// 1411
	if( byte_11C9 != 1 )
		X = 1;
	else
		X = 0;

	if( mMenuIntro == 1 )
		A = mMenuScreenCount;
	else
		A = mDump[ 0x7809 + X ];

	word word_42 = lvlPtrCalculate( A );
	
	A = (*level(word_42)) & 0xF;

	word *w = (word*) level(word_42 + 4);

	word word_3E = *( (word*) level(word_42 + 6));

	if(mMenuIntro)
		word_3E += 0x2000;

	// Function ptr
	TextGraphicsDraw( word_3E );
}

// 15E0
void cCreep::TextGraphicsDraw( word pData ) {
	word func = 0x01;

	while(func) {

		func = *((word*) level( pData ));
		pData  += 2;

		switch( func ) {
			case 0x0803:
				sub_410C( pData );
				break;

			default:
				break;
		}

	}

}

// B8D
void cCreep::Menu() {

	mMenuMusicScore = 0;
	mMenuScreenTimer = 3;
	mMenuScreenCount = 0;
	mMenuIntro = true;

	for(;;) {
		++mMenuScreenTimer;

		if( (mMenuScreenTimer & 3) == 0 ) {
			++mMenuScreenCount;
			word word_42 = lvlPtrCalculate( mMenuScreenCount );

			if( !((*level( word_42 )) && 0x40) )
				mMenuScreenCount = 0;

			ClearScreen();
		}
	}
	
}

// 5FD9
word cCreep::lvlPtrCalculate( byte pCount ) {
	word word_42 = pCount;
	
	word_42 *= 2;
	word_42 *= 2;
	word_42 *= 2;
	word_42 |= (0x79 << 8);
	if( mMenuIntro )
		word_42 += (0x20 << 8);

	return word_42;
}

void cCreep::Game() {
	
}

void cCreep::drawGraphics( word &pData, word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY ) {
	byte gfxPosTopY;
	word gfx;
		
	byte gfxWidth;
	byte gfxHeight;
	byte gfxHeight_0;

	byte gfxPosBottomY, gfxBottomY;
	byte gfxDestX;
	byte gfxEdgeOfScreenX;
	byte gfxDestX2;
	byte gfxPosRightX;
	byte gfxDestY;
	byte gfxCurPos;

	if( pDecodeMode	!= 0 ) {
		// Draw Text

		//
	} 

	if( pDecodeMode == 1 ) {
		
		
	} else {
		word byte_38;

		//58B7
		// Draw Graphics
		byte_38 = pGfxID << 1;
		byte_38 += 0x603B;
		
		gfx = *((word*)(&mDump[ byte_38 ]));
		
		gfxWidth = mDump[ gfx ];
		gfxHeight = mDump[ gfx+1 ];
		gfxHeight_0 = gfxHeight;

		gfxPosBottomY = pGfxPosY + gfxHeight;
		--gfxPosBottomY;
		//58EA


		gfxDestX = (pGfxPosX - 0x10);

		if( pGfxPosX < 0x10 )
			gfxEdgeOfScreenX = 0xFF;
		else
			gfxEdgeOfScreenX = 0;

		gfxDestX2 = gfxDestX >> 2;

		gfxDestX2 |= (gfxEdgeOfScreenX & 0xC0);
		
		if(gfxDestX & 0x80) {
			gfxEdgeOfScreenX <<= 1;
			gfxEdgeOfScreenX |= 0x1;
		}

		gfxDestX <<= 1;
		
		//592C
		gfxPosRightX = gfxDestX2 + gfxWidth;
		--gfxPosRightX;
		
		byte Counter2 = 0;

		gfx += 3;
		gfxDestY = pGfxPosY;

		if( pGfxPosY < 0xDC )
			byte_38 = 0;
		else
			byte_38 = 0xFF;

		// 595B
		if(byte_38 & 0x1) {
			gfxDestY >>= 1;
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;

		if(byte_38 & 0x1) {
			gfxDestY >>= 1;
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;
		
		if(byte_38 & 0x1) {
			gfxDestY >>= 1;
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;

		// 596A
		byte byte_5CE2 = gfxHeight;
		--byte_5CE2;
		byte_5CE2 >>= 3;
		byte_5CE2 += gfxDestY;

		byte A = gfxDestY;

		if(gfxDestY >= 0x80) {
			A = 0;
			A -= gfxDestY;
		}
		
		byte videoPtr0 = mDump[0x5CE6 + A];
		byte videoPtr1 = mDump[0x5D06 + A];
		
		if( gfxDestY > 0x80 ) {
			char a = 0 - videoPtr0;
			videoPtr0 = a;
			
			a = 0 - videoPtr1;
			videoPtr1 = a;
		}

		// 59A8

		byte_38 = pGfxPosX - 0x10;
		if( !(byte_38 & 0x80) )
			A = 0xFF;
		else
			A = 0;

		byte byte38 = byte_38 & 0xFF, byte39 = ((byte_38 & 0xFF00) >> 8);

		byte39 = A;
		byte38 >>= 1;
		if( byte39 & 0x01 ) {
			byte38 |= 0x80;
		}
		byte38 >>= 1;
		byte39 >>= 1;
		if( byte39 & 0x01 ) {
			byte38 |= 0x80;
		}
		byte39 >>= 1;

		// 59C6
		videoPtr0 += byte38;
		videoPtr1 += byte39;

		if( pDecodeMode == 0 ) {
			gfxPosTopY = pGfxPosY;
			gfxBottomY = gfxPosBottomY;
		}
			
	}

	// 59F0
	if( pDecodeMode == 1 ) {

		// TODO
		//gfxPosTopY = txtY_0;
		//gfxBottomY = txtPosLowerY;

	} else if( pDecodeMode == 2 ) {
		byte A = 0;

		if( pGfxPosY != mTxtY_0 ) {
			if( pGfxPosY >= mTxtY_0 ) {
				
				if( pGfxPosY >= 0xDC || mTxtY_0 < 0xDC ) {
					 A = mTxtY_0;
				}  
			} else {
				// 5a17
				if( mTxtY_0 >= 0xDC || pGfxPosY < 0xDC ) {
					A = mTxtY_0;
				}
			}
		}

		if(!A)
			A = pGfxPosY;
		
		// 5A2E
		gfxPosTopY = A;

		A = 0;

		if( gfxPosBottomY != mTxtPosLowerY ) {

			if( gfxPosBottomY >= mTxtPosLowerY ) {

				if( gfxPosBottomY >= 0xDC || mTxtPosLowerY < 0xDC ) {
					 A = mTxtY_0;
				}  

			} else {

				// 5A49
				if( mTxtPosLowerY >= 0xDC || gfxPosBottomY < 0xDC ) {
					 A = mTxtY_0;
				} 
			}

		} 
			
		if(!A)
			//5A60
			gfxBottomY = mTxtPosLowerY;
	}

	// 5A66
	byte gfxCurrentPosY = gfxPosTopY;
	
	word byte_34 = mDump[ 0xBC00 + gfxCurrentPosY ];
	byte_34 |= mDump[ 0xBB00 + gfxCurrentPosY ] << 8;

	byte drawingFirst;
	byte count;

	// 5A77
	for(;;) {
		
		if( pDecodeMode != 0 && count != 0 ) {
			
			if( drawingFirst != 1 ) {
				if( mTxtY_0 == gfxCurrentPosY ) 
					drawingFirst = 1;
			}
			//5A97
			--count;

			if( gfxCurrentPosY < 0xC8 ) {
				
				gfxCurPos = mTxtDestXLeft;
				byte byte_36 = ((byte_34 & 0xFF00) >> 8) + mTxtDestX;
				byte byte_37 = (byte_34 & 0xFF) + mTxtEdgeScreenX;

				word word_36 = (byte_36 << 8) + byte_37;

				// 5AB8
				for( count = 0;; ++count ) {
					if( gfxCurPos < 0x28 ) {
						
						byte A = mDump[ gfx + count ];
						A ^= 0xFF;
						A &= mDump[ word_36 ];
						mDump[ word_36 ] = A;
					} 
					// 5AC7
					if( gfxCurPos == mTxtDestXRight )
						break;
					
					word_36 += 0x7;
				}
			}
			// 5AE1
			gfx += mTxtWidth;
		}
		
		//5aed


	}
	// 5B8C


}

void cCreep::sub_410C( word &pData ) {
	byte byte_41D0 = *level(pData++);
	word byte_41D3 = pData;
	
	byte X, gfxCurrentID, gfxPosX, gfxPosY;

	for( byte count = 0; count != byte_41D0; ++count) {
		X = *level(pData + 7);
		
		gfxCurrentID  = mDump[ 0x41D1 + X ];
		gfxPosX = *level( pData + 0 );
		gfxPosY = *level( pData + 1 );

		drawGraphics(pData, 0, gfxCurrentID, gfxPosX, gfxPosY );
	}

}
