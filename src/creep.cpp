#include "stdafx.h"
#include "graphics/window.h"
#include "graphics/surface.h"
#include "creep.h"
#include "bitmapMulticolor.h"
#include "sprite.h"

cCreep::cCreep() {
	size_t RomSize;

	m64CharRom = fileRead( "char.rom", RomSize );
	mDumpSize = 0;
	mDump = fileRead( "object", mDumpSize );
	mLevel = 0;
	mQuit = false;

	mWindow = new cVideoWindow( 640, 400, 4 );
	mBitmap = new cBitmapMulticolor();
	mSurface = new cVideoSurface<dword>( 640, 400 );

	mMenuMusicScore = 0xFF;

	byte_840 = 0x40;
	byte_841 = 0x20;
	byte_882 = 0x80;
	byte_883 = 0x40;
	byte_884 = 0x20;
	byte_885 = 0x10;
	byte_886 = 0x08;
	byte_887 = 0x04;
	byte_888 = 0x02;
	byte_889 = 0x01;
	byte_88A = 0x80;
	byte_88B = 0x40;
	byte_88C = 0x20;
	byte_88D = 0x10;
	byte_88E = 0x01;

	byte_83F = 0x80;
	byte_11C9 = 0xA0;

	byte_20DE = 0x00;
	byte_24FD = 0x00;
	byte_2E35 = 0x00;
	byte_2E36 = 0xA0;

	byte_45DD = 0x80;
	byte_45DE = 0x40;
	byte_45DF = 0x20;

	byte_5642 = 0x80;

	byte_5643 = 0x20;
	byte_5644 = 0x10;
	byte_5645 = 0x08;
	byte_5646 = 0x04;
	byte_5647 = 0x02;
	byte_5648 = 0x01;
	
	byte_5F57 = 0xA0;
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

	sub_2973();
}

void cCreep::sub_2973() {
	byte byte_29AB, byte_29AC, byte_29AD;

	byte_29AB = byte_29AC = byte_29AD = 0xF8;

	sub_95F();
}

void cCreep::sub_95F() {
	byte A = 0x20;

	// Sprite??
	for( byte X = 0; X < 8; ++X ) {
		mDump[ 0x26 + X ] = A;
		++A;
	}

	mDump[ 0xD025 ] = 0x0A;
	mDump[ 0xD026 ] = 0x0D;

}

// 0B84
void cCreep::mainLoop() {

	while(!mQuit) {
		
		if( Menu() == true )
			continue;

		Game();
	}

}

// 18E4
void cCreep::BlackScreen() {
	word word_30 = 0xFF00;

	byte Y = 0xF9;

	// Disable all sprites
	mDump[ 0x21 ] = 0; 

	for( ; word_30 >= 0xE000; word_30 -= 0x0100) {

		for(;;) {
			mDump[ word_30 + Y ] = 0;
			--Y;
			if(Y == 0xFF)
				break;
		}
	}

	for( Y = 0;; ) {
		mDump[ 0xBD04 + Y ] = byte_889;
		Y += 0x20;
		if(!Y)
			break;
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

	word_3E = *( (word*) &mDump[word_42 + 6] );

	if(mMenuIntro)
		word_3E += 0x2000;

	// Function ptr
	TextGraphicsDraw( );
}

// 15E0
void cCreep::TextGraphicsDraw( ) {
	word func = 0x01;

	while(func) {

		func = *((word*) &mDump[ word_3E ]);
		word_3E  += 2;

		switch( func ) {
			case 0:
				return;

			case 0x0803:	// Doors
				sub_410C( );
				break;
			case 0x0806:	// Walkway
				sub_166A( );
				break;
			case 0x0809:	// Sliding Pole
				sub_1747( );
				break;
			case 0x080C:	// Ladder
				sub_17EE( );
				break;
			case 0x080F:	// Doorbell
				sub_422A( );
				break;
			case 0x0812:	// Lightning Machine
				sub_43E4( );
				break;
			case 0x0815:	// Forcefield
				sub_46AE( );
				break;
			case 0x081E:	// Lock
				sub_4AB6( );
				break;
			
			case 0x082D:	// Conveyor
				sub_5501( );
				break;

			case 0x0821:
			case 0x160A:
				sub_160A( );
				break;
			
			case 0x2A6D:
				DisplayText( );
				break;

			default:
				printf("asd");
				break;
		}

	}

}

// B8D
bool cCreep::Menu() {

	mMenuMusicScore = 0;
	mMenuScreenTimer = 3;
	mMenuScreenCount = 0;
	mMenuIntro = true;
	byte_D10 = 0;

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
			word_3E = 0x0D1A;

			BlackScreen();
			TextGraphicsDraw( );
			
			mBitmap->load( &mDump[ 0xE000 ], &mDump[ 0xCC00 ], &mDump[ 0xD800 ], 0 );
			mSurface->surfaceWipe(0xFF);
			mSurface->surfacePut( mBitmap->mSurface, 0, 0 );
		}
		
		// 0BE1
		if( byte_20DE != 1 )
			if( mMenuMusicScore == 0 )
				++mMenuMusicScore;
			else {
				// TODO 
				// 0C49
			}

		byte_D12 = 0xC8;

		for(;;) {

			_sleep(20);

			if( mMenuScreenTimer )
				handleEvents();
			else {
				// C0D
				// TODO
				//while(byte_2E35)
				//	;

				byte_2E35 = 2;
			}

			mWindow->clear(0);
			mWindow->blit( mSurface->surfaceGet(), 0, 0 );
			
			// C17
			KeyboardJoystickMonitor( byte_D10 );
			if( byte_5F57 )
				break;

			byte_D10 ^= 0x01;
			if( RunRestorePressed == 1 ) {
				optionsMenu();
				if( byte_24FD == 1 ) {
					byte_5F57 = 1;
					break;
				}

				return true;
			}
			--byte_D12;
			if( byte_D12 == 0 )
				break;
		}

		if( byte_5F57 )
			break;
	}
	
	// 0CDD
	byte_20DE = 0;
	mMenuIntro = 0;
	
	char X = 0x0E;

	for(;;) {
		mDump[ 0x20EF + X ] = (mDump[ 0x20EF + X ] & 0xFE);
		mDump[ 0xD404 + X ] = mDump[ 0x20EF + X ];

		X -= 0x07;
		if( X < 0 )
			break;
	}

	return false;
}

