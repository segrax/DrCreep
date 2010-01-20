#include "stdafx.h"
#include "graphics/window.h"
#include "graphics/surface.h"
#include "creep.h"
#include "bitmapMulticolor.h"

cCreep::cCreep() {
	size_t RomSize;

	m64CharRom = fileRead( "char.rom", RomSize );
	mDumpSize = 0;
	mDump = fileRead( "object", mDumpSize );
	mLevel = 0;
	mQuit = false;

	mWindow = new cVideoWindow( 640, 400, 4 );
	mBitmap = new cBitmapMulticolor();

	byte_83F = 0x80;
	byte_11C9 = 0xA0;

}

cCreep::~cCreep() {

	delete m64CharRom;
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

		word_30 = 0x7572;
		word_32 = 0x77F7;

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

	memcpy( &mDump[0x9800], mLevel + 2, size - 2 );
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

// 18E4
void cCreep::BlackScreen() {
	word word_30 = 0xFF00;

	byte Y = 0xF9;

	for( ; word_30 >= 0xE000; word_30 -= 0x0100) {

		for(;;) {
			mDump[ word_30 + Y ] = 0;
			--Y;
			if(Y == 0xFF)
				break;
		}
	}

	byte_83E = 0;
	
}

// 13F0
void cCreep::ClearScreen() {
	
	mWindow->clear(0);
	
	BlackScreen();

	word_30 = 0xC000;

	while( word_30 < 0xC800) {
		mDump[word_30++] = 0;
	}

	byte X, A;

	// 1411
	if( byte_11C9 != 1 )
		X = 1;
	else
		X = 0;

	if( mMenuIntro == true )
		A = mMenuScreenCount;
	else
		A = mDump[ 0x7809 + X ];

	word word_42 = lvlPtrCalculate( A );
	
	A = mDump[word_42] & 0xF;
	
	// 1438
	mDump[ 0x6481 ] = A;
	A <<= 4;
	mDump[ 0x6481 ] |= A;

	word word_3E = *( (word*) &mDump[word_42 + 6] );

	if(mMenuIntro)
		word_3E += 0x2000;

	// Function ptr
	TextGraphicsDraw( word_3E );
}

// 15E0
void cCreep::TextGraphicsDraw( word &pData ) {
	word func = 0x01;

	while(func) {

		func = *((word*) &mDump[ pData ]);
		pData  += 2;

		switch( func ) {
			case 0x0803:	// Doors
				sub_410C( pData );
				break;
			case 0x0806:	// Walkway
				sub_166A( pData );
				break;
			case 0x0809:	// Sliding Pole
				sub_1747( pData );
				break;
			case 0x080C:	// Ladder
				sub_17EE( pData );
				break;
			case 0x080F:	// Doorbell
				sub_422A( pData );
				break;
			case 0x0815:	//Forcefield
				sub_46AE( pData );
				break;
			case 0x0821:
			case 0x160A:
				sub_160A( pData );
				break;
			
			case 0x2A6D:
				DisplayText( pData );
				break;

			default:
				printf("asd");
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
		
		mMenuScreenTimer &= 3;

		if( mMenuScreenTimer != 0 ) {
			++mMenuScreenCount;
			word word_42 = lvlPtrCalculate( mMenuScreenCount );

			if( ((*level( word_42 )) & 0x40) )
				mMenuScreenCount = 0;

			ClearScreen();
		} else {
			static word introAddress = 0x0D1A;

			BlackScreen();
			TextGraphicsDraw( introAddress );
		}
		
		//size_t size;
		//byte *mFile = fileRead("creepy", size );

		// 0BE1
		mBitmap->load( &mDump[ 0xE000 ], &mDump[ 0xCC00 ], &mDump[ 0xD800 ], 0 );
		
		mWindow->blit( mBitmap->mSurface->surfaceGet(), 0, 0 );
		mWindow->clear(0);
	}
	
}

// 5FD9
word cCreep::lvlPtrCalculate( byte pCount ) {
	word word_42 = pCount;
	
	word_42 <<= 1;
	word_42 <<= 1;
	word_42 <<= 1;
	word_42 |= (0x79 << 8);
	if( mMenuIntro )
		word_42 += (0x20 << 8);

	return word_42;
}

void cCreep::Game() {
	
}

void cCreep::textDecode( word &pData ) {
	byte gfxPosX, gfxPosY;

	gfxPosX = mTxtX_0 = mTextXPos;
	gfxPosY = mTxtY_0 = mTextYPos;
	
	if( mTextFont == 0 )
		mTextFontt = 1;
	else
		mTextFontt = mTextFont & 0x03;

	mDump[0x73B5] = mTextFontt << 3;
	mDump[0x73E8] = mDump[0x73B5];

	word_30 = ((mDump[0x73E8] << 1) + 0xEA);
	word_30 += 0x7300;

	for( char Y = 5; Y >= 0; --Y)
		mDump[ word_30 + Y] = mTextColor << 4;
	
	//2AFE
	for(;;) {
		byte X = mTextFont & 0x30;
		X >>= 3;

		word_30 = (mDump[ pData ] & 0x7F) << 3;
		word_30 += mDump[ 0x2BE8 + X ];
		word_30 += (mDump[ 0x2BE9 + X ] << 8);

		// Copy from Char ROM
		for( char count = 7; count >= 0; --count )
			mDump[ 0x2BF0 + count ] = charRom( word_30 + count);

		word_30 = 0x73EA;
		X = 0;

		// 2B50
		for(;;) {
			byte Y = mDump[ 0x2BF0 + X ];
			Y >>= 4;
			Y &= 0x0F;

			word tmp =  mDump[ 0x2BF8 + Y ];
			Y = mDump[ 0x2BF0 + X ] & 0xF;
			tmp += (mDump[ 0x2BF8 + Y] << 8);
			
			*(word*)(&mDump[ word_30 ]) = tmp;

			byte A;

			if( mTextFontt < 2 ) {
				A = 2;

			} else {
				if( mTextFontt == 2 ) {
					mDump[ word_30 + 2 ] = mDump[ word_30 ];
					mDump[ word_30 + 3 ] = mDump[ word_30 + 1 ];

					A = 4;

				} else {
					mDump[ word_30 + 2 ] = mDump[ word_30 ];
					mDump[ word_30 + 4 ] = mDump[ word_30 ];
					mDump[ word_30 + 3 ] = mDump[ word_30 + 1 ];
					mDump[ word_30 + 5 ] = mDump[ word_30 + 1 ];

					A = 6;
				}
			}
			
			//2BAb
			word_30 += A;
			++X;
			if( X >= 8 )
				break;

		}

		drawGraphics( pData, 2, 0x95, gfxPosX, gfxPosY, 0x94 );

		if( ((char) mDump[ pData ]) < 0 )
			break;

		++pData;
		gfxPosX += 8;
		mTxtX_0 = gfxPosX;
	}

	// 2BD7
	++pData;
}

void cCreep::DisplayText( word &pData ) {
	//2A6D

	while( (mTextXPos = mDump[ pData ]) ) {
		mTextYPos = mDump[ pData + 1 ];
		mTextColor = mDump[ pData + 2 ];
		mTextFont = mDump[ pData + 3 ];

		pData += 0x04;

		textDecode( pData );
	}

	++pData;
}

void cCreep::drawGraphics( word &pData, word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY, byte pTxtCurrentID = 0 ) {
	byte gfxPosTopY;
	byte gfxHeight_0;

	byte gfxPosBottomY, gfxBottomY;
	byte gfxDestX;
	byte gfxDestX2;
	byte gfxPosRightX;
	byte gfxDestY;
	byte gfxCurPos;

	byte videoPtr0, videoPtr1;
	byte Counter2;
	byte drawingFirst;

	if( pDecodeMode	!= 0 ) {
		// Draw Text
		word word_38 = pTxtCurrentID;
		
		word_38 <<= 1;
		word_38 += 0x603B;

		word_30 = mDump[ word_38 ];
		word_30 += mDump[ word_38 + 1 ] << 8;
		
		mTxtWidth = mDump[ word_30 ];
		mTxtHeight = mDump[ word_30 + 1];
		mCount = mDump[ word_30 + 1 ];

		mTxtPosLowerY = mTxtY_0 + mTxtHeight;
		--mTxtPosLowerY;

		if( mTxtX_0 < 0x10 ) {
			mTxtDestX = 0xFF;
			mTxtEdgeScreenX = 0xFF;
		} else {
			mTxtDestX = mTxtX_0 - 0x10;
			mTxtEdgeScreenX = 0;
		}		
		
		mTxtDestXLeft = mTxtDestX >> 2;
		mTxtDestXLeft |= (mTxtEdgeScreenX & 0xC0);
		
		mTxtEdgeScreenX <<= 1;
		if( mTxtDestX & 0x80 )
			mTxtEdgeScreenX |= 0x01;

		mTxtDestX <<= 1;
		mTxtDestXRight = mTxtDestXLeft + mTxtWidth;
		--mTxtDestXRight;
		drawingFirst = false;

		word_30 += 0x03;
	} 

	if( pDecodeMode != 1 ) {
		word byte_38;

		//58B7
		// Draw Graphics
		byte_38 = pGfxID << 1;
		byte_38 += 0x603B;
		
		word_32 = *((word*)(&mDump[ byte_38 ]));
		
		mGfxWidth = mDump[ word_32 ];
		mGfxHeight = mDump[ word_32 + 1 ];
		gfxHeight_0 = mGfxHeight;

		//58ED
		gfxPosBottomY = pGfxPosY + mGfxHeight;
		--gfxPosBottomY;

		gfxDestX = (pGfxPosX - 0x10);

		if( pGfxPosX < 0x10 )
			mGfxEdgeOfScreenX = 0xFF;
		else
			mGfxEdgeOfScreenX = 0;

		gfxDestX2 = gfxDestX >> 2;
		gfxDestX2 |= (mGfxEdgeOfScreenX & 0xC0);
		
		mGfxEdgeOfScreenX <<= 1;
		if(gfxDestX & 0x80)
			mGfxEdgeOfScreenX |= 0x1;

		gfxDestX <<= 1;
		
		//592C
		gfxPosRightX = gfxDestX2 + mGfxWidth;
		--gfxPosRightX;
		
		Counter2 = 0;

		word_32 += 3;
		gfxDestY = pGfxPosY;

		if( pGfxPosY < 0xDC )
			byte_38 = 0;
		else
			byte_38 = 0xFF;

		// 595B
		gfxDestY >>= 1;
		if(byte_38 & 0x1) {
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;
		gfxDestY >>= 1;
		if(byte_38 & 0x1) {
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;
		gfxDestY >>= 1;
		if(byte_38 & 0x1) {
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;

		// 596A
		byte_5CE2 = mGfxHeight;
		--byte_5CE2;
		byte_5CE2 >>= 3;
		byte_5CE2 += gfxDestY;

		byte A = gfxDestY;

		if(gfxDestY >= 0x80) {
			A = 0;
			A -= gfxDestY;
		}
		
		videoPtr0 = mDump[0x5CE6 + A];
		videoPtr1 = mDump[0x5D06 + A];
		
		if( gfxDestY > 0x80 ) {
			char a = 0 - videoPtr0;
			videoPtr0 = a;
			
			a = 0 - videoPtr1;
			videoPtr1 = a;
		}

		// 59A8

		byte_38 = pGfxPosX - 0x10;
		if( pGfxPosX < 0x10 )
			A = 0xFF;
		else
			A = 0;

		byte byte38 = byte_38 & 0xFF, byte39 = ((byte_38 & 0xFF00) >> 8);

		byte39 = A;
		
		if( byte39 & 0x01 ) {
			byte38 |= 0x80;
		}
		byte38 >>= 1;
		
		if( byte39 & 0x01 ) {
			byte38 |= 0x80;
		}
		byte38 >>= 1;
		byte39 = A;

		// 59C6
		if((videoPtr0 + byte38) > 0xFF)
			++videoPtr1;

		videoPtr0 += byte38;
		videoPtr1 += byte39;

		if( pDecodeMode == 0 ) {
			gfxPosTopY = pGfxPosY;
			gfxBottomY = gfxPosBottomY;
		}
			
	}

	// 59F0
	if( pDecodeMode == 1 ) {

		gfxPosTopY = mTxtY_0;
		gfxBottomY = mTxtPosLowerY;

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

				if( gfxPosBottomY >= 0xDC || mTxtPosLowerY >= 0xDC ) {
					gfxBottomY = mTxtPosLowerY;
				}  else
					gfxBottomY = gfxPosBottomY;

			} else {

				// 5A49
				if( mTxtPosLowerY >= 0xDC || gfxPosBottomY < 0xDC ) {
					gfxBottomY = gfxPosBottomY;
				} else
					gfxBottomY = mTxtPosLowerY;
			}

		} else {
			//5a60
			gfxBottomY = mTxtPosLowerY;
		}
	}

	// 5A66
	byte gfxCurrentPosY = gfxPosTopY;
	
	word byte_34 = mDump[ 0xBC00 + gfxCurrentPosY ];
	byte_34 |= mDump[ 0xBB00 + gfxCurrentPosY ] << 8;

	// 5A77
	for(;;) {
		
		if( pDecodeMode != 0 && mCount != 0 ) {
			
			if( drawingFirst != 1 ) {
				if( mTxtY_0 == gfxCurrentPosY ) 
					drawingFirst = 1;
			}
			//5A97
			--mCount;

			if( gfxCurrentPosY < 0xC8 ) {
				
				gfxCurPos = mTxtDestXLeft;
				byte byte_36 = ((byte_34 & 0xFF)) + mTxtDestX;
				byte byte_37 = (byte_34 & 0xFF00) >> 8 + mTxtEdgeScreenX;

				word word_36 = byte_36 + (byte_37 << 8);

				// 5AB8
				for( byte Y = 0;; ++Y ) {
					if( gfxCurPos < 0x28 ) {
						
						byte A = mDump[ word_30 + Y ];
						A ^= 0xFF;
						A &= mDump[ word_36 + Y];
						mDump[ word_36 + Y] = A;
					} 
					// 5AC7
					if( gfxCurPos == mTxtDestXRight )
						break;
					
					word_36 += 0x7;
					++gfxCurPos;

				}
			}
			// 5AE1
			word_30 += mTxtWidth;
		}
		
		//5aed
		if( pDecodeMode != 1 && gfxHeight_0 != 0) {
			if( Counter2 != 1 ) {
				if( gfxCurrentPosY == pGfxPosY ) {
					Counter2 = 1;
				} else {
					goto noCarry2;
				}
			}

			--gfxHeight_0;
			if( gfxCurrentPosY < 0xC8 ) {
				// 5b17
				gfxCurPos = gfxDestX2;
				word byte_36 = byte_34 +  (mGfxEdgeOfScreenX << 8);
				byte_36 += gfxDestX;

				for( byte Y = 0; ; ++Y ) {
					// 5B2E
					if( gfxCurPos < 0x28 ) {
						byte A = mDump[ word_32 + Y ];
						A |= mDump[ byte_36 + Y];
						mDump[ byte_36 + Y] = A;
					}

					if( gfxCurPos == gfxPosRightX )
						break;

					//5B43
					byte_36 += 7;
					++gfxCurPos;
				}
			}

			//5B55
			word_32 += mGfxWidth;
		}
noCarry2:;
		//5B61
		if( gfxCurrentPosY == gfxBottomY ) 
			break;

		++gfxCurrentPosY;
		if(gfxCurrentPosY & 0x7 ) {
			++byte_34;
			continue;
		}

		byte_34 += 0x0139;
	}
	// 5B8C

	if(pDecodeMode == 1)
		return;

	if( pGfxPosY & 0x7)
		mDump[0x5CE5] = 1;
	else
		mDump[0x5CE5] = 0;

	// 5BA4
	gfxCurrentPosY = gfxDestY;
	word_30 = videoPtr0;
	word_30 += (0xCC + videoPtr1) << 8;

	// 5BBC
	for( ;; ) {
		if( gfxCurrentPosY < 0x19 ) {
			gfxCurPos = gfxDestX2;
		
			byte Y = 0;

			for( ;; ) {
			
				if( gfxCurPos < 0x28 )
					mDump[ word_30 + Y ] = mDump[ word_32 + Y ];
			
				++Y;
				if( gfxCurPos == gfxPosRightX )
					break;

				++gfxCurPos;
			}
		}

		// 5BE5
		if( gfxCurrentPosY != byte_5CE2 ) {
			++gfxCurrentPosY;
			
			word_32 += mGfxWidth;

		} else {
			// 5BFF
			if( mDump[ 0x5CE5 ] != 1 )
				break;

			mDump[ 0x5CE5 ] = 0;

			if( gfxCurrentPosY != 0xFF )
				if( gfxCurrentPosY >= 0x18 )
					break;
		}				
		// 5C16
		word_30 += 0x28;
	}
	
	// 5C24
	word_32 += mGfxWidth;
	if( pGfxPosY & 0x7 )
		mDump[ 0x5CE5 ] = 1;
	else
		mDump[ 0x5CE5 ] = 0;

	gfxCurrentPosY = gfxDestY;
	word_30 = videoPtr0;
	word_30 += (0xD8 + videoPtr1) << 8;
	
	// 5C56
	for( ;; ) {
		
		if( gfxCurrentPosY < 0x19 ) {
			gfxCurPos = gfxDestX2;
			
			byte Y = 0;

			for(;;) {
				if( gfxCurPos < 0x28 ) 
					mDump[ word_30 + Y ] = mDump[ word_32 + Y ];
				
				++Y;
				if( gfxCurPos == gfxPosRightX )
					break;

				++gfxCurPos;
			}
		}
		//5C7F
		if( gfxCurrentPosY != byte_5CE2 ) {
			++gfxCurrentPosY;
			word_32 += mGfxWidth;
		} else {
		// 5C99
			if( mDump[ 0x5CE5 ] != 1 )
				return;

			mDump[ 0x5CE5 ] = 0;
			if( gfxCurrentPosY != 0xFF )
				if( gfxCurrentPosY >= 0x18 )
					return;
		}
		// 5CB0
		word_30 += 0x28;
	}
}

void cCreep::sub_160A( word &pData ) {
	byte gfxCurrentID, gfxPosX, gfxPosY;
	char gfxRepeat;

	while( (gfxRepeat = mDump[ pData ]) != 0 ) {

		gfxCurrentID = mDump[ pData + 1 ];
		gfxPosX = mDump[ pData + 2 ];
		gfxPosY = mDump[ pData + 3 ];

		--gfxRepeat;

		for( ; gfxRepeat >= 0; --gfxRepeat ) {
		
			drawGraphics( pData, 0, gfxCurrentID, gfxPosX, gfxPosY );
		
			gfxPosX += mDump[ pData + 4 ];
			gfxPosY += mDump[ pData + 5 ];
		}

		pData += 0x06;
	}
	
	++pData;
}

void cCreep::sub_166A( word &pData ) {
	byte byte_1744, byte_1745, byte_1746;
	byte gfxCurrentID, gfxPosX, gfxPosY;

	for(;;) {
		
		byte_1746 = mDump[ pData ];

		if( ! byte_1746 ) {
			++pData;
			return;
		}
		
		gfxPosX = mDump[ pData + 1 ];
		gfxPosY = mDump[ pData + 2 ];
		
		byte_1744 = 1;

		byte_5FD5 = (gfxPosX >> 2);
		byte_5FD5 -= 4;

		byte_5FD6 = (gfxPosY >> 3);
		sub_5FA3();

		// 16A9
		
		for(;;) {
			byte A;

			if( byte_1744 != 1 ) {
				if( byte_1744 != byte_1746 )
					A = 0x1C;
				else
					A = 0x1D;
			} else 
				A = 0x1B;

			// 16C1
			gfxCurrentID = A;
			drawGraphics( pData, 0, gfxCurrentID, gfxPosX, gfxPosY );

			byte_1745 = 1;
			
			// 16D1
			for(;;) {
				
				if( byte_1744 != 1 ) {
					
					if( byte_1744 != byte_1746 )
						A = 0x44;

					else {
						// 16EE
						if( byte_1745 == mGfxWidth )
							A = 0x40;
						else
							A = 0x44;
					}

				} else {
					// 16E2
					if( byte_1745 == 1 )
						A = 0x04;
					else
						A = 0x44;
				}

				// 16F8
				A |= mDump[ word_3C ];
				mDump[ word_3C ] = A;
				
				++byte_1745;
				word_3C += 2;

				if( byte_1745 > mGfxWidth )
					break;
			}
			
			gfxPosX += (mGfxWidth << 2);
			++byte_1744;

			if( byte_1744 > byte_1746 )
				break;
		}
		// 1732

		pData += 3;
	}

}

void cCreep::sub_5FA3() {
	
	word_3C = mDump[ 0x5CE6 + byte_5FD6 ];
	word_3C += mDump[ 0x5D06 + byte_5FD6 ] << 8;

	word_3C <<= 1;
	word_3C += 0xC000;

	byte A = byte_5FD5 << 1;
	word_3C += A;
}

void cCreep::sub_1747( word &pData ) {
	byte byte_17ED;
	byte A, gfxPosX, gfxPosY;

	for(;;) {
	
		byte_17ED = mDump[ pData ];

		if( ! byte_17ED ) {
			++pData;
			return;
		}

		gfxPosX = mDump[ pData + 1 ];
		gfxPosY = mDump[ pData + 2 ];
		
		byte_5FD5 = (gfxPosX >> 2) - 0x04;
		byte_5FD6 = (gfxPosY >> 3);

		sub_5FA3();

		//1781
		for(;;) {
			A = mDump[ word_3C ];
			A &= 0x44;
			if(A) {
				mTxtX_0 = gfxPosX - 4;
				mTxtY_0 = gfxPosY;
				
				drawGraphics( pData, 2, 0x27, gfxPosX, gfxPosY, 0x25 );
			} else {
				// 17AA
				drawGraphics( pData, 0, 0x24, gfxPosX, gfxPosY );
			}

			A = mDump[ word_3C ];
			A |= 0x10;
			mDump[ word_3C ] = A;
			--byte_17ED;
			if( !byte_17ED ) {
				pData += 0x03;
				break;
			}
			
			gfxPosY += 0x08;
			word_3C += 0x50;
		}
	}
}

void cCreep::sub_17EE( word &pData ) {
	byte byte_18E3, gfxPosX, gfxPosY;
	
	for(;;) {
	
		byte_18E3 = mDump[ pData ];
		if( byte_18E3 == 0 ) {
			++pData;
			return;
		}

		// 1800
		gfxPosX = mDump[ pData + 1 ];
		gfxPosY = mDump[ pData + 2 ];

		byte A = gfxPosX >> 2;
		A -= 0x04;

		byte_5FD5 = A;
		A = (gfxPosY >> 3);
		byte_5FD6 = A;

		sub_5FA3();

		// 1828
		for(;;) {
			if( (mDump[ word_3C ] & 0x44) == 0 ) {
				byte  gfxCurrentID;

				if( byte_18E3 != 1 ) 
					gfxCurrentID = 0x28;
				else
					gfxCurrentID = 0x2B;

				drawGraphics( pData, 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );

			} else {
				// 184C
				if( byte_18E3 == 1 ) {
					mTxtX_0 = gfxPosX;
					mTxtY_0 = gfxPosY;

					drawGraphics( pData, 2, 0x29, gfxPosX, gfxPosY, 0x2A );
				} else {
				// 1874
					gfxPosX -= 4;
					mTxtX_0 = gfxPosX;
					mTxtY_0 = gfxPosY;
					drawGraphics( pData, 2, 0x2C, gfxPosX, gfxPosY, 0x2D );
					gfxPosX += 4;
				}
			}
			// 189C
			if( byte_18E3 != mDump[ pData ] )
				mDump[ word_3C ] = ( mDump[ word_3C ] | 1);
			
			--byte_18E3;
			if( byte_18E3 == 0 ) { 
			
				pData += 3;
				break;
			}

			mDump[ word_3C ] |= 0x10;
			gfxPosY += 8;
		
			word_3C += 0x50;
		}
	}

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

		drawGraphics( pData, 0, gfxCurrentID, gfxPosX, gfxPosY );
		// 4159

		sub_5750( X );
		
		gfxPosX += 0x04;
		gfxPosY += 0x10;

		mDump[ 0xBE00 + X ] = count;
		mDump[ 0xBF00 + X ] = 0;

		word word_42 = lvlPtrCalculate( *level( pData + 3 ) );
		
		byte A =  (*level( word_42 ) & 0xF);

		mDump[ 0xBE03 + X ] = A;
		A <<= 4;

		A |= mDump[ 0xBE03 + X ];
		mDump[ 0xBE03 + X ] = A;
		
		A = *level( pData + 2 );
		if(A & 0x80) {
			mDump[ 0xBE01 + X ] = 1;
			A = mDump[ 0xBE03 + X ];

			byte Y = 5;
			while(Y) {
				mDump[ 0x6390 + Y ] = A;
				--Y;
			}

			A = 0x08;
		} else
			A = 0x07;

		// 41B2
		gfxCurrentID = A;
		SpriteMovement( pData, gfxCurrentID, gfxPosX, gfxPosY, 0, X );

		pData += 0x08;
	}

}

void cCreep::sub_422A( word &pData ) {
	byte gfxCurrentID, gfxPosX, gfxPosY;
	byte byte_42AB;

	byte_42AB = mDump[ pData ];
	++pData;
	byte X = 0;

	for( ;; ) {

		if( byte_42AB == 0 )
			return;

		sub_5750(X);
		mDump[ 0xBF00 + X ] = 1;

		gfxPosX = mDump[pData];
		gfxPosY = mDump[pData+1];
		gfxCurrentID = 0x09;
		mDump[ 0xBE00 + X ] = mDump[ pData+2 ];
		
		byte A = 0;
		char Y = 0;

		for(;;) {
			if( mDump[ 0xBF00 + Y ] == 0 ) {

				if( mDump[ 0xBE00 + Y ] == mDump[ 0xBE00 + X ] ) {
					A = mDump[ 0xBE03 + Y ];
					break;
				}
			}	

			Y += 8;
		}

		Y = 8;
		while( Y >= 0 ) {
			mDump[ 0x63D2 + Y ] = A;
			--Y;
		}

		A >>= 4;
		A |= 0x10;

		mDump[ 0x63D6 ] = A;
		SpriteMovement( pData, gfxCurrentID, gfxPosX, gfxPosY, 0, X);
		pData += 0x03;

		--byte_42AB;
	}

}

void cCreep::sub_46AE( word &pData ) {
	// 46AE
	byte X = 0;
	byte gfxPosX, gfxPosY;

	byte_474F = 0;
	
	for(;;) {
		
		if( mDump[ pData ] == 0 ) {
			++pData;
			return;
		}

		sub_5750( X );

		mDump[ 0xBF00 + X ] = 4;

		gfxPosX = mDump[ pData ];
		gfxPosY = mDump[ pData + 1 ];

		drawGraphics( pData, 0, 0x3F, gfxPosX, gfxPosY, 0 );

		// 46EA
		gfxPosX += 4;
		gfxPosY += 8;

		for( char Y = 7; Y >= 0; --Y ) 
			mDump[ 0x6889 + Y ] = 0x55;

		SpriteMovement( pData, 0x40, gfxPosX, gfxPosY, 0, X );

		mDump[ 0xBE00 + X ] = byte_474F;
		mDump[ 0x4750 + byte_474F ] = 1;

		sub_3757( pData );

		gfxPosX = mDump[ pData + 2 ];
		gfxPosY = mDump[ pData + 3 ];

		drawGraphics( pData, 0, 0x3E, gfxPosX, gfxPosY, 0 );
		
		++byte_474F;
		pData += 0x04;
	}

}

void cCreep::sub_3757( word &pData ) {
	byte X = 0;

	sub_3F14( X );

	mDump[ 0xBD00 + X ] = 2;
	mDump[ 0xBD01 + X ] = mDump[ pData + 2 ];
	mDump[ 0xBD02 + X ] = mDump[ pData + 3 ] + 2;
	mDump[ 0xBD03 + X ] = 0x35;

	mDump[ 0xBD1F + X ] = byte_474F;
	mDump[ 0xBD1E + X ] = 0;
	mDump[ 0xBD06 + X ] = 4;
	mDump[ 0xBD0C + X ] = 2;
	mDump[ 0xBD0D + X ] = 0x19;

}

void cCreep::sub_3F14( byte &pX ) {
	
}

void cCreep::SpriteMovement( word &pData, byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX ) {
	//5783
	byte gfxDecodeMode;

	byte A = mDump[ 0xBF04 + pX ];
	if( A & byte_83F ) {
		gfxDecodeMode = 2;
		mTxtX_0 = mDump[ 0xBF01 + pX ];
		mTxtY_0 = mDump[ 0xBF02 + pX ];
		pTxtCurrentID = mDump[ 0xBF03 + pX ];

	} else {
		gfxDecodeMode = 0;
	}

	drawGraphics( pData, gfxDecodeMode, pGfxID, pGfxPosX, pGfxPosY, pTxtCurrentID );
	//57AE
	mDump[ 0xBF04 + pX ] = ((byte_83F ^ 0xFF) & mDump[ 0xBF04 + pX]);
	mDump[ 0xBF03 + pX ] = pGfxID;
	mDump[ 0xBF01 + pX ] = pGfxPosX;
	mDump[ 0xBF02 + pX ] = pGfxPosY;
	mDump[ 0xBF05 + pX ] = mGfxWidth;
	mDump[ 0xBF06 + pX ] = mGfxHeight;

	mDump[ 0xBF05 + pX ] <<= 2;
}

bool cCreep::sub_5750( byte &pX ) {

	if( byte_83E == 0x20 )
		return true;

	pX = byte_83E++;
	pX <<= 3;
	
	char Y = 8;

	while( Y ) {
		mDump[ 0xBF00 + pX ] = 0;
		mDump[ 0xBE00 + pX ] = 0;
		++pX;
		--Y;
	}

	pX -= 8;
	mDump[ 0xBF04 + pX ] = byte_83F;

	return false;
}