void cCreep::optionsMenu() {

}

void cCreep::KeyboardJoystickMonitor( byte pA ) {
	SDL_Event keyevent;
	
	SDL_PollEvent(&keyevent);

	byte_5F58 = pA;
	RunRestorePressed = false;

	switch( keyevent.type ) {
		case SDL_KEYDOWN:
			switch( keyevent.key.keysym.sym ) {
				case SDLK_F1:
					RunRestorePressed = true;
					break;
			}
	}
	
	byte_5F57 = 0;
}

void cCreep::handleEvents() {
	// 2E1D
	//TODO
	//while(byte_2E35)
	// ;
	
	byte_2E35 = 2;

	//sub_2E37();
	sub_2E79();
	sub_3F4F();
	++byte_2E36;

	mBitmap->load( &mDump[ 0xE000 ], &mDump[ 0xCC00 ], &mDump[ 0xD800 ], 0 );
	mSurface->surfaceWipe(0xFF);
	mSurface->surfacePut( mBitmap->mSurface, 0, 0 );

	SpriteDraw();
}

void cCreep::sub_2E79( ) {
	byte X = 0, A;

	for(;;) {
		A = mDump[ 0xBD04 + X ];

		if(! (A & byte_889) ) {
			// 2E8B
			if(! (A & byte_885) ) {
				
				if(! (A & byte_883) ) {
					--mDump[ 0xBD05 + X ];

					if( mDump[ 0xBD05 + X ] != 0 ) {
						if(! (A & byte_888 )) {
							sub_3026(X);
							A = mDump[ 0xBD04 + X];
							if(!(A & byte_883))
								goto s2F72;
							else
								goto s2EF3;
						}
					} else {
					// 2EAD
						if(A & byte_884) 
							goto s2EF3;
						if(!(A & byte_887)) {
							ObjectActions();
							if( mDump[ 0xBD04 + X ] & byte_883)
								goto s2EF3;
						}
						// 2EC2
						if(!(mDump[ 0xBD04 + X ] & byte_888))
							goto s2ED5;

						sub_3026(X);
						if( mDump[ 0xBD04 + X ] & byte_883)
							goto s2EF3;

						goto s2ED5;
					}

				} else {
					// 2EF3
s2EF3:
					GameAction();
				}

			} else {
				// 2ED5
s2ED5:
				objectFunction(X);
				if( mDump[ 0xBD04 + X ] & byte_883 )
					goto s2EF3;
			}
			// 2EF6
			if( mDump[ 0xBD04 + X ] & byte_885 )
				goto s2ED5;

			byte Y = (X >> 5);
			if( (mDump[ 0xBD04 + X ] & byte_886) ) {
				mDump[ 0xBD04 + X ] = byte_889;
				
			} else {
				// 2F16
				word_30 = mDump[ 0xBD01 + X ];
				word_30 <<= 1;

				mDump[ 0x10 + Y ] = (word_30 - 32);

				byte w30 = (word_30 & 0xFF00) >> 8;

				if( w30 )
					A = (mDump[ 0x20 ] | mDump[ 0x2F82 + Y ]);
				else
					A = (mDump[ 0x20 ] ^ 0xFF);

				// Sprites X Bit 8
				mDump[ 0x20 ] = A;
				if((A & mDump[ 0x2F82 + Y ]) && (mDump[ 0x10 + Y ] < 0x58) )
					A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0x21 ];
				else {
					// 2F5B
					mDump[ 0x18 + Y ] = mDump[ 0xBD02 + X ];// + 0x32;
					A = mDump[ 0x21 ] | mDump[ 0x2F82 + Y ];
				}

				// Enabled Sprites
				mDump[ 0x21 ] = A;
				
				mDump[ 0xBD05 + X ] = mDump[ 0xBD06 + X ];
			}
				
		}
s2F72:
		// 2F72
		X += 0x20;
		if( !X )
			break;
	}

}

void cCreep::ObjectActions() {
	
}

void cCreep::ObjectHitsObject( byte pX ) {

}

void cCreep::sub_3026( byte pX ) {
	byte byte_3115 = pX, byte_3116;
	byte byte_311A, byte_311B, byte_3117, byte_3118, byte_3119;

	byte Y = mDump[ 0xBD00 + pX ] << 3;
	
	byte A = mDump[ 0x895 + Y ];
	
	if(A & 0x80) {
		byte_311B = A;
		byte_3117 = mDump[ 0xBD01 + pX ];
		byte_3118 = byte_3117 + mDump[ 0xBD0A + pX];
		if( (byte_3117 + mDump[ 0xBD0A + pX ]) > 0x80 )
			byte_3117 = 0;

		byte_3119 = mDump[ 0xBD02 + pX ];
		byte_311A = byte_3119 + mDump[ 0xBD0B + pX ];
		if( (byte_3119 + mDump[ 0xBD0B + pX ]) > 0x80 )
			byte_3119 = 0;

		Y = 0;
		for(;;) {
			// 3068
			byte_3116 = Y;
			if( byte_3115 != byte_3116 ) {
				A = mDump[ 0xBD04 + Y];

				if( !(A & byte_889) ) {
					
					if( (A & byte_888) ) {
						Y = mDump[ 0xBD00 + Y ] << 3;
						A = mDump[ 0x395 + Y ];

						if( !(A & 0x80 )) {
							if(! (A & byte_311B )) {
								// 808E
								Y = byte_3116;
								A = byte_3118;
								if( A >= mDump[ 0xBD01 + Y ] ) {
									A = mDump[ 0xBD01 + Y ] + mDump[ 0xBD0A + Y ];
									if( A >= byte_3117 ) {
										// 30A5
										if( byte_311A >= mDump[ 0xBD02 + Y ] ) {
											if( (mDump[ 0xBD02 + Y ] + mDump[ 0xBD0B + Y ]) >= byte_3119 ) {
												ObjectHitsObject( pX );
												pX = byte_3116;
												Y = byte_3115;
												ObjectHitsObject( pX );
											}
											// 30C5
											pX = byte_3115;
											Y = byte_3116;
										}
									}
								}
							}
						}
					}
				}
			}

			pX = byte_3115;
			Y = byte_3116;
			// 30CB
			Y+= 0x20;
			if(!Y)
				break;
		}
	}
	// 30D5
}

void cCreep::GameAction() {

}

// Originally this was not a function, but its too big to bother
// implementing in the original location
void cCreep::objectFunction( byte pX ) {
	//2ED5
	byte Y =  mDump[ 0xBD00 + pX ] << 3;
	word func = *((word*) &mDump[ 0x88F + Y ]);
	
	switch(func) {

		case 0x31F6:
			//sub_31F6( pX );
			break;
		
		case 0x3534:
			break;

		case 0x34EF:
			break;

		case 0x3639:
			break;

		case 0x3682:
			break;

		case 0x36B3:
			sub_36B3( pX );
			break;

		case 0x374F:
			break;
		case 0x379A:
			break;
		case 0x3940:
			break;
		case 0x38CE:
			break;
		case 0x3A08:
			break;
		case 0x3A60:
			break;
		case 0x3AEB:
			break;
		case 0x3DDE:
			break;
		case 0x3D6E:
			break;
	}

}

void cCreep::sub_31F6( byte pX ) {
	
}

void cCreep::sub_3F4F() {
	byte X;

	byte_3FD4 = 0;
	
	for(;;) {
		if( byte_3FD4 >= byte_83E ) 
			break;
	
		X = byte_3FD4 << 3;

		byte A = mDump[ 0xBF04 + X ];
		if(A & byte_840) {
			byte Y = mDump[ 0xBF00 + X ] << 2;
		
			word func = *((word*) &mDump[ 0x842 + Y ]);
		
			switch( func ) {
				case 0:
					mDump[ 0xBF04 + X ] ^= byte_840;
					break;
				case 0x3FD5:
					//sub_3FD5( X );
					break;
				case 0x4075:
					//sub_4075( X );
					break;
				case 0x41D8:
					//sub_41D8( X );
					break;
				case 0x42AD:	// Lightning
					sub_42AD( X );
					break;
				case 0x44E7:
					//sub_44E7( X );
					break;
				case 0x45E0:
					break;
				case 0x4647:
					break;
				case 0x475E:
					break;
				case 0x47A7:
					break;
				case 0x4990:
					break;
				case 0x4A68:
					break;
				case 0x4B1A:
					break;
				case 0x4D70:
					break;
				case 0x4E32:
					break;
				case 0x4EA8:
					break;
				case 0x50D2:
					break;
				case 0x538B:		// Moving Sidewalk
					sub_538B( X );
					break;
				case 0x548B:
					break;
				case 0x5611:
					break;
			}

			// 3F93
			A = mDump[ 0xBF04 + X ];
		}
	
		// 3F96
		if( A & byte_841 ) {
			sub_57DF( X );
			--byte_83E;
			A = byte_83E << 3;
			if( X == A )
				break;

			byte Y = A;
			byte byte_3FD3 = 8;

			for(;;) {
				mDump[ 0xBF00 + X ] = mDump[ 0xBF00 + Y ];
				mDump[ 0xBE00 + X ] = mDump[ 0xBE00 + Y ];
				++X;
				++Y;
				--byte_3FD3;
				if( !byte_3FD3 )
					break;
			}
		}

		// 3FC7
		++byte_3FD4;
	}

	//

}

void cCreep::sub_368A( byte &pY  ) {
	byte X;

	sub_3F14( X );
	mDump[ 0xBD00 + X ] = 1;
	
	mDump[ 0xBD02 + X ] = mDump[ 0xBF02 + pY ] + 8;
	mDump[ 0xBD01 + X ] = mDump[ 0xBF01 + pY ];
	mDump[ 0xBD1F + X ] = mDump[ 0xBE00 + pY ];

}

void cCreep::sub_36B3( byte pX ) {
	byte A = mDump[ 0xBD04 + pX ];
	byte Y;

	if(A & byte_885 ) {
		A ^= byte_885;
		A |= byte_886;
		mDump[ 0xBD04 + pX ] = A;
		return;
	}

	if(A & byte_882 ) 
		mDump[ 0xBD04 + pX ] = (A ^ byte_882);

	Y = mDump[ 0xBD1F + pX ];
	A = mDump[ 0x4750 + Y ];
	if( A == 1 ) {
		A = mDump[ 0xBD1E + pX ];
		if( A != 1 ) {
			mDump[ 0xBD1E + pX ] = 1;
			sub_5F6B( pX );
			
			word_3C -= 2;
			mDump[ word_3C + 0 ] = mDump[ word_3C + 0 ] & 0xFB;
			mDump[ word_3C + 4 ] = mDump[ word_3C + 4 ] & 0xBF;

			A = 0x35;
		} else {
			// 3709
			if( mDump[ 0xBD03 + pX ] == 0x35 )
				A = 0x3D;
			else
				A = 0x35;
		}

	} else {
	// 371A
		A = mDump[ 0xBD1E + pX ];
		if( A != 1 )
			return;

		mDump[ 0xBD1E + pX ] = 0;
		sub_5F6B( pX );

		word_3C -= 2;
		mDump[ word_3C + 0 ] |= 4;
		mDump[ word_3C + 4 ] |= 0x40;
		A = 0x41;
	}

	// 3746
	mDump[ 0xBD03 + pX ] = A;
	sub_5D26( pX );
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

void cCreep::textDecode() {
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

		word_30 = (mDump[ word_3E ] & 0x7F) << 3;
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

		drawGraphics( 2, 0x95, gfxPosX, gfxPosY, 0x94 );

		if( ((char) mDump[ word_3E ]) < 0 )
			break;

		++word_3E;
		gfxPosX += 8;
		mTxtX_0 = gfxPosX;
	}

	// 2BD7
	++word_3E;
}

void cCreep::DisplayText( ) {
	//2A6D

	while( (mTextXPos = mDump[ word_3E ]) ) {
		mTextYPos = mDump[ word_3E + 1 ];
		mTextColor = mDump[ word_3E + 2 ];
		mTextFont = mDump[ word_3E + 3 ];

		word_3E += 0x04;

		textDecode( );
	}

	++word_3E;
}

void cCreep::drawGraphics( word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY, byte pTxtCurrentID = 0 ) {
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

void cCreep::sub_160A() {
	byte gfxCurrentID, gfxPosX, gfxPosY;
	char gfxRepeat;

	while( (gfxRepeat = mDump[ word_3E ]) != 0 ) {

		gfxCurrentID = mDump[ word_3E + 1 ];
		gfxPosX = mDump[ word_3E + 2 ];
		gfxPosY = mDump[ word_3E + 3 ];

		--gfxRepeat;

		for( ; gfxRepeat >= 0; --gfxRepeat ) {
		
			drawGraphics( 0, gfxCurrentID, gfxPosX, gfxPosY );
		
			gfxPosX += mDump[ word_3E + 4 ];
			gfxPosY += mDump[ word_3E + 5 ];
		}

		word_3E += 0x06;
	}
	
	++word_3E;
}

void cCreep::sub_166A() {
	byte byte_1744, byte_1745, byte_1746;
	byte gfxCurrentID, gfxPosX, gfxPosY;

	for(;;) {
		
		byte_1746 = mDump[ word_3E ];

		if( ! byte_1746 ) {
			++word_3E;
			return;
		}
		
		gfxPosX = mDump[ word_3E + 1 ];
		gfxPosY = mDump[ word_3E + 2 ];
		
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
			drawGraphics( 0, gfxCurrentID, gfxPosX, gfxPosY );

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

		word_3E += 3;
	}

}

void cCreep::sub_57DF( byte pX ) {

	if( !(mDump[ 0xBF04 + pX ] & byte_83F) ) {
		mTxtX_0 = mDump[ 0xBF01 + pX ];
		mTxtY_0 = mDump[ 0xBF02 + pX ];
		
		drawGraphics( 1, 0, 0, 0, mDump[ 0xBF03 + pX ] );
		
		mDump[ 0xBF04 + pX ] |= byte_83F;
	}


}

void cCreep::sub_5D26( byte &pX ) {
	byte byte_5E8C, byte_5E8D, byte_5E88;
	byte A;

	word word_38 = *((word*) &mDump[ 0xBD03 + pX ]);
	word_38 <<= 1;

	word_38 += 0x3B;
	word_38 += 0x6000;
	
	word_30 = *((word*) &mDump[ word_38 ]);
	
	mDump[ 0xBD09 + pX ] = mDump[ word_30 + 2 ];
	
	byte_5E8D = mDump[ word_30 ];
	mDump[ 0xBD0A + pX ] = byte_5E8D << 2;
	mDump[ 0xBD0B + pX ] = mDump[ word_30 + 1 ];
	
	byte_5E88 = pX >> 5;
	byte Y = byte_5E88;
	// 5D72
	
	word_32 = mDump[ 0x26 + Y ] ^ 8;
	word_32 <<= 6;
	word_32 += 0xC000;

	word_30 += 0x03;
	byte_5E8C = 0;
	
	// 5DB2
	for(;;) {
		Y = 0;

		for(;;) {
			
			if( Y < byte_5E8D )
				A = mDump[ word_30 + Y ];
			else
				A = 0;

			mDump[ word_32 + Y ] = A;
			++Y;
			if( Y >= 3 )
				break;
		}

		++byte_5E8C;
		if( byte_5E8C == 0x15 )
			break;

		if( byte_5E8C < mDump[ 0xBD0B + pX ] ) 
			word_30 += byte_5E8D;
		else 
			word_30 = 0x5E89;
		
		// 5DED
		word_32 += 0x03;
	}

	// 5DFB
	Y = byte_5E88;
	mDump[ 0x26 + Y ] = mDump[ 0x26 + Y ] ^ 8;
	
	// Sprite Color
	mDump[ 0xD027 + Y ]  = mDump[ 0xBD09 + pX ] & 0x0F;

	if( !(mDump[ 0xBD09 + pX ] & byte_88A ))
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD01D ];
	else {
		A = (mDump[ 0xD01D ] | mDump[ 0x2F82 + Y ]);
		mDump[ 0xBD0A + pX ] <<= 1;
	}

	// Sprite X Expansion
	mDump[ 0xD01D ] = A;
	
	// 5E2D
	if( !(mDump[ 0xBD09 + pX ] & byte_88B ))
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD017 ];
	else {
		A = (mDump[ 0xD017 ] | mDump[ 0x2F82 + Y ]);
		mDump[ 0xBD0B + pX ] <<= 1;
	}

	// Sprite Y Expansion
	mDump[ 0xD017 ] = A;

	// 5E4C
	if( !(mDump[ 0xBD09 + pX ] & byte_88C ))
		A = mDump[ 0xD01B ] | mDump[ 0x2F82 + Y ];
	else
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD01B ]; 
	
	// Sprite data priority
	mDump[ 0xD01B ] = A;

	// 5E68
	if(! (mDump[ 0xBD09 + pX ] & byte_88D ))
		A = mDump[ 0xD01C ] | mDump[ 0x2F82 + Y ];
	else
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD01C ];
		
	// MultiColor Enable
	mDump[ 0xD01C ] = A;
}

void cCreep::SpriteDraw() {
	cSprite *sprite;

	byte Y2 = 1;

	for( byte Y = 0; Y < 8; ++Y, Y2 <<= 1 ) {
		if( !(mDump[ 0x21 ] & Y2) )
			continue;

		sprite = new cSprite( 0x0A, 0x0D );

		if( mDump[ 0xD01C ] & Y2)
			sprite->_rMultiColored = true;

		if( mDump[ 0xD017 ] & Y2)
 			sprite->_rDoubleHeight = true;

		if( mDump[ 0xD01D ] & Y2)
			sprite->_rDoubleWidth = true;

		sprite->_color = mDump[ 0xD027 + Y ];
		
		word_32 = (mDump[ 0x26 + Y ] ^ 8);
		word_32 <<= 6;
		word_32 += 0xC000;
		
		sprite->streamLoad( &mDump[ word_32 ] );
		
		word posX = mDump[ 0x10 + Y ];
		word posY = mDump[ 0x18 + Y ];
		if( (mDump [0x20] & Y2) )
			posX += 0x100;

		mSurface->surfacePut( sprite->_surface, posX, posY );
		delete sprite;
	}

}
void cCreep::sub_5F6B( byte &pX ) {
	byte_5FD5 = mDump[ 0xBD01 + pX ] + mDump[ 0xBD0C + pX ];
	
	byte_5FD7 = byte_5FD5 & 3;
	byte_5FD5 = (byte_5FD5 >> 2) - 4;

	byte_5FD6 = mDump[ 0xBD02 + pX ] + mDump[ 0xBD0D + pX ];
	byte_5FD8 = byte_5FD6 & 7;
	byte_5FD6 >>= 3;
	
	sub_5FA3();
}

void cCreep::sub_5FA3() {
	
	word_3C = mDump[ 0x5CE6 + byte_5FD6 ];
	word_3C += mDump[ 0x5D06 + byte_5FD6 ] << 8;

	word_3C <<= 1;
	word_3C += 0xC000;

	byte A = byte_5FD5 << 1;
	word_3C += A;
}

void cCreep::sub_1747() {
	byte byte_17ED;
	byte A, gfxPosX, gfxPosY;

	for(;;) {
	
		byte_17ED = mDump[ word_3E ];

		if( ! byte_17ED ) {
			++word_3E;
			return;
		}

		gfxPosX = mDump[ word_3E + 1 ];
		gfxPosY = mDump[ word_3E + 2 ];
		
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
				
				drawGraphics( 2, 0x27, gfxPosX, gfxPosY, 0x25 );
			} else {
				// 17AA
				drawGraphics( 0, 0x24, gfxPosX, gfxPosY );
			}

			A = mDump[ word_3C ];
			A |= 0x10;
			mDump[ word_3C ] = A;
			--byte_17ED;
			if( !byte_17ED ) {
				word_3E += 0x03;
				break;
			}
			
			gfxPosY += 0x08;
			word_3C += 0x50;
		}
	}
}

void cCreep::sub_17EE() {
	byte byte_18E3, gfxPosX, gfxPosY;
	
	for(;;) {
	
		byte_18E3 = mDump[ word_3E ];
		if( byte_18E3 == 0 ) {
			++word_3E;
			return;
		}

		// 1800
		gfxPosX = mDump[ word_3E + 1 ];
		gfxPosY = mDump[ word_3E + 2 ];

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

				drawGraphics( 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );

			} else {
				// 184C
				if( byte_18E3 == 1 ) {
					mTxtX_0 = gfxPosX;
					mTxtY_0 = gfxPosY;

					drawGraphics( 2, 0x29, gfxPosX, gfxPosY, 0x2A );
				} else {
				// 1874
					gfxPosX -= 4;
					mTxtX_0 = gfxPosX;
					mTxtY_0 = gfxPosY;
					drawGraphics( 2, 0x2C, gfxPosX, gfxPosY, 0x2D );
					gfxPosX += 4;
				}
			}
			// 189C
			if( byte_18E3 != mDump[ word_3E ] )
				mDump[ word_3C ] = ( mDump[ word_3C ] | 1);
			
			--byte_18E3;
			if( byte_18E3 == 0 ) { 
			
				word_3E += 3;
				break;
			}

			mDump[ word_3C ] |= 0x10;
			gfxPosY += 8;
		
			word_3C += 0x50;
		}
	}

}

void cCreep::sub_4AB6() {
	
	byte X, gfxPosX, gfxPosY;

	for(;;) {
		
		if( mDump[ word_3E ] == 0 )
			break;

		sub_5750( X );
		
		mDump[ 0xBF00 + X ] = 7;
		gfxPosX = mDump[ word_3E + 3 ];
		gfxPosY = mDump[ word_3E + 4 ];
		
		byte A = (mDump[ word_3E ] << 4);
		mDump[ word_3E ] |= A;

		for( char Y = 8; Y >= 0; --Y )
			mDump[ 0x6C53 + Y ] = A;

		mDump[ 0xBE00 + X ] = mDump[ word_3E ];
		mDump[ 0xBE01 + X ] = mDump[ word_3E + 2 ];
		SpriteMovement( 0x58, gfxPosX, gfxPosY, 0, X );

		word_3E += 0x05;
	}

	++word_3E;
}

void cCreep::sub_410C() {
	byte byte_41D0 = *level(word_3E++);
	word byte_41D3 = word_3E;
	
	byte X, gfxCurrentID, gfxPosX, gfxPosY;

	for( byte count = 0; count != byte_41D0; ++count) {
		X = *level(word_3E + 7);
		
		gfxCurrentID  = mDump[ 0x41D1 + X ];
		gfxPosX = *level( word_3E + 0 );
		gfxPosY = *level( word_3E + 1 );

		drawGraphics( 0, gfxCurrentID, gfxPosX, gfxPosY );
		// 4159

		sub_5750( X );
		
		gfxPosX += 0x04;
		gfxPosY += 0x10;

		mDump[ 0xBE00 + X ] = count;
		mDump[ 0xBF00 + X ] = 0;

		word word_42 = lvlPtrCalculate( *level( word_3E + 3 ) );
		
		byte A =  (*level( word_42 ) & 0xF);

		mDump[ 0xBE03 + X ] = A;
		A <<= 4;

		A |= mDump[ 0xBE03 + X ];
		mDump[ 0xBE03 + X ] = A;
		
		A = *level( word_3E + 2 );
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
		SpriteMovement( gfxCurrentID, gfxPosX, gfxPosY, 0, X );

		word_3E += 0x08;
	}

}

// Lightning
void cCreep::sub_42AD( byte pX ) {
	byte gfxPosX, gfxPosY;

	byte byte_43E2, byte_43E3;
	word_40 = word_45DB + mDump[ 0xBE00 + pX ];
	byte Y = 0;

	if( mDump[ 0xBE01 + pX ] != 1 ) {
		mDump[ 0xBE01 + pX ] = 1;

		sub_368A( pX );

	} else {
		// 42CF
		byte A = mDump[ word_40 + Y ];
		if( !(A & byte_45DE )) {
			// 42D8
			mDump[ 0xBE01 + pX ] = 0;
			mDump[ 0xBF04 + pX ] ^= byte_840;
			mDump[ 0x66E6 ] = mDump[ 0x66E7 ] = 0x55;

			gfxPosX = mDump[ word_40 + 1 ];
			gfxPosY = mDump[ word_40 + 2 ];
			byte_43E2 = mDump[ word_40 + 3 ];
			
			for(;;) {
				if( !byte_43E2 )
					break;
				
				drawGraphics( 0, 0x34, gfxPosX, gfxPosY, 0 );
				gfxPosY += 0x08;

				--byte_43E2;
			}

			Y = 0;

			// 4326
			for(;;) {
				if( mDump[ 0xBD00 + Y] == 1 ) {
					if( !(mDump[ 0xBD04 + Y ] & byte_889) )
						if( mDump[ 0xBD1F + Y ] == mDump[ 0xBE00 + pX ] )
							break;
				}

				Y+= 0x20;
			}
			// 4345
			mDump[ 0xBD04 + Y ] |= byte_885;
			return;

		} else {
			// 4351
			if( byte_2E36 & 3 )
				return;
		}
	}
	// 435B
	++mDump[ 0xBE02 + pX ];
	byte A = mDump[ 0xBE02 + pX];
	if( A >= 3 ) {
		A = 0;
		mDump[ 0xBE02 + pX ] = A;
	}
	
	byte_43E3 = A;
	
	gfxPosX = mDump[ word_40 + 1 ];
	gfxPosY = mDump[ word_40 + 2 ];
	byte_43E2 = mDump[ word_40 + 3 ];

	for(;;) {
		if( !byte_43E2 )
			break;

		if( byte_43E3 ) {
			if( byte_43E3 != 1 ) {
				mDump[ 0x66E6 ] = 0x66;
				mDump[ 0x66E7 ] = 0x01;

			} else {
			// 43B4
				mDump[ 0x66E6 ] = 0x61;
				mDump[ 0x66E7 ] = 0x06;
			}
		} else {
			// 43A7
			mDump[ 0x66E6 ] = 0x16;
			mDump[ 0x66E7 ] = 0x06;
		}

		// 43BE
		drawGraphics( 0, 0x34, gfxPosX, gfxPosY, 0 );
		++byte_43E3;
		if( byte_43E3 >= 3 )
			byte_43E3 = 0;

		gfxPosY += 0x08;
		--byte_43E2;
	}

}

void cCreep::sub_422A() {
	byte gfxCurrentID, gfxPosX, gfxPosY;
	byte byte_42AB;

	byte_42AB = mDump[ word_3E ];
	++word_3E;
	byte X = 0;

	for( ;; ) {

		if( byte_42AB == 0 )
			return;

		sub_5750(X);
		mDump[ 0xBF00 + X ] = 1;

		gfxPosX = mDump[word_3E];
		gfxPosY = mDump[word_3E+1];
		gfxCurrentID = 0x09;
		mDump[ 0xBE00 + X ] = mDump[ word_3E+2 ];
		
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
		SpriteMovement( gfxCurrentID, gfxPosX, gfxPosY, 0, X);
		word_3E += 0x03;

		--byte_42AB;
	}

}

// Lightning Machine Setup
void cCreep::sub_43E4() {
	byte	byte_44E5, byte_44E6, gfxPosX, gfxPosY;

	word_45DB = word_3E;
	byte_44E5 = 0;

	byte X = 0, A;

	for(;;) {
		
		if( mDump[ word_3E ] & byte_45DF ) {
			++word_3E;
			break;
		}

		sub_5750( X );

		mDump[ 0xBE00 + X ] = byte_44E5;
		if( mDump[ word_3E ] & byte_45DD ) {
			// 441C
			gfxPosX = mDump[ word_3E + 1 ];
			gfxPosY = mDump[ word_3E + 2 ];
			drawGraphics( 0, 0x36, gfxPosX, gfxPosY, 0 );
			
			gfxPosX += 0x04;
			gfxPosY += 0x08;

			mDump[ 0xBF00 + X ] = 3;
			if( mDump[ word_3E ] & byte_45DE )
				A = 0x37;
			else
				A = 0x38;

			SpriteMovement( A, gfxPosX, gfxPosY, 0, X );

		} else {
			// 4467
			mDump[ 0xBF00 + X ] = 2;
			gfxPosX = mDump[ word_3E + 1 ];
			gfxPosY = mDump[ word_3E + 2 ];

			byte_44E6 = mDump[ word_3E + 3 ];
			mDump[ 0xBE03 + X ] = mDump[ word_3E + 3 ];

			for(;; ) {
				if( !byte_44E6 ) 
					break;

				drawGraphics( 0, 0x32, gfxPosX, gfxPosY, 0 );
				gfxPosY += 0x08;
				--byte_44E6;
			}

			gfxPosX -= 0x04;

			SpriteMovement( 0x33, gfxPosX, gfxPosY, 0, X );
			if( mDump[ word_3E ] & byte_45DE )
				mDump[ 0xBF04 + X ] |= byte_840;
		}

		// 44C8
		byte_44E5 += 0x08;
		word_3E += 0x08;
	}
}

void cCreep::sub_46AE() {
	// 46AE
	byte X = 0;
	byte gfxPosX, gfxPosY;

	byte_474F = 0;
	
	for(;;) {
		
		if( mDump[ word_3E ] == 0 ) {
			++word_3E;
			return;
		}

		sub_5750( X );

		mDump[ 0xBF00 + X ] = 4;

		gfxPosX = mDump[ word_3E ];
		gfxPosY = mDump[ word_3E + 1 ];

		drawGraphics( 0, 0x3F, gfxPosX, gfxPosY, 0 );

		// 46EA
		gfxPosX += 4;
		gfxPosY += 8;

		for( char Y = 7; Y >= 0; --Y ) 
			mDump[ 0x6889 + Y ] = 0x55;

		SpriteMovement( 0x40, gfxPosX, gfxPosY, 0, X );

		mDump[ 0xBE00 + X ] = byte_474F;
		mDump[ 0x4750 + byte_474F ] = 1;

		sub_3757( );

		gfxPosX = mDump[ word_3E + 2 ];
		gfxPosY = mDump[ word_3E + 3 ];

		drawGraphics( 0, 0x3E, gfxPosX, gfxPosY, 0 );
		
		++byte_474F;
		word_3E += 0x04;
	}

}

// Moving Sidewalk
void cCreep::sub_538B( byte pX ) {
	
}

void cCreep::sub_5501() {
	word_564B = word_3E;

	byte byte_5649 = 0, gfxPosX = 0, gfxPosY = 0;
	
	byte A, X;

	for(;;) {
		if( mDump[ word_3E ] & byte_5642 ) {
			++word_3E;
			break;
		}
		
		//5527
		A = 0xFF;

		A ^= byte_5646;
		A ^= byte_5645;
		A ^= byte_5644;
		A ^= byte_5643;

		A &= mDump[ word_3E ];
		mDump[ word_3E ] = A;

		sub_5750( X );

		mDump[ 0xBF00 + X ] = 0x0D;
		mDump[ 0xBE00 + X ] = byte_5649;

		mDump[ 0xBF04 + X ] = (mDump[ 0xBF04 + X ] | byte_840);
		
		mTxtX_0 = mDump[ word_3E + 1 ];
		mTxtY_0 = mDump[ word_3E + 2 ];

		drawGraphics( 1, 0x7D, gfxPosX, gfxPosY, 0x7D );
		gfxPosX = mTxtX_0;
		gfxPosY = mTxtY_0;
		
		SpriteMovement( 0x7E, gfxPosX, gfxPosY, 0x7D, X );
		sub_5750(X);

		mDump[ 0xBF00 + X ] = 0x0E;
		mDump[ 0xBE00 + X ] = byte_5649;
		
		gfxPosX = mDump[ word_3E + 3 ];
		gfxPosY = mDump[ word_3E + 4 ];
		byte gfxCurrentID = 0x82;
		byte gfxDecodeMode = 0;

		if( mDump[ word_3E ] & byte_5648 == 0 ) {
			mDump[ 0x70A6 ] = 0xC0;
			mDump[ 0x70A8 ] = 0xC0;
		} else {
			if( mDump[ word_3E ] & byte_5647 == 0 ) {
				// 55BE
				mDump[ 0x70A6 ] = 0xC0;
				mDump[ 0x70A8 ] = 0x20;
			} else {
				// 55CB
				mDump[ 0x70A6 ] = 0x50;
				mDump[ 0x70A8 ] = 0xC0;
			}
		}

		drawGraphics( gfxDecodeMode, gfxCurrentID, gfxPosX, gfxPosY, 0 );
		gfxPosX = mDump[ word_3E + 3] + 0x04;
		gfxPosY = mDump[ word_3E + 4] + 0x08;

		SpriteMovement( 0x83, gfxPosX, gfxPosY, 0, X );
		byte_5649 += 0x05;
		word_3E += 0x05;
	}
}

void cCreep::sub_3757() {
	byte X = 0;

	sub_3F14( X );

	mDump[ 0xBD00 + X ] = 2;
	mDump[ 0xBD01 + X ] = mDump[ word_3E + 2 ];
	mDump[ 0xBD02 + X ] = mDump[ word_3E + 3 ] + 2;
	mDump[ 0xBD03 + X ] = 0x35;

	mDump[ 0xBD1F + X ] = byte_474F;
	mDump[ 0xBD1E + X ] = 0;
	mDump[ 0xBD06 + X ] = 4;
	mDump[ 0xBD0C + X ] = 2;
	mDump[ 0xBD0D + X ] = 0x19;
}

void cCreep::sub_3F14( byte &pX ) {
	
	pX = 0;
	byte A;

	for( ;; ) {
		A = mDump[ 0xBD04 + pX ];
		if( A & byte_889 )
			break;

		pX += 0x20;
		if( pX == 0 )
			return;
	}

	for( byte Y = 0x20; Y > 0; --Y ) {
		mDump[ 0xBD00 + pX ] = 0;
		++pX;
	}
	
	pX -= 0x20;
	mDump[ 0xBD04 + pX ] = byte_882;
	mDump[ 0xBD05 + pX ] = 1;
	mDump[ 0xBD06 + pX ] = 1;
}

void cCreep::SpriteMovement( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX ) {
	//5783
	byte gfxDecodeMode;

	byte A = mDump[ 0xBF04 + pX ];
	if( !(A & byte_83F) ) {
		gfxDecodeMode = 2;
		mTxtX_0 = mDump[ 0xBF01 + pX ];
		mTxtY_0 = mDump[ 0xBF02 + pX ];
		pTxtCurrentID = mDump[ 0xBF03 + pX ];

	} else {
		gfxDecodeMode = 0;
	}

	drawGraphics( gfxDecodeMode, pGfxID, pGfxPosX, pGfxPosY, pTxtCurrentID );
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
