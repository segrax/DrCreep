#include "stdafx.h"
#include "graphics/surface.h"
#include "screen.h"
#include "creep.h"
#include "sprite.h"

cCreep::cCreep() {
	size_t RomSize;

	mScreen = new cScreen();
	m64CharRom = fileRead( "char.rom", RomSize );
	mDumpSize = 0;
	mDump = fileRead( "object", mDumpSize );
	mLevel = 0;
	mQuit = false;

	mMenuMusicScore = 0xFF;

	byte_839 = 0;
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
	
	byte_8C0 = 0x80;
	byte_8C1 = 0x40;

	byte_B83 = 0;

	byte_11C9 = 0xA0;

	byte_20DE = 0x00;
	byte_24FD = 0x00;
	byte_2E35 = 0x00;
	byte_2E36 = 0xA0;

	byte_3638 = 0xBA;

	byte_45DD = 0x80;
	byte_45DE = 0x40;
	byte_45DF = 0x20;

	byte_4D60 = 0x80;
	byte_4D61 = 0x40;
	byte_4D62 = 0x20;
	byte_4D63 = 0x10;
	byte_4D64 = 0x08;
	byte_4D65 = 0x04;
	byte_4D66 = 0x02;
	byte_4D67 = 0x01;

	byte_5389 = 0x80;
	byte_538A = 0x01;

	byte_5642 = 0x80;
	byte_5643 = 0x20;
	byte_5644 = 0x10;
	byte_5645 = 0x08;
	byte_5646 = 0x04;
	byte_5647 = 0x02;
	byte_5648 = 0x01;
	byte_574D = 0x04;
	byte_574E = 0x02;
	byte_574F = 0x01;	
	byte_574C = 0x80;

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
		// 93A
		for( byte Y = 0; ; ) {
			for(;;) {
				mDump[ word_30 + Y ] = mDump[ word_32 + Y ];
				if(++Y==0)
					break;
			}
			
			word_32 += 0x100;
			word_30 += 0x100;

			if( word_30 &= 0x7800 )
				break;

		}

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
	mDump[ 0x21 ] = 0;
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
void cCreep::ScreenClear() {
	word word_30 = 0xFF00;

	byte Y = 0xF9;
	
	mScreen->clear(0);

	// Disable all sprites
	mScreen->spriteDisable();

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
void cCreep::roomLoad() {
	
	ScreenClear();

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

	if( mMenuIntro )
		A = mMenuScreenCount;
	else
		A = mDump[ 0x7809 + X ];

	lvlPtrCalculate( A );
	
	// Room Color
	A = mDump[word_42] & 0xF;

	// Set floor colours
	// 1438
	mDump[ 0x6481 ] = A;
	A <<= 4;
	A |= mDump[ 0x6481 ];

	mDump[ 0x6481 ] = A;
	mDump[ 0x648E ] = A;
	mDump[ 0x649B ] = A;
	mDump[ 0x65CC ] = A;
	mDump[ 0x65CE ] = A;
	mDump[ 0x6EAE ] = A;
	mDump[ 0x6EAF ] = A;
	mDump[ 0x6EB0 ] = A;
	mDump[ 0x6EC6 ] = A;
	mDump[ 0x6EC7 ] = A;
	mDump[ 0x6EC8 ] = A;
	mDump[ 0x6EDB ] = A;
	mDump[ 0x6EDC ] = A;
	mDump[ 0x6EED ] = A;
	mDump[ 0x6EEE ] = A;
	mDump[ 0x6EEF ] = A;
	mDump[ 0x6EFC ] = A;
	mDump[ 0x6EFD ] = A;
	mDump[ 0x6EFE ] = A;
	mDump[ 0x6F08 ] = A;
	mDump[ 0x6F09 ] = A;
	mDump[ 0x6F0A ] = A;

	//1487

	for( char Y = 7; Y >= 0; --Y ) {
		mDump[ 0x6FB2 + Y ] = A;
		mDump[ 0x6FF5 + Y ] = A;
		mDump[ 0x7038 + Y ] = A;
		mDump[ 0x707B + Y ] = A;
	}
	
	A &= 0x0F;
	A |= 0x10;
	mDump[ 0x6584 ] = A;
	mDump[ 0x659B ] = mDump[ 0x65CD ] = (mDump[ 0x649B ] & 0xF0) | 0x01;

	//14AC
	// Ptr to start of room data
	word_3E = *( (word*) &mDump[word_42 + 6] );

	if(mMenuIntro)
		word_3E += 0x2000;

	// Function ptr
	roomPrepare( );
}

// 15E0
void cCreep::roomPrepare( ) {
	word func = 0x01;

	while(func) {

		func = *((word*) &mDump[ word_3E ]);
		word_3E  += 2;

		switch( func ) {
			case 0:
				return;

			case 0x0803:	// Doors
				obj_PrepDoors( );
				break;
			case 0x0806:	// Walkway
				obj_PrepWalkway( );
				break;
			case 0x0809:	// Sliding Pole
				obj_PrepSlidingPole( );
				break;
			case 0x080C:	// Ladder
				obj_PrepLadder( );
				break;
			case 0x080F:	// Doorbell
				obj_PrepDoorbell( );
				break;
			case 0x0812:	// Lightning Machine
				obj_PrepLightning( );
				break;
			case 0x0815:	// Forcefield
				obj_PrepForcefield( );
				break;
			case 0x0818:	// Mummy
				obj_PrepMummy( );
				break;
			case 0x081B:	// Key
				obj_PrepKey( );
				break;
			case 0x081E:	// Lock
				obj_PrepLock( );
				break;
			case 0x0824:	// Ray Gun
				obj_PrepRayGun( );
				break;
			case 0x0827:	// Teleport
				obj_PrepTeleport( );
				break;
			case 0x082A:	// Trap Door
				obj_PrepTrapDoor( );
				break;
			case 0x082D:	// Conveyor
				obj_PrepConveyor( );
				break;
			case 0x0830:	// Frankenstein
				obj_PrepFrankenstein( );
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
			lvlPtrCalculate( mMenuScreenCount );

			if( ((*level( word_42 )) & 0x40) )
				mMenuScreenCount = 0;

			roomLoad();
		} else {
			word_3E = 0x0D1A;

			ScreenClear();
			roomPrepare( );
			
			mScreen->bitmapLoad( &mDump[ 0xE000 ], &mDump[ 0xCC00 ], &mDump[ 0xD800 ], 0 );
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

			_sleep(5);

			if( mMenuScreenTimer )
				handleEvents();
			else {
				// C0D
				// TODO
				//while(byte_2E35)
				//	;

				byte_2E35 = 2;
			}

			mScreen->refresh();

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
	byte A = 0, X = 0xFF;

	switch( keyevent.type ) {
		case SDL_KEYDOWN:
			switch( keyevent.key.keysym.sym ) {
				case SDLK_F1:
					RunRestorePressed = true;
					break;
				case SDLK_RCTRL:
					A = 1;
					break;
				case SDLK_LEFT:
					X = 0xFB;
					break;
				case SDLK_RIGHT:
					X = 0xF7;
					break;
				case SDLK_DOWN:
					X = 0xFD;
					break;
				case SDLK_UP:
					X = 0xFE;
					break;
			}
	}

	X &= 0x0F;
	byte_5F56 = mDump[ 0x5F59 + X ];
	byte_5F57 = A;
}

void cCreep::handleEvents() {
	// 2E1D
	//TODO
	//while(byte_2E35)
	// ;
	
	byte_2E35 = 2;

	sub_2E37();
	sub_2E79();
	sub_3F4F();
	++byte_2E36;

	mScreen->bitmapLoad( &mDump[ 0xE000 ], &mDump[ 0xCC00 ], &mDump[ 0xD800 ], 0 );
	mScreen->spriteDraw();
}

void cCreep::sub_29AE() {
	
	byte A = mDump[ word_3E + 1 ];
	sub_29D0( A, 6 );

	A = mDump[ word_3E + 2 ];
	sub_29D0( A, 3 );
	
	A = mDump[ word_3E + 3 ];
	sub_29D0( A, 0 );

}

void cCreep::sub_29D0( byte pA, byte pY ) {
	byte byte_2A6B = pA;
	
	byte byte_2A6C = 0;

	byte A = byte_2A6B >> 4;
	
	for(;;) {
		A << 3;
		byte X = A;
		for(;;) {
			
			A = X;
			A &= 7;
			if( A == 0 )
				break;
			
			mDump[ 0x736C + pY ] = mDump[ 0x2A1B + X ];
			pY += 0x08;
		}

		// 29FE
		++byte_2A6C;
		if( byte_2A6C == 2 )
			break;
		
		pY -= 0x37;
		A = byte_2A6B & 0x0F;
	}
}

void cCreep::sub_2E37() {
	byte gfxSpriteCollision, gfxBackgroundCollision;

	//gfxSpriteCollision = mSprite
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
						if((A & byte_888)) {
							sub_3026(X);
							A = mDump[ 0xBD04 + X];
							if((A & byte_883))
								goto s2EF3;
						}
						
						goto s2F72;
					} else {
					// 2EAD
						if(A & byte_884) 
							goto s2EF3;
						if((A & byte_887)) {
							ObjectActions( X );
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

				byte w30 = (word_30 & 0xFF00) >> 8;

				cSprite *sprite = mScreen->spriteGet( Y );

				// Sprite X
				mDump[ 0x10 + Y ] = (word_30 - 32);
				sprite->_X = (word_30 - 32);

				if((word_30 >= 0x100) && ((word_30 - 32) < 0x100))
					--w30;

				if( (w30 & 0x80) ) {
					// 2f51
					A = (mDump[ 0x2F82 + Y ] ^ 0xFF);

				} else {
					if( w30 ) {
						A = (mDump[ 0x20 ] | mDump[ 0x2F82 + Y ]);
						//sprite->_X += 0x100;
					} else
						A = (mDump[ 0x2F82 ] ^ 0xFF) & mDump[ 0x20 ];

					// Sprites X Bit 8
					mDump[ 0x20 ] = A;

					// 2F45
					if((A & mDump[ 0x2F82 + Y ]) && (mDump[ 0x10 + Y ] >= 0x58) ) {
						A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0x21 ];
						sprite->_rEnabled = false;
					} else {
						// 2F5B
						sprite->_Y = mDump[ 0xBD02 + X ];
						mDump[ 0x18 + Y ] = mDump[ 0xBD02 + X ];// + 0x32;
						A = mDump[ 0x21 ] | mDump[ 0x2F82 + Y ];

						sprite->_rEnabled = true;
					}
				}

				// 2F69
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

// 311E
void cCreep::ObjectActions( byte pX ) {

	byte_31F1 = mDump[ 0xBD01 + pX ];
	byte_31F2 = byte_31F1 + mDump[ 0xBD0A + pX ];
	if( (mDump[ 0xBD01 + pX ] +  mDump[ 0xBD0A + pX ]) > 0xFF )
		byte_31F1 = 0;

	byte_31F3 = mDump[ 0xBD02 + pX ];
	byte_31F4 = byte_31F3 + mDump[ 0xBD0B + pX ];
	if( (mDump[ 0xBD02 + pX ] +  mDump[ 0xBD0B + pX ]) > 0xFF )
		byte_31F3 = 0;

	// 3149
	if( !byte_83E )
		return;

	byte_31EF = byte_83E << 3;
	for( byte Y = 0; Y != byte_31EF; Y = byte_31F0 + 8) {

		byte_31F0 = Y;
		if( mDump[ 0xBD04 + Y ] & byte_83F )
			continue;
		if( byte_31F2 < mDump[ 0xBF01 + Y ] )
			continue;
		if( mDump[ 0xBF01 + Y ] + mDump[ 0xBF05 + Y ] < byte_31F1 )
			continue;
		if( byte_31F4 < mDump[ 0xBF02 + Y ] )
			continue;
		if( mDump[ 0xBF02 + Y ] + mDump[ 0xBF06 + Y ] < byte_31F3 )
			continue;
		//318C
		byte_31F5 = 1;
		Y = mDump[ 0xBD00 + pX ]  << 3;

		ObjectActionFunction( pX, Y );

		Y = byte_31F0;
		if( byte_31F5 == 1 ) 
			mDump[ 0xBD04 + pX ] |= byte_883;

		Y = mDump[ 0xBF00 + byte_31F0 ] << 2;
		ObjectActionFunction2( pX, Y );
	}
}

void cCreep::ObjectActionFunction( byte pX, byte pY ) {
	word func = *((word*) &mDump[ 0x893 + pY ]);
	
	switch( func ) {
		case 0:
			break;
		case 0x34EF:
			sub_34EF( pX, pY );
			break;

		default:
			printf("objectactionfunction");
			break;

	}
}

void cCreep::ObjectActionFunction2( byte pX, byte pY ) {
	word func = *((word*) &mDump[ 0x844 + pY ]);
	
	switch( func ) {
		case 0:
			break;
		
		case 0x4075:
			sub_4075( pX, pY );
			break;

		default:
			printf("objectactionfunction2");
			break;

	}
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
			sub_31F6( pX );
			break;
		
/*		case 0x3534:
			break;

		case 0x34EF:
			break;
*/
		case 0x3639:
			obj_ExecLightning( pX );
			break;

		//case 0x3682:
		//	break;

		case 0x36B3:		// Forcefield
			obj_ExecForcefield( pX );
			break;

		/*case 0x374F:
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
			break;*/
		case 0x3AEB:
			sub_3AEB( pX );
			break;
		/*case 0x3DDE:
			break;
		case 0x3D6E:
			break;*/
		default:
			printf("acc");
			break;
	}

}

void cCreep::sub_31F6( byte pX ) {
	char A =  mDump[ 0xBD04 + pX ];

	if( A & byte_885 ) {
		A ^= byte_885;
		A |= byte_886;
		mDump[ 0xBD04 + pX ] = A;
		A = mDump[ 0xBD1C + pX ];
		A <<= 1;

		char Y = A;

		word_30 = *((word*) &mDump[ 0x34E7 + Y ]);
		word_32 = *((word*) &mDump[ 0x34EB + Y ]);
		
		for( Y = 3; Y >= 0; --Y ) 
			mDump[ word_32 + Y ] = mDump[ word_30 + Y ];

		return;
	} 

	//322C
	if( A & byte_882 ) {
		A ^= byte_882;
		mDump[ 0xBD04 + pX ] = A;
		char Y = mDump[ 0xBD1C + pX ] << 1;
		
		word_32 = *((word*) &mDump[ 0x34E7 + Y ]);
		word_30 = *((word*) &mDump[ 0x34EB + Y ]);

		for( Y = 3; Y >= 0; --Y ) 
			mDump[ word_32 + Y ] = mDump[ word_30 + Y ];
		
		Y = mDump[ 0xBD1C + pX ];
		A = mDump[ 0x780D + Y ];
		if( A != 6 ) {
			sub_3488( pX );
			goto s32DB;
		}

	} else {
		// 3269
		byte Y = mDump[ 0xBD1C + pX ];
		A = mDump[ 0x780D + Y ];

		if( A == 5 ) {
			//3280
			byte_34D6 = Y;
			Y = mDump[ 0xBD1B + pX ];
			A = mDump[ 0x34A4 + Y ];

			if( A != 0xFF ) {
				mDump[ 0x780D + Y ] = A;
				mDump[ 0xBD06 + pX ] = 1;
				A = mDump[ 0x780D + Y ];
				goto s32CB;

			} else {
				// 329E
				mDump[ 0xBD1B + pX ] += 0x04;
				Y = mDump[ 0xBD1B + pX ];

				mDump[ 0xBD01 + pX ] += mDump[ 0x34A1 + Y ];
				mDump[ 0xBD02 + pX ] += mDump[ 0x34A2 + Y ];
			}

		} else if( A == 6 ) {
			mDump[ 0x780D + Y ] = 5;
		} else
			goto s32CB;
	}
	// 32BC
	mDump[ 0xBD03 + pX ] = mDump[ 0x34A3 + mDump[ 0xBD1B + pX ] ];
	sub_3488( pX );
	return;
s32CB:;
	
	if( A != 0 ) {
		mDump[ 0xBD04 + pX ] |= byte_885;
		return;
	}

s32DB:;
	byte AA = mDump[ 0xBD1A + pX ];
	A = AA;
	if( AA != 0xFF )
		if( AA != mDump[ 0xBD19 + pX ] )
			sub_526F( A );

	mDump[ 0xBD19 + pX ] = A;
	mDump[ 0xBD1A + pX ] = 0xFF;
	sub_5F6B( pX );
	
	byte byte_34D5 = mDump[ word_3C ] & mDump[ 0xBD18 + pX ];
	//32FF
	mDump[ 0xBD18 + pX ] = 0xFF;

	if( byte_5FD8 != 0 ) {
		// 3309
		if( !(byte_34D5 & 0x11) ) {
			byte_34D5 &= 0xBB;	
			if( byte_5FD8 >> 1 != byte_5FD7 )
				byte_34D5 &= 0x77;
			else
				byte_34D5 &= 0xDD;
		}

	} else {
		// 3337
		A = byte_5FD7;
		if( A == 3 ) {
			word_3C -= 0x4E;

			byte_34D5 &= 0x75;
			byte_34D5 |= mDump[ word_3C ] & 2;

		} else {
			// 3360
			if( A == 0 ) {
				word_3C -= 0x52;

				byte_34D5 &= 0x5D;
				byte_34D5 |= mDump[ word_3C ] & 0x80;

			} else {
				// 3386
				byte_34D5 &= 0x55;
			}
		}
	}
	// 338E
	KeyboardJoystickMonitor( mDump[ 0xBD1C + pX ] );
	mDump[ 0xBD1D + pX ] = byte_5F57;
	mDump[ 0xBD1E + pX ] = byte_5F56;
	
	byte Y = byte_5F56;
	if( !(Y & 0x80 )) {

		if( mDump[ 0x2F82 + Y ] & byte_34D5 ) {
			mDump[ 0xBD1F + pX ] = Y;
			goto s33DE;

		} 

		// 33B2
		A = mDump[ 0xBD1F + pX ];

		if(!( A & 0x80 )) {
			A += 1;
			A &= 7;
			if( A != byte_5F56 ) {
				A -= 2;
				A &= 7;
				if( A != byte_5F56 )
					goto s33D6;
			}
			
			if( mDump[ 0x2F82 + mDump[ 0xBD1F + pX ] ] & byte_34D5 )
				goto s33DE;
		}
	}
s33D6:;
	// 33D6
	mDump[ 0xBD1F + pX ] = 0x80;
	return;

	// 33DE
s33DE:;
	A = (mDump[ 0xBD1F + pX ] & 3);

	if( A == 2 ) {
		mDump[ 0xBD02 + pX ] -= byte_5FD8;

	} else {
		// 33F4
		if( A == 0 ) {
			mDump[ 0xBD01 + pX ] -= byte_5FD7;
			++mDump[ 0xBD01 + pX ];
		}
	}
	// 3405
	Y = mDump[ 0xBD1F + pX ];
	mDump[ 0xBD01 + pX ] += mDump[ 0x34D7 + Y ];
	mDump[ 0xBD02 + pX ] += mDump[ 0x34DF + Y ];

	if( !(Y & 3) ) {
		// 3421
		if( byte_34D5 & 1 ) {
			A = mDump[ 0xBD1F + pX ];
			if( !A )
				++mDump[ 0xBD03 + pX ];
			else 
				--mDump[ 0xBD03 + pX ];
			
			// 3436
			A = mDump[ 0xBD03 + pX ];
			if( A >= 0x2E ) {
				// 3445
				if( A >= 0x32 )
					mDump[ 0xBD03 + pX ] = 0x2E;
			} else {
				// 343D
				mDump[ 0xBD03 + pX ] = 0x31;
			}

		} else {
			//3451
			mDump[ 0xBD03 + pX ] = 0x26;
		}

	} else {
		// 3459
		++mDump[ 0xBD03 + pX ];
		if( mDump[ 0xBD1F + pX ] < 4 ) {
			// 3463
			A = mDump[ 0xBD03 + pX ];
			if( A >= 6 || A < 3 )
				mDump[ 0xBD03 + pX ] = 3;

		} else {
			// 3476
			if( mDump[ 0xBD03 + pX ] >= 3 )
				mDump[ 0xBD03 + pX ] = 0;
		}
	}

	// 3482
	sub_3488( pX );

}

// 3488: 
void cCreep::sub_3488( byte pX ) {
	hw_SpritePrepare( pX );

	byte_34D6 = pX >> 5;

	mDump[ 0xD027 + byte_34D6 ] = mDump[ 0x34D3 + mDump[ 0xBD1C + pX ] ];
}

// 3AEB: 
void cCreep::sub_3AEB( byte pX ) {
	char A = mDump[ 0xBD04 + pX ];
	char byte_3F0A, byte_3F0B, byte_3F10, byte_3F11, byte_3F12;

	if( A & byte_885 ) {
		mDump[ 0xBD04 + pX ] = (A ^ byte_885) | byte_886;
		return;
	}

	if( A & byte_882 ) 
		mDump[ 0xBD04 + pX ] ^= byte_882;

	word_40 = word_5748 + mDump[ 0xBD1F + pX ] + (mDump[ 0xBD1E + pX ] << 8);
	
	if( !(mDump[ 0xBD1E + pX ] & byte_574E) ) {
		if( mMenuIntro == 1 )
			return;

		
		for(byte_3F0A = 1; byte_3F0A >= 0; --byte_3F0A) {
			byte Y = byte_3F0A;

			if( mDump[ 0x780D + Y ] != 0 )
				continue;

			Y = mDump[ 0x34D1 + Y ];
			A = mDump[ 0xBD02 + pX ];
			A -= mDump[ 0xBD02 + Y ];
			if( A >= 4 )
				continue;

			// 3B4A
			A = mDump[ 0xBD01 + pX ];
			A -= mDump[ 0xBD01 + Y ];
			if(A >= 0) {
				A = mDump[ 0xBD1E + pX ];
				if( !(A & byte_574F) )
					continue;
				else
					goto s3B6E;
			}
			A = mDump[ 0xBD1E + pX ];
			if( !(A & byte_574F)) {
s3B6E:
				A |= byte_574E;
				mDump[ 0xBD1E + pX ] = A;
				mDump[ word_40 ] = A;
				mDump[ 0xBD1D + pX ] = 0x80;

				sub_21C8(7);
			}
		}
	// 3B82
	A = mDump[ 0xBD1B + pX ];
	if( A != 0xFF )
		if( A != mDump[ 0xBD1A + pX ] )
			sub_526F(A);

	mDump[ 0xBD1A + pX ] = A;
	mDump[ 0xBD1B + pX ] = 0xFF;
	sub_5F6B( pX );
	
	}

	//3B9C
	A = mDump[ word_3C ] + mDump[ 0xBD1C + pX ];
	byte byte_3F13 = A;

	mDump[ 0xBD1C + pX ] = 0xFF;
	A = byte_3F13;
	if(!A) {
		mDump[ 0xBD1D + pX ] = 0x80;
		goto s3CB4;
	} else {
		byte_3F0A = 0;
		
		// 3BBF
		for(char Y = 6; Y >= 0;) {
			if( !(mDump[ 0x2F82 + Y ] & byte_3F13 )) {
				++byte_3F0A;
				byte_3F0B = Y;
			}

			Y -= 2;
		}
	
		// 3BD1
		if( byte_3F0A == 1 ) {
			mDump[ 0xBD1D + pX ] = byte_3F0B;
			goto s3CB4;
		}
		if( byte_3F0A == 2 ) {
			byte Y = (byte_3F0B - 4) & 7;
			if( mDump[ 0x2F82 + Y ] & byte_3F13 ) {
				Y = mDump[ 0xBD1D + pX ];
				if( !(Y & 0x80 ))
					if( mDump[ 0x2F82 + Y ] & byte_3F13 )
						goto s3CB4;
			}
		}
		// 3C06
		byte Y = 3;
		while(Y >= 0) {
			mDump[ 0x3F0C + Y ] = 0xFF;
			--Y;
		}

		byte_3F0A = 1;

		// 3C15
		for(;;) {
			byte Y = byte_3F0A;
			if( mDump[ 0x780D + Y ] == 0 ) {
				
				Y = mDump[ 0x34D1 + Y ];
				A = mDump[ 0xBD01 + Y ];
				A -= mDump[ 0xBD01 + pX ];
				//3C2A
				if( A < 0 ) {
					A ^= 0xFF;
					++A;
					Y = 3;
				} else {
					Y = 1;
				}
				if( A < mDump[ 0x3F0C + Y ] )
					mDump[ 0x3F0C + Y ] = A;

				Y = byte_3F0A;
				Y = mDump[ 0x34D1 + Y ];
				A = mDump[ 0xBD02 + Y ];
				A -= mDump[ 0xBD02 + pX ];
				if( A < 0 ) {
					A ^= 0xFF;
					++A;
					Y = 0;
				} else {
					Y = 2;
				}
				if( A < mDump[ 0x3F0C + Y ] )
					mDump[ 0x3F0C + Y ] = A;
			}
			// 3C62
			--byte_3F0A;
			if( byte_3F0A < 0 )
				break;
		}

		// 3C67
		byte_3F10 = 0xFF;
		for(;;) {
			byte_3F11 = 0x00;
			byte_3F12 = 0xFF;
			
			for( char Y = 3; Y >= 0; --Y ) {
				A = mDump[ 0x3F0C + Y ];
				if( A >= byte_3F10 )
					continue;
				if( A < byte_3F11 )
					continue;

				byte_3F11 = A;
				byte_3F12 = Y;
			}
		
			//3C8E
			if( byte_3F12 == 0xFF ) {
				mDump[ 0xBD1D + pX ] = 0x80;
				goto s3CB4;
			}
			
			Y = A << 1;
			mDump[ 0x2F82 + Y ] = A;
			if( A & byte_3F13 )
				break;

			byte_3F10 = byte_3F11;
		}

		// 3CB0
		mDump[ 0xBD1D + pX ] = Y;
	}

	// 3CB4
s3CB4:;
	if( mDump[ 0xBD1D + pX ] & 2 ) {
		mDump[ 0xBD02 + pX ] -= mDump[ 0xBD02 + pX ];

		++mDump[ 0xBD03 + pX ];
		if( mDump[ 0xBD1D + pX ] != 2 ) {
			// 3ccf
			--mDump[ 0xBD01 + pX ];
			if( mDump[ 0xBD03 + pX ] >= 0x87 )
				if( mDump[ 0xBD03 + pX ] >= 0x8A )
					goto s3D4C;
			
			mDump[ 0xBD03 + pX ] = 0x87;
			
		} else {
			// 3ce5
			++mDump[ 0xBD01 + pX ];
			if( mDump[ 0xBD03 + pX ] >= 0x84 )
				if( mDump[ 0xBD03 + pX ] >= 0x87 )
					goto s3D4C;

			mDump[ 0xBD03 + pX ] = 0x84;

			goto s3D4C;
		}
	} else {
		// 3CFB
		A = mDump[ 0xBD1D + pX ];
		if(A < 0)
			goto s3D4F;

		mDump[ 0xBD01 + pX ] -= byte_5FD7;
		++mDump[ 0xBD01 + pX ];
		if( !(mDump[ word_3C ] & 1) ) {
			// 3d15
			mDump[ 0xBD03 + pX ] = 0x8A;
			mDump[ 0xBD02 + pX ] += 2;
			goto s3D4C;
		} else {
			// 3d26
			if( !(mDump[ 0xBD1D + pX ]) )
				mDump[ 0xBD02 + pX ] -= 2;
			else
				mDump[ 0xBD02 + pX ] += 2;

			// 3d40
			mDump[ 0xBD03 + pX ] = ((mDump[ 0xBD02 + pX ] & 0x06) >> 1) + 0x8B;
		}
	}
s3D4C:;
	// 3D4C
	hw_SpritePrepare( pX );

s3D4F:;
	mDump[ word_40 + 6 ] = mDump[ 0xBD1D + pX ];
	mDump[ word_40 + 3 ] = mDump[ 0xBD01 + pX ];
	mDump[ word_40 + 4 ] = mDump[ 0xBD02 + pX ];
	mDump[ word_40 + 5 ] = mDump[ 0xBD03 + pX ];
}

void cCreep::sub_3E87() {
	if( mDump[ word_3E ] & byte_574D )
		return;

	byte X;
	
	sub_3F14(X);
	mDump[ 0xBD00 + X ] = 5;
	mDump[ 0xBD1F + X ] = byte_574A;
	mDump[ 0xBD1E + X ] = mDump[ word_3E ];
	if( !(mDump[ word_3E ] & byte_574E) ) {
		mDump[ 0xBD01 + X ] = mDump[ word_3E + 1 ];
		mDump[ 0xBD02 + X ] = mDump[ word_3E + 2 ] + 7;
		mDump[ 0xBD03 + X ] = 0x8F;
	} else {
		// 3EC8
		mDump[ 0xBD01 + X ] = mDump[ word_3E + 3 ];
		mDump[ 0xBD02 + X ] = mDump[ word_3E + 4 ];
		mDump[ 0xBD03 + X ] = mDump[ word_3E + 5 ];
		mDump[ 0xBD1D + X ] = mDump[ word_3E + 6 ];
	}
	// 3ee4
	mDump[ 0xBD0C + X ] = 3;
	mDump[ 0xBD0D + X ] = 0x11;
	hw_SpritePrepare(X);
	mDump[ 0xBD1C + X ] = 0xFF;
	mDump[ 0xBD1A + X ] = 0xFF;
	mDump[ 0xBD1B + X ] = 0xFF;
	mDump[ 0xBD06 + X ] = 2;
	mDump[ 0xBD05 + X ] = 2;
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
				/*case 0x3FD5:
					//sub_3FD5( X );
					break;
				case 0x4075:
					//sub_4075( X );
					break;
				case 0x41D8:
					//sub_41D8( X );
					break;*/
				case 0x42AD:	// Lightning
					obj_ExecLightningMachine( X );
					break;
				/*case 0x44E7:
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
					break;*/
				case 0x4B1A:		// RayGun Control
					obj_ExecRayGun( X );
					break;
				/*case 0x4D70:
					break;
				case 0x4E32:
					break;
				case 0x4EA8:
					break;
				case 0x50D2:
					break;*/
				case 0x538B:		// Conveyor
					obj_ExecConveyor( X );
					break;
				/*case 0x548B:
					break;
				case 0x5611:
					break;*/
				default:
					printf("aaa");
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

void cCreep::obj_ExecLightning( byte pX ) {
	byte A = mDump[ 0xBD04 + pX ];
	if( A & byte_885 ) {
		mDump[ 0xBD04 + pX ] = (A ^ byte_885) | byte_886;
		return;
	}

	if( A & byte_882 ) {
		A ^= byte_882;
		mDump[ 0xBD04 + pX ] = A;
	}

	A = sub_5ED5();
	A &= 0x03;
	mDump[ 0xBD06 + pX ] = A;
	++mDump[ 0xBD06 + pX ];

	A = sub_5ED5();
	A &= 3;

	A += 0x39;
	if( A == mDump[ 0xBD03 + pX ] ) {
		A += 0x01;
		if( A >= 0x3D )
			A = 0x39;
	}
	// 3679
	mDump[ 0xBD03 + pX ] = A;
	hw_SpritePrepare( pX );
}

void cCreep::sub_368A( byte &pY  ) {
	byte X;

	sub_3F14( X );
	mDump[ 0xBD00 + X ] = 1;
	
	mDump[ 0xBD02 + X ] = mDump[ 0xBF02 + pY ] + 8;
	mDump[ 0xBD01 + X ] = mDump[ 0xBF01 + pY ];
	mDump[ 0xBD1F + X ] = mDump[ 0xBE00 + pY ];

}

// 36B3: Forcefield
void cCreep::obj_ExecForcefield( byte pX ) {
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

	if( mDump[ 0x4750 + Y ] == 1 ) {

		if( mDump[ 0xBD1E + pX ] != 1 ) {
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
		if( mDump[ 0xBD1E + pX ] != 1 )
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

	// Draw the forcefield
	hw_SpritePrepare( pX );
}

// 5FD9
word cCreep::lvlPtrCalculate( byte pCount ) {
	word_42 = pCount;
	
	word_42 <<= 1;
	word_42 <<= 1;
	word_42 <<= 1;
	word_42 |= (0x79 << 8);
	if( mMenuIntro )
		word_42 += (0x20 << 8);

	return word_42;
}

// 0D71: 
void cCreep::Game() {
	if( byte_24FD == 1 ) {
		// D7D
		byte_24FD = 0;
		mDump[ 0x7802 ] |= 1;
	} else {
		// D8D
		word_30 = 0x9800;
		word_32 = 0x7800;

		word_34 = mDump[ 0x9800 ];
		word_34 |= (mDump[ 0x9801 ] << 8);
		
		// Hope this is correct
		memcpy( &mDump[ word_32 ], &mDump[ word_30 ], word_34 );

		// DC6
		mDump[ 0x7812 ] = byte_D10;

		for( char Y = 7; Y >= 0; --Y )
			mDump[ 0x7855 + Y ] = 0;

		mDump[ 0x785D ] = mDump[ 0x785E ] = 0;
		mDump[ 0x7809 ] = mDump[ 0x7803 ];
		mDump[ 0x780A ] = mDump[ 0x7804 ];
		mDump[ 0x780B ] = mDump[ 0x7805 ];
		mDump[ 0x780C ] = mDump[ 0x7806 ];
		mDump[ 0x7811 ] = 0;
		mDump[ 0x780F ] = 1;

		// E00
		if( mDump[ 0x7812 ] != 1 ) {
			mDump[ 0x7810 ] = 0;
			mDump[ 0x780E ] = 4;
		} else
			mDump[ 0x7810 ] = 1;
	}

	// E19
	for(;;) {
		if( mDump[ 0x780F ] != 1 )
			if( mDump[ 0x7810 ] != 1 )
				break;
		// E2A
		if( mDump[ 0x780F ] == 1 && mDump[ 0x7810 ] == 1 ) {
			if( mDump[ 0x7809 ] == mDump[ 0x780A ] ) {
				byte X = mDump[ 0x7811 ];

				mDump[ 0x11C9 + X ] = 0;
				X ^= 0x01;
				mDump[ 0x11C9 + X ] = 1;

			} else {
				mDump[ 0x11C9 ] = mDump[ 0x11CA ] = 1;
			}

		} else {
			// E5F
			if( mDump[ 0x780F ] != 1 ) {
				mDump[ 0x11CA ] = 1;
				mDump[ 0x11C9 ] = 0;
			} else {
				// E73
				mDump[ 0x11C9 ] = 1;
				mDump[ 0x11CA ] = 0;
			}
		}

		// E7D
		MapDisplay();
		GameMain();
		ScreenClear();
		
		// E8D
	}
	// F5B
}

// F94: Display the map/time taken screen
void cCreep::MapDisplay() {
	byte gfxPosX, gfxPosY;

sF99:;
	ScreenClear();

	mDump[ 0xD028 ] = mDump[ 0xD027 ] = 0;
	mDump[ 0x11D7 ] = 0;

	// Draw both players Name/Time/Arrows
	// FA9
	for(;; ) {
		byte X = mDump[ 0x11D7 ];

		if( mDump[ 0x11C9 + X ] == 1 ) {
			// FB6

			char A = mDump[ 0x7809 + X ];
			lvlPtrCalculate( A );
			
			mDump[ word_42 ] |= byte_8C0;
			
			sub_6009( mDump[ 0x780B + X ] );
			
			mDump[ 0x11D9 ] = mDump[ word_40 + 2 ] & 3;
			
			sub_3F14( X );
			mDump[ 0x11D8 ] = X >> 5;
			
			word posX;
			
			A = mDump[ word_42 + 1 ];
			A += mDump[ word_40 + 5 ];

			byte Y = mDump[ 0x11D9 ];
			A += mDump[ 0x11DA + Y ];
			posX = A;

			bool cf = false;

			if( (A - 16) < 0 )
				cf = true;
			A -= 16;
			A <<= 1;

			posX -= 16;
			posX <<= 1;
			
			Y = mDump[ 0x11D8 ];
			cSprite *sprite = mScreen->spriteGet( Y );

			// Sprite X
			mDump[ 0x10 + Y ] = A;
			sprite->_X = posX;

			if( cf ) {
				A = mDump[ 0x2F82 + Y ] | mDump[ 0x20 ];
			} else
				A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0x20 ];

			// Sprite X 8bit
			mDump[ 0x20 ] = A;

			// 100D
			A = mDump[ word_42 + 2 ];
			A += mDump[ word_40 + 6 ];
			A += mDump[ 0x11DE + mDump[ 0x11D9 ] ];
			//A += 0x32;

			// Sprite Y
			sprite->_Y = A;
			mDump[ 0x18 + mDump[ 0x11D8 ] ] = A;
			mDump[ 0xBD03 + X ] = mDump[ 0x11E2 + Y ];
			
			// Enable the Arrow sprite
			hw_SpritePrepare( X );
			
			// Sprites Enabled
			mDump[ 0x21 ] = (mDump[ 0x2F82 + mDump[ 0x11D8 ] ] | mDump[ 0x21 ]);
			sprite->_rEnabled = true;

			// 103C
			X = mDump[ 0x7807 + mDump[ 0x11D7 ] ];
			A = mDump[ 0x11E5 + X ];

			mDump[ 0x11EF ] = mDump[ 0x11FA ] = A;
			X = Y << 1;
			
			word_3E = mDump[ 0x11E9 + X ];
			word_3E += mDump[ 0x11EA + X ] << 8;

			// Player (One Up / Two Up)
			DisplayText();

			// 1058
			word_3E = (Y << 1);
			word_3E += 0x7855;

			sub_29AE();

			gfxPosX = mDump[ 0x11D3 + Y ] + 8;
			gfxPosY = 0x10;

			// Draw Time ' : : '
			drawGraphics(0, 0x93, gfxPosX, gfxPosY, 0);
		} 
		
		// 1087
		++mDump[ 0x11D7 ];
		if( mDump[ 0x11D7 ] == 2 )
			break;
	}
	
	// 1094
	mapRoomDraw();

	mDump[ 0x11CB ] = 0;
	mDump[ 0x11CD ] = mDump[ 0x11CC ] = 1;
	if( mDump[ 0x11C9 ] != 1 ) {
		if( mDump[ 0x11CA ] == 1 )
			mDump[ 0x11CD ] = 0;

	} else {
		// 10BA
		mDump[ 0x11CC ] = 0;
		if( mDump[ 0x11CA ] == 1 ) {
			mDump[ 0x11CD ] = 0;
			if( mDump[ 0x780B ] == mDump[ 0x780C ] ) {
				mDump[ 0x11CB ] = 1;
				mDump[ 0xD028 ] = mDump[ 0xD027 ] = 1;
				goto s10EB;
			}
		}
	}

	// 10E3
	mDump[ 0xD027 ] = mDump[ 0xD028 ] = 0;

s10EB:;
	mDump[ 0x11D0 ] = 1;
	byte_B83 = 0;
	mDump[ 0x11CE ] = mDump[ 0x11D1 ];
	mDump[ 0x11CF ] = mDump[ 0x11D2 ];
	mDump[ 0x11D7 ] = 0;
	

	// 110E
	for( ;; ) {
		byte_2E35 = 1;

		byte X = mDump[ 0x11D7 ];

		// Player Arrow Flash
		if( mDump[ 0x11CB ] != 1 ) {
			--mDump[ 0x11CE + X ];
			if(mDump[ 0x11CE + X ] == 0 ) {
				// 1122
				mDump[ 0x11CE + X ] = mDump[ 0x11D1 + X ];
				if( X == 0 || byte_11C9 != 1 ) {
					// 1133
					mDump[ 0xD027 ] ^= 0x01;
					mScreen->spriteGet( 0 )->_color ^= 0x01;
				} else {
					// 113E
					mDump[ 0xD028 ] ^= 0x01;
					mScreen->spriteGet( 1 )->_color ^= 0x01;
				}
			}
		}

		// 1146
		if( byte_B83 == 1 ) {
			byte_B83 = 0;
			mDump[ 0x11D0 ] = 0;
			return;
		}
		// 115A
		
		KeyboardJoystickMonitor( X );
		if( byte_5F6A == 1 ) {
			byte_2E02 = 1;
			// 116A
			// TODO: Figure this out, usually a BRK instruction is
			// Executed inside the function
			//sub_2C08();
			goto sF99;
		}
		if( RunRestorePressed == 1 ) {
			SavePosition();
			goto sF99;
		}
		// 117D
		if( byte_5F57 )
			mDump[ 0x11CC + X ] = 1;

		if( mDump[ 0x11CC ] == 1 )
			if( mDump[ 0x11CD ] == 1 )
				break;
		
		// 1195
		mDump[ 0x11D7 ] ^= 0x01;
		// TODO: Wait for vic interrupts


		mScreen->bitmapLoad( &mDump[ 0xE000 ], &mDump[ 0xCC00 ], &mDump[ 0xD800 ], 0 );
		mScreen->spriteDraw();
		
		mScreen->refresh();
	}
	// 11A5

	for(;;) {
		KeyboardJoystickMonitor(0);
		if( byte_5F57 )
			continue;

		KeyboardJoystickMonitor(1);
		if( byte_5F57 )
			continue;
		
		break;
	}

	sub_21C8( 9 );
	mDump[ 0x11D0 ] = 0;
}

void cCreep::sub_34EF( byte pX, byte pY ) {
	byte A;
	if( mDump[ 0xBF00 + pY ] == 0x0B ) {
		
		A = mDump[ 0xBD01 + pX ] + mDump[ 0xBD0C + pX ];
		A -= mDump[ 0xBF01 + pY ];

		if( A < 4 ) {
			mDump[ 0x780D + mDump[ 0xBD1C + pX ] ] = 2;
			return;
		}

	} 
	//3505
	byte_31F5 = 0;
	if( mDump[ 0xBF00 + pY ] != 0x0C ) 
		return;

	A = mDump[ 0xBD01 + pX ] + mDump[ 0xBD0C + pX ];
	A -= mDump[ 0xBF01 + pY ];

	if( A >= 4 )
		return;

	mDump[ 0xBD1A + pX ] = mDump[ 0xBE00 + pY ];
	return;
}

void cCreep::sub_359E( ) {
	byte X;
	sub_3F14( X );
	
	byte Y = byte_3638;
	mDump[ 0x34D1 + Y ] = X;
	
	byte A = mDump[ 0x780B + Y ];
	A <<= 3;
	word_40 = A + word_41D3;
	
	// 35C0
	if( mDump[ word_40 + 2 ] & 0x80 ) {
		mDump[ 0x780D + byte_3638 ] = 6;
		mDump[ 0xBD01 + X ] = mDump[ word_40 ] + 0x0B;
		mDump[ 0xBD02 + X ] = mDump[ word_40 + 1 ] + 0x0C;
		mDump[ 0xBD1B + X ] = 0x18;
		mDump[ 0xBD06 + X ] = 0x03;

	} else {
		// 35F1
		mDump[ 0x780D + byte_3638 ] = 0;
		mDump[ 0xBD01 + X ] = mDump[ word_40 ] + 6;
		mDump[ 0xBD02 + X ] = mDump[ word_40 + 1 ] + 0x0F;
	}

	// 360D
	mDump[ 0xBD0C + X ] = 3;
	mDump[ 0xBD0D + X ] = 0x11;
	mDump[ 0xBD1F + X ] = 0x80;
	mDump[ 0xBD1C + X ] = byte_3638;
	mDump[ 0xBD03 + X ] = 0;
	mDump[ 0xBD19 + X ] = mDump[ 0xBD1A + X ] = mDump[ 0xBD18 + X ] = 0xFF;
}

void cCreep::GameMain() {

	roomLoad();

	byte X = 0;

	for(;;) {
		
		if( mDump[ 0x11C9 + X ] == 1 ) {
			byte_3638 = X;
			sub_359E();
		}
		++X;
		if(X >= 2)
			break;
	}

	//14EA
	mDump[ 0x15D7 ] = 1;
	byte_B83 = 0;

	for(;;) {
		mScreen->refresh();

		handleEvents();
		if( byte_5F6A == 1 ) {
			byte_2E02 = 0;
			//sub_2C08;
			// TODO: Figure out what this does
		}
		
		if( RunRestorePressed == 1 ) {
			byte_2E35 = 3;
			
			KeyboardJoystickMonitor( 0 );
			// TODO: 150E

		}
		// 156B
		if( byte_B83 == 1 ) {
			byte_B83 = 0;
			char X = 1;
			for(;;) {
				if( mDump[ 0x780D + X ] == 0 ) {
					mDump[ 0x780D + X ] = 2;
					byte Y = mDump[ 0x34D1 + X ];
					mDump[ 0xBD04 + Y ] |= byte_883;
				}
				--X;
				if(X < 0)
					break;
			}
		}
		// 1594
		if( mDump[ 0x780D ] == 0 ) {
			mDump[ 0x7811 ] = 0;
			continue;
		}
		// 15A3
		if( mDump[ 0x780E ] != 0 ) {
			mDump[ 0x7811 ] = 1;
			continue;
		}
		
		byte X = 0;
s15B4:;
	if( mDump[ 0x780D + X ] == 5 )
		continue;
	if( mDump[ 0x780D + X ] == 6 )
		continue;

	++X;
	if( X < 2 )
		goto s15B4;

		break;
	}

	// 15C4
	mDump[ 0x15D7] = 0;

	for( byte X = 0x1E; X; --X ) 
		handleEvents();
}

void cCreep::sub_6009( byte pA ) {
	byte_603A = pA;

	word_40 = *((word*) &mDump[ word_42 + 4]);
	
	byte A2 = mDump[ word_40 ];
	
	word_40 += (byte_603A << 3) + 1;

	byte_603A = A2;	
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
		word_30 += (mDump[ word_38 + 1 ] << 8);
		
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

				if( gfxPosBottomY >= 0xDC && mTxtPosLowerY < 0xDC ) {
					gfxBottomY = mTxtPosLowerY;
				}  else
					gfxBottomY = gfxPosBottomY;

			} else {

				// 5A49
				if( mTxtPosLowerY >= 0xDC && gfxPosBottomY < 0xDC ) {
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

				word word_36 = (byte_34 + mTxtDestX) + (mTxtEdgeScreenX << 8);


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

// 1203: 
void cCreep::mapRoomDraw() {
	byte byte_13EA, byte_13EB, byte_13EC;
	byte byte_13ED, byte_13EE, byte_13EF;

	word_42 = 0x7900;
	
	byte gfxPosX;
	byte gfxPosY;

	//1210
	for(;;) {
		
		if( mDump[ word_42 ] & byte_8C1 )
			return;
		
		if( mDump[ word_42 ] & byte_8C0 ) {
			//1224
			
			mDump[ 0x63E7 ] = mDump[ word_42 ] & 0xF;
			byte_13EC = mDump[ word_42 + 1 ];
			byte_13ED = mDump[ word_42 + 2 ];
			byte_13EF = mDump[ word_42 + 3 ] & 7;
			byte_13EE = (mDump[ word_42 + 3 ] >> 3) & 7;

			gfxPosY = byte_13ED;

			byte_13EB = byte_13EF;
			
			// Draw Room Floor Square
			// 1260
			for(;;) {
				byte_13EA = byte_13EE;
				gfxPosX = byte_13EC;
				
				for(;;) {
					drawGraphics( 0, 0x0A, gfxPosX, gfxPosY, 0 );
					gfxPosX += 0x04;
					--byte_13EA;
					if(!byte_13EA)
						break;
				}
				gfxPosY += 0x08;
				--byte_13EB;
				if(!byte_13EB)
					break;
			}

			// 128B
			// Top edge of room
			mTxtX_0 = byte_13EC;
			mTxtY_0 = byte_13ED;
			byte_13EA = byte_13EE;

			for(;;) {
				drawGraphics(1, 0, 0, 0, 0x0B );
				mTxtX_0 += 0x04;
				--byte_13EA;
				if(!byte_13EA)
					break;
			}

			// 12B8
			// Bottom edge of room
			mTxtX_0 = byte_13EC;
			mTxtY_0 = ((byte_13EF << 3) + byte_13ED) - 3;
			byte_13EA = byte_13EE;

			for(;;) {
				drawGraphics(1, 0, 0, 0, 0x0B );
				mTxtX_0 += 0x04;
				--byte_13EA;
				if(!byte_13EA)
					break;
			}

			//12E5
			// Draw Left Edge
			mTxtX_0 = byte_13EC;
			mTxtY_0 = byte_13ED;
			byte_13EA = byte_13EF;

			for(;;) {
				drawGraphics(1, 0, 0, 0, 0x0C );
				mTxtY_0 += 0x08;
				--byte_13EA;
				if(!byte_13EA)
					break;
			}

			//130D
			// Draw Right Edge
			mTxtX_0 = ((byte_13EE << 2) + byte_13EC) - 4;
			mTxtY_0 = byte_13ED;
			byte_13EA = byte_13EF;

			for(;;) {
				drawGraphics(1, 0, 0, 0, 0x0D );
				mTxtY_0 += 0x08;
				--byte_13EA;
				if(!byte_13EA)
					break;
			}

			// 133E
			sub_6009( 0 );
			byte_13EA = byte_603A;
s1349:;
			if( byte_13EA )
				break;
		} 

		//134E
		word_42 += 0x08;
	}

	//135C
	byte A = mDump[ word_40 + 2 ];

	if( !( A & 3) ) {
		mTxtY_0 = byte_13ED;
	} else {
		// 136D
		if( A == 2 ) {
			mTxtY_0 = (byte_13EF << 3) + byte_13ED;
			mTxtY_0 -= 3;
		} else {
			// 13A0
			mTxtY_0 = byte_13ED + mDump[ word_40 + 6 ];
			
			if( A != 3 ) {
				mTxtX_0 = ((byte_13EE << 2) + byte_13EC) - 4;
				A = 0x11;
			} else {
				// 13C5
				mTxtX_0 = byte_13EC;
				A = 0x10;
			}
		}
		goto s13CD;
	}

	// 1381
	mTxtX_0 = byte_13EC + mDump[ word_40 + 5 ];
	if( mTxtX_0 & 2 ) {
		mTxtX_0 ^= mTxtX_0;
		A = 0x0F;
	} else 
		A = 0x0E;
		
	// Draw Doors in sides
s13CD:;
	drawGraphics( 1, 0, 0, 0, A );

	word_40 += 0x08;
	--byte_13EA;
	goto s1349;

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

void cCreep::sub_21C8( char pA ) {
	char byte_2231 = pA;

	if( mMenuIntro == 1 )
		return;

	if( byte_839 == 1 )
		return;

	if( byte_2232 < 0 )
		return;

	byte_2232 = byte_2231;

	byte Y = byte_2232 << 1;
	word_44 = *((word*) &mDump[ 0x7572 + Y ]);

	mDump[ 0xD404 ] = mDump[ 0xD40B ] = mDump[ 0xD412 ] = mDump[ 0xD417 ] = 0;
	mDump[ 0x20DC ] = mDump[ 0x20DD ] = 0;
	mDump[ 0xD418 ] = 0x0F;
	mDump[ 0x2104 ] = 0x18;
	mDump[ 0x2105 ] = 0x18;
	mDump[ 0x2106 ] = 0x18;
	mDump[ 0x2107 ] = 0x14;
	mDump[ 0xDC05 ] = (mDump[ 0x2107 ] << 2 ) | 3;
	mDump[ 0xDC0D ] = 0x81;
	mDump[ 0xDC0E ] = 0x01;
}

void cCreep::obj_PrepWalkway() {
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

byte cCreep::sub_5ED5() {
	static byte byte_5EFB = 0x57, byte_5EFA = 0xC6, byte_5EF9 = 0xA0;
	
	byte A = byte_5EFB;
	bool ocf = false, cf = false;

	if( A & 0x01 )
		cf = true;
	
	A >>= 1;
	if(ocf)
		A |= 0x80;
	
	ocf = cf;
	cf = false;
	A = byte_5EFA;
	if( A & 0x01 )
		cf = true;
	A >>= 1;
	if(ocf)
		A |= 0x80;

	byte_5EF9 = A;
	A = 0;
	if(cf)
		A = 0x01;

	A ^= byte_5EFA;
	byte_5EFA = A;

	A = byte_5EF9;
	A ^= byte_5EFB;
	byte_5EFB = A;
	
	A ^= byte_5EFA;
	byte_5EFA = A;

	return A;
}

// 5D26: Prepare sprites
void cCreep::hw_SpritePrepare( byte &pX ) {
	byte byte_5E8C, byte_5E8D, byte_5E88;
	byte A;

	word word_38 = *((word*) &mDump[ 0xBD03 + pX ]);
	word_38 <<= 1;

	word_38 += 0x603B;
	
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
	
	cSprite *sprite = mScreen->spriteGet(Y);

	word dataSrc = mDump[ 0x26 + Y ] ^ 8;
	dataSrc <<= 6;
	dataSrc += 0xC000;

	mDump[ 0x26 + Y ] = mDump[ 0x26 + Y ] ^ 8;

	// Sprite Color
	mDump[ 0xD027 + Y ]  = mDump[ 0xBD09 + pX ] & 0x0F;
	sprite->_color = mDump[ 0xD027 + Y ];

	if( !(mDump[ 0xBD09 + pX ] & byte_88A )) {
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD01D ];
		sprite->_rDoubleWidth = false;
	} else {
		A = (mDump[ 0xD01D ] | mDump[ 0x2F82 + Y ]);
		mDump[ 0xBD0A + pX ] <<= 1;
		sprite->_rDoubleWidth = true;
	}

	// Sprite X Expansion
	mDump[ 0xD01D ] = A;
	
	// 5E2D
	if( !(mDump[ 0xBD09 + pX ] & byte_88B )) {
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD017 ];
		sprite->_rDoubleHeight = false;
	} else {
		A = (mDump[ 0xD017 ] | mDump[ 0x2F82 + Y ]);
		mDump[ 0xBD0B + pX ] <<= 1;
		sprite->_rDoubleHeight = true;
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
	if(! (mDump[ 0xBD09 + pX ] & byte_88D )) {
		A = mDump[ 0xD01C ] | mDump[ 0x2F82 + Y ];
		sprite->_rMultiColored = true;
	} else {
		A = (mDump[ 0x2F82 + Y ] ^ 0xFF) & mDump[ 0xD01C ];
		sprite->_rMultiColored = false;
	}
		
	// MultiColor Enable
	mDump[ 0xD01C ] = A;

	sprite->streamLoad( &mDump[ dataSrc ] );
}

// 24FF
void cCreep::SavePosition() {
	// TODO
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

void cCreep::obj_PrepSlidingPole() {
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
			if (mDump[ word_3C ]  & 0x44) {
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

void cCreep::obj_PrepLadder() {
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

// 4075: Pole Sliding
void cCreep::sub_4075( byte pX, byte pY ) {
	byte byte_41D5 = pY;
	if( mDump[ 0xBE01 + pY ] == 0 )
		return;
	if( mDump[ 0xBD00 + pX ] )
		return;

	// 4085
	if( mDump[ 0xBD1E + pX ] != 1 )
		return;
	
	pY = mDump[ 0xBD1C + pX ];

	byte A = mDump[ 0x780D + pY ];
	if( A != 0 )
		return;

	mDump[ 0x780D + pY ] = 6;
	mDump[ 0xBD1B + pX ] = 0;
	mDump[ 0xBD06 + pX ] = 3;
	
	A = mDump[ 0xBE00 + byte_41D5 ];
	A <<= 3;

	word_40 = word_41D3 + A;

	// 40BB

	mDump[ 0xBD02 + pX ] = mDump[ word_40 + 1 ] + 0xF;
	mDump[ 0xBD01 + pX ] = mDump[ word_40 ] + 0x06;
	if( mDump[ word_40 + 7 ] != 0 ) {
		mDump[ 0x785D + mDump[ 0xBD1C + pX ] ] = 1;
	}

	//40DD
	word word_41D6 = *((word*) &mDump[ word_40 + 3 ]);
	lvlPtrCalculate( word_41D6 );
	
	mDump[ word_42 ] |= byte_8C0;

	pY = mDump[ 0xBD1C + pX ];
	mDump[ 0x7809 + pY ] = word_41D6 & 0xFF;
	mDump[ 0x780B + pY ] = (word_41D6 & 0xFF00) >> 8;
}

// 4F5C: Load the rooms' Teleports
void cCreep::obj_PrepTeleport() {
	mTxtX_0 = mDump[ word_3E ];
	mTxtY_0 = mDump[ word_3E + 1 ];

	byte byte_50D0 = 3;
	for(;;) {
		drawGraphics( 1, 0, 0, 0, 0x1C );
		
		mTxtX_0 += 0x04;
		--byte_50D0;
		if( byte_50D0 == 0 )
			break;
	}

	byte gfxPosX = mDump[ word_3E ];
	byte gfxPosY = mDump[ word_3E + 1 ];

	// Draw the teleport unit
	drawGraphics( 0, 0x6F, gfxPosX, gfxPosY, 0 );

	//4fad
	gfxPosX += 0x0C;
	gfxPosY += 0x18;
	drawGraphics( 0, 0x1C, gfxPosX, gfxPosY, 0 );

	byte X;
	sub_5750( X );
	
	mDump[ 0xBF00 + X ] = 0x0A;
	gfxPosX = mDump[ word_3E ] + 4;
	gfxPosY = mDump[ word_3E + 1 ] + 0x18;

	mDump[ 0xBE00 + X ] = (word_3E & 0xFF);
	mDump[ 0xBE01 + X ] = (word_3E & 0xFF00) >> 8;
	
	SpriteMovement( 0x70, gfxPosX, gfxPosY, 0, X );

	sub_505C( (mDump[ word_3E + 2 ] + 2), X );
	
	byte_50D0 = 0x20;

	while( mDump[ word_3E + 3 ] ) {
		byte A = byte_50D0;
		mDump[ 0x6E95 ] = mDump[ 0x6E96 ] = mDump[ 0x6E97 ] = mDump[ 0x6E98 ] = A;
		gfxPosX = mDump[ word_3E + 3 ];
		gfxPosY = mDump[ word_3E + 4 ];

		drawGraphics( 0, 0x72, gfxPosX, gfxPosY, 0 );
		
		word_3E += 2;

		byte_50D0 += 0x10;
	}

	word_3E += 0x04;
}

// 4B1A: 
void cCreep::obj_ExecRayGun( byte pX ) {
	if( byte_2E36 & 3 )
		return;

	word_40 = word_4D5B + mDump[ 0xBE00 + pX ];

	byte A = mDump[ 0xBF04 + pX ];
	if(!( A & byte_83F )) {
		if( mMenuIntro == 1 )
			return;

		// 4B46
		if(!(mDump[ word_40 ] & byte_4D62) ) {
			byte_4D5D = 0xFF;
			byte_4D5E = 0x00;
			byte_4D5F = 0x01;

			for(;;) {

				if( mDump[ 0x780D + byte_4D5F ] == 0 ) {
					byte Y = mDump[ 0x34D1 + byte_4D5F ];
					char A = mDump[ 0xBD02 + Y ];
					A -= mDump[ 0xBF02 + pX ];
					if( A < 0 )
						A = (A ^ 0xFF) + 1;

					if( A < byte_4D5D ) {
						byte_4D5D = A;
						if( mDump[ 0xBD02 + Y ] >= 0xC8  ) {
							byte_4D5E = byte_4D65;
						} else {
							byte_4D5E = byte_4D66;
						}
					}
				}
				//4B9C
				--byte_4D5F;
				if( byte_4D5F < 0 )
					break;
			}
			// 4BA1
			A = 0xFF;
			A ^= byte_4D65;
			A ^= byte_4D66;

			A &= mDump[ word_40 ];
			A |= byte_4D5E;
			mDump[ word_40 ] = A;
		}
		//4BB2
		if( mDump[ word_40 ] & byte_4D65 ) {
			A = mDump[ word_40 + 4 ];
			if( A != mDump[ word_40 + 2 ] ) {
				A -= 1;
				mDump[ word_40 + 4 ] = A;
				sub_4DE9( 0x5C );
			} else
				goto s4BD9;
		} else {
			// 4BD4
			if( !(A & byte_4D66) ) {
s4BD9:;
				sub_4DE9( 0xCC );
				goto s4C27;
			}
			// 4BE1
			A = mDump[ word_40 + 4 ];
			if( A >= mDump[ 0xBE01 + pX ] )
				goto s4BD9;

			mDump[ word_40 + 4 ] = A + 1;
			sub_4DE9( 0xC2 );
		}	
	}
	// 4BF4
	byte gfxPosX = mDump[ 0xBF01 + pX ];
	byte gfxPosY = mDump[ word_40 + 4 ];

	A = mDump[ word_40 ];
	if( A & byte_4D67 )
		A = 4;
	else
		A = 0;

	byte_4D5E = A;
	byte Y = mDump[ word_40 + 4 ] & 3;
	Y |= byte_4D5E;

	SpriteMovement( mDump[ 0x4D68 + Y ], gfxPosX, gfxPosY, 0, pX );

s4C27:;
	A = mDump[ word_40 ];

	if( A & byte_4D62 ) {
		A ^= byte_4D62;
		mDump[ word_40 ] = A;
		if( !(A & byte_4D64 ))
			return;
	} else {
		// 4C3D
		if( byte_4D5D >= 5 )
			return;
	}
	// 4C44
	A = mDump[ word_40 ];
	if( (A & byte_4D61) )
		return;

	sub_3A7F( pX );
	A |= byte_4D61;
	mDump[ word_40 ] = A;
}

// 4C58: Load the rooms' Ray Guns
void cCreep::obj_PrepRayGun() {

	word_4D5B = word_3E;
	byte_4D5D = 0;

	for(;;) {
		if( mDump[ word_3E ] & byte_4D60 ) {
			++word_3E;
			break;
		}
		// 4C7E

		mDump[ word_3E ] &= 0xFF ^ byte_4D61;
		byte gfxPosX = mDump[ word_3E + 1 ];
		byte gfxPosY = mDump[ word_3E + 2 ];
		byte gfxCurrentID;

		if( mDump[ word_3E ] & byte_4D67 )
			gfxCurrentID = 0x5F;
		else
			gfxCurrentID = 0x60;

		byte_4D5E = mDump[ word_3E + 3 ];
		for(;;) {
			if(!byte_4D5E)
				break;
			
			drawGraphics( 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );
			gfxPosY += 0x08;
			--byte_4D5E;
		}
		// 4CCB
		if(!( mDump[ word_3E ] & byte_4D63 )) {
			byte X;

			sub_5750( X );
			
			mDump[ 0xBF00 + X ] = 8;
			mDump[ 0xBE00 + X ] = byte_4D5D;
			mDump[ 0xBF04 + X ] |= byte_840;
			
			byte A = mDump[ word_3E + 3 ];
			A <<= 3;
			A += mDump[ word_3E + 2 ];
			A -= 0x0B;
			mDump[ 0xBE01 + X ] = A;
			// 4D01
			if( !(mDump[ word_3E ] & byte_4D67) ) {
				A = mDump[ word_3E + 1 ];
				A += 4;
			} else {
				A = mDump[ word_3E + 1 ];
				A -= 8;
			}
			mDump[ 0xBF01 + X ] = A;
		} 
		
		// 4D1A
		byte X;

		sub_5750( X );
		mDump[ 0xBF00 + X ] = 9;
		gfxPosX = mDump[ word_3E + 5 ];
		gfxPosY = mDump[ word_3E + 6 ];

		SpriteMovement( 0x6D, gfxPosX, gfxPosY, 0, X );

		mDump[ 0xBE00 + X ] = byte_4D5D;

		word_3E += 0x07;
		byte_4D5D += 0x07;
	}
}

// 49F8: Load the rooms' Keys
void cCreep::obj_PrepKey() {
	word_4A65 = word_3E;

	byte_4A64 = 0;
	
	for(;;) {
		if( mDump[ word_3E ] == 0 ) {
			++word_3E;
			break;
		}

		if( mDump[ word_3E + 1 ] != 0 ) {
			byte X;
			
			sub_5750(X);
			mDump[ 0xBF00 + X ] = 6;

			byte gfxPosX = mDump[ word_3E + 2 ];
			byte gfxPosY = mDump[ word_3E + 3 ];
			byte gfxCID = mDump[ word_3E + 1 ];

			mDump[ 0xBE00 + X ] = byte_4A64;

			SpriteMovement( gfxCID, gfxPosX, gfxPosY, 0, X );
		}

		// 4A47
		byte_4A64 += 0x04;
		word_3E += 0x04;
	}

}

void cCreep::obj_PrepLock() {
	
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

void cCreep::obj_PrepDoors() {
	byte byte_41D0 = *level(word_3E++);
	word_41D3 = word_3E;
	
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

		lvlPtrCalculate( *level( word_3E + 3 ) );
		
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

// 42AD: Lightning Machine pole movement
void cCreep::obj_ExecLightningMachine( byte pX ) {
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

void cCreep::obj_PrepDoorbell() {
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
void cCreep::obj_PrepLightning() {
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

// 46AE: Forcefield
void cCreep::obj_PrepForcefield() {
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

		// Draw outside of timer
		drawGraphics( 0, 0x3F, gfxPosX, gfxPosY, 0 );

		// 46EA
		gfxPosX += 4;
		gfxPosY += 8;

		for( char Y = 7; Y >= 0; --Y ) 
			mDump[ 0x6889 + Y ] = 0x55;

		// Draw inside of timer
		SpriteMovement( 0x40, gfxPosX, gfxPosY, 0, X );

		mDump[ 0xBE00 + X ] = byte_474F;
		mDump[ 0x4750 + byte_474F ] = 1;

		sub_3757( );

		gfxPosX = mDump[ word_3E + 2 ];
		gfxPosY = mDump[ word_3E + 3 ];

		// Draw top of forcefield
		drawGraphics( 0, 0x3E, gfxPosX, gfxPosY, 0 );
		
		++byte_474F;
		word_3E += 0x04;
	}

}

// 4872 : Load the rooms' Mummys
void cCreep::obj_PrepMummy( ) {
	byte	byte_498D = 0, byte_498E, byte_498F;
	byte	X;
	byte	gfxCurrentID;

	word_498B = word_3E;

	for(;;) {
		
		if( mDump[ word_3E ] == 0 ) {
			++word_3E;
			return;
		}

		sub_5750( X );
		
		mDump[ 0xBF00 + X ] = 5;

		byte gfxPosX = mDump[ word_3E + 1 ];
		byte gfxPosY = mDump[ word_3E + 2 ];
		gfxCurrentID = 0x44;

		mDump[ 0xBE00 + X ] = byte_498D;
		mDump[ 0xBE02 + X ] = 0x66;
		for( char Y = 5; Y >= 0; --Y )
			mDump[ 0x68F0 + Y ] = 0x66;

		SpriteMovement( gfxCurrentID, gfxPosX, gfxPosY, 0, X );
		byte_498E = 3;
		gfxPosY = mDump[ word_3E + 4 ];
		gfxCurrentID = 0x42;

		while(byte_498E) {
			byte_498F = 5;
			gfxPosX = mDump[ word_3E + 3 ];

			while(byte_498F) {
				drawGraphics( 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );
				gfxPosX += 4;
				--byte_498F;
			}

			gfxPosY += 8;
			--byte_498E;
		}

		if( mDump[ word_3E ] != 1 ) {
			// 4911
			mTxtX_0 = mDump[ word_3E + 3 ] + 4;
			mTxtY_0 = mDump[ word_3E + 4 ] + 8;
			byte_498E = 3;

			while( byte_498E ) {
				drawGraphics( 1, gfxCurrentID, gfxPosX, gfxPosY, 0x42 );
				mTxtX_0 += 4;
			}
			
			gfxPosX = mTxtX_0 - 0x0C;
			gfxPosY = mTxtY_0;
			drawGraphics( 0, 0x43, gfxPosX, gfxPosY, 0x42 );
			
			if( mDump[ word_3E ] == 2 ) {
				sub_396A(0xFF, X);
			}
		}
		// 496E
		word_3E += 0x07;
		byte_498D += 0x07;
	}
}

// 517F : Load the rooms' Trapdoors
void cCreep::obj_PrepTrapDoor( ) {
	byte	byte_5381;
	word_5387 = word_3E;

	byte X;

	byte_5381 = 0;
	for(;;) {
	
		if( (mDump[ word_3E ] & byte_5389) ) {
			++word_3E;
			return;
		}
		
		sub_5750( X );
		mDump[ 0xBF00 + X ] = 0x0B;
		mDump[ 0xBE00 + X ] = byte_5381;
		if( !(mDump[ word_3E ] & byte_538A) ) {
			// 51BC
			mDump[ 0x6F2E ] = 0xC0;
			mDump[ 0x6F30 ] = 0x55;
		} else {
			// 51c9
			mTxtX_0 = mDump[ word_3E + 1 ];
			mTxtY_0 = mDump[ word_3E + 2 ];
			drawGraphics( 1, 0, 0, 0, 0x7B );
			
			byte gfxPosX, gfxPosY;

			gfxPosX = mTxtX_0 + 4;
			gfxPosY = mTxtY_0;
			SpriteMovement( 0x79, gfxPosX, gfxPosY, 0x7B, X );
			mDump[ 0x6F2E ] = 0x20;
			mDump[ 0x6F30 ] = 0xCC;
			byte_5FD5 = mDump[ word_3E + 1 ] >> 2;
			byte_5FD5 -= 4;
			
			byte_5FD6 = mDump[ word_3E + 2 ] >> 3;
			sub_5FA3();

			mDump[ word_3C ] = mDump[ word_3C ] & 0xFB;
			mDump[ word_3C + 4 ] = mDump[ word_3C + 4 ] & 0xBF;
		}

		// 522E
		sub_5750( X );
		mDump[ 0xBF00 + X ] = 0x0C;
		
		byte gfxPosX = mDump[ word_3E + 3 ];
		byte gfxPosY = mDump[ word_3E + 4 ];
		
		mDump[ 0xBE00 + X ] = byte_5381;
		SpriteMovement( 0x7A, gfxPosX, gfxPosY, 0, X );
		
		byte_5381 += 0x05;
		word_3E += 0x05;
	}

}

// 538B: Conveyor
void cCreep::obj_ExecConveyor( byte pX ) {
	
	word_40 = word_564B + mDump[ 0xBE00 + pX ];
	byte A = mDump[ word_40 ];

	// FIXME: this is not quite right...
	// 539F
	if( ((!(A & byte_5646)) || !(A & byte_5644)) &&  
		(A & byte_5645) && !(A & byte_5643 )) {
		
		// 53B3
			if( A & byte_5648 ) {
				
				A ^= byte_5648;
				A ^= byte_5647;

				mDump[ word_40 ] = A;
				mDump[ 0x70A8 ] = mDump[ 0x70A6 ] = 0xC0;
				//RAM:53E6 8D 24 76                    STA     loc_7623+1

			} else {
				// 53D0
				A |= byte_5648;
				mDump[ word_40 ] = A;
				if( A & byte_5647 ) {
					mDump[ 0x70A6 ] = 0x50;
					mDump[ 0x70A8 ] = 0xC0;
					//RAM:53E6 8D 24 76                    STA     loc_7623+1
				} else {
					// 53EC
					mDump[ 0x70A6 ] = 0xC0;
					mDump[ 0x70A8 ] = 0x20;
					//RAM:53F8 8D 24 76                    STA     loc_7623+1
				}
			}

			// 53FB
			byte gfxPosX = mDump[ word_40 + 3 ];
			byte gfxPosY = mDump[ word_40 + 4 ];
			drawGraphics( 0, 0x82, gfxPosX, gfxPosY, 0 );

			sub_21C8(0xA);
	}

	// 541B
	A = 0xFF;

	A ^= byte_5644;
	A ^= byte_5643;
	A &= mDump[ word_40 ];
	// 5427
	if( A & byte_5646 ) {
		A |= byte_5644;
		A ^= byte_5646;
	}
	if( A & byte_5645 ) {
		A |= byte_5643;
		A ^= byte_5645;
	}

	mDump[ word_40 ] = A;
	// 543F
	if( A & byte_5648 ) {
		A = byte_2E36 & 1;
		if( A )
			return;

		byte gfxCurrentID = mDump[ 0xBF03 + pX ];

		if( !(mDump[ word_40 ] & byte_5647) ) {
			// 5458
			++gfxCurrentID;
			if( gfxCurrentID >= 0x82 )
				gfxCurrentID = 0x7E;

		} else {
			//546a
			--gfxCurrentID;
			if( gfxCurrentID < 0x7E )
				gfxCurrentID = 0x81;
		}
		// 5479
		byte gfxPosX = mDump[ 0xBF01 + pX ];
		byte gfxPosY = mDump[ 0xBF02 + pX ];
		
		SpriteMovement( gfxCurrentID, gfxPosX, gfxPosY, 0, pX );
	}
}

// 564E: Load the rooms' frankensteins
void cCreep::obj_PrepFrankenstein() {
	word_5748 = word_3E;
	byte byte_574B;

	byte_574A = 0;

	for(;;) {

		if( mDump[ word_3E ] & byte_574C ) {
			++word_3E;
			return;
		}

		mTxtX_0 = mDump[ word_3E + 1 ];
		mTxtY_0 = mDump[ word_3E + 2 ] + 0x18;
		drawGraphics( 1, 0, 0, 0, 0x92 );

		byte_5FD5 = (mTxtX_0 >> 2) - 4;
		byte_5FD6 = (mTxtY_0 >> 3);

		sub_5FA3();
		byte A;

		if( ( mDump[ word_3E ] & byte_574F )) {
			word_3C -= 2;
			A = 0xFB;
		} else
			A = 0xBF;

		// 56C4
		byte_574B = A;
		
		for( char Y = 4; Y >= 0; Y -= 2 ) {
			mDump[ word_3C + Y ] &= byte_574B;
		}
		byte X;
		sub_5750(X);
		mDump[ 0xBF00 + X ] = 0x0F;
		byte gfxPosX = mDump[ word_3E + 1 ];
		byte gfxPosY = mDump[ word_3E + 2 ];
		if( !(mDump[ word_3E ] & byte_574F ))
			A = 0x90;
		else
			A = 0x91;

		SpriteMovement( A, gfxPosX, gfxPosY, 0, X );

		//5700
		if(!( mDump[ word_3E ]  & byte_574F )) {
			gfxPosX += 4;
			gfxPosY += 0x18;
			drawGraphics( 0, 0x1C, gfxPosX, gfxPosY, 0 );
		}
		sub_3E87();

		word_3E += 0x07;
		byte_574A += 0x07;
	}

}

// 5501: Load the rooms' Conveyors
void cCreep::obj_PrepConveyor() {
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

void cCreep::sub_3A7F( byte pX ) {
	byte Y = pX;

	byte A = mDump[ 0xBE00 + Y ] + 0x07;
	A |= 0xF8;
	A >>= 1;
	A += 0x2C;
	
	// TODO
	//RAM:3A90 8D 93 75                    STA     loc_7591+2
	sub_21C8( 0 );

	byte X;
	sub_3F14( X );
	mDump[ 0xBD00 + X ] = 4;
	mDump[ 0xBD01 + X ] = mDump[ 0xBF01 + Y ];
	mDump[ 0xBD02 + X ] = mDump[ 0xBF02 + Y ] + 0x05;
	mDump[ 0xBD03 + X ] = 0x6C;
	mDump[ 0xBD1E + X ] = mDump[ 0xBE00 + Y ];

	if( mDump[ word_40 ] & byte_4D67 ) {
		mDump[ 0xBD01 + X ] -= 0x08;
		mDump[ 0xBD1F + X ] = 0xFC;
	} else {
		// 3AD4	
		mDump[ 0xBD01 + X ] += 0x08;
		mDump[ 0xBD1F + X ] = 4;
	}

	hw_SpritePrepare( X );
}

void cCreep::sub_396A( byte pA, byte pX ) {
	byte byte_39EE = pA;
	byte X;
	byte Y = pX;

	sub_3F14( X );
	
	mDump[ 0xBD00 + X ] = 3;
	mDump[ 0xBD1B + X ] = 0xFF;
	mDump[ 0xBD1C + X ] = 0xFF;
	mDump[ 0xBD1D + X ] = mDump[ 0xBE00 + Y ];
	
	word_40 = word_498B + mDump[ 0xBD1D + X ];
	//3998

	mDump[ 0xBD0C + X ] = 5;
	mDump[ 0xBD0D + X ] = 0x11;
	mDump[ 0xBD03 + X ] = 0xFF;
	if( byte_39EE == 0 ) {
		mDump[ 0xBD1E + X ] = 0;
		mDump[ 0xBD1F + X ] = 0xFF;
		mDump[ 0xBD06 + X ] = 4;
		
		mDump[ 0xBD01 + X ] = mDump[ word_40 + 3 ] + 0x0D;
		mDump[ 0xBD02 + X ] = mDump[ word_40 + 4 ] + 0x08;
	} else {
		// 39D0
		mDump[ 0xBD1E + X ] = 1;
		mDump[ 0xBD01 + X ] = mDump[ word_40 + 5 ];
		mDump[ 0xBD02 + X ] = mDump[ word_40 + 6 ];
		mDump[ 0xBD06 + X ] = 2;
	}

	// 39E8
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

// 4DE9: 
void cCreep::sub_4DE9( byte pA ) {
	byte byte_4E31 = pA;
	
	mDump[ 0x6DBF ] = pA;
	mDump[ 0x6DC0 ] = pA;

	byte gfxPosX = mDump[ word_40 + 5 ];
	byte gfxPosY = mDump[ word_40 + 6 ];

	drawGraphics( 0, 0x6E, gfxPosX, gfxPosY, 0 );
	mDump[ 0x6DBF ] = byte_4E31 << 4;
	mDump[ 0x6DC0 ] = byte_4E31 << 4;

	gfxPosY += 0x10;
	drawGraphics( 0, 0x6E, gfxPosX, gfxPosY, 0 );
}

// 505C: 
void cCreep::sub_505C( byte pA, byte pX ) {
	byte byte_50D1 = pA;

	byte A =  (pA << 4) | 0x0A;

	mDump[ 0x6E70 ] = mDump[ 0x6E71 ] = mDump[ 0x6E72 ] = A;
	mDump[ 0x6E73 ] = mDump[ 0x6E74 ] = mDump[ 0x6E75 ] = 0x0F;

	word_40 = *((word*) &mDump[ 0xBE00 + pX ]);

	byte gfxPosX = mDump[ word_40 ] + 4;
	byte gfxPosY = mDump[ word_40 + 1 ] ;
	drawGraphics( 0, 0x71, gfxPosX, gfxPosY, 0 );

	gfxPosY += 0x08;

	mDump[ 0x6E73 ] = mDump[ 0x6E74 ] = mDump[ 0x6E75 ] = 1;
	drawGraphics( 0, 0x71, gfxPosX, gfxPosY, 0 );
	gfxPosY += 0x08;
	drawGraphics( 0, 0x71, gfxPosX, gfxPosY, 0 );
}

void cCreep::sub_526F( char &pA ) {
	byte byte_5382;
	word word_5383, word_5385;

	byte_5382 = pA;

	word_5383 = word_40;
	word_5385 = word_3C;

	word_40 = word_5387 + byte_5382;
	// 529B
	mDump[ word_40 ] ^= byte_538A;
	byte X;

	for( X = 0 ;;X+=8) {
		if( mDump[ 0xBF00 + X ] != 0x0B )
			continue;
		if( mDump[ 0xBE00 + X ] == byte_5382 )
			break;
	}

	//52bd
	mDump[ 0xBF04 + X ] |= byte_840;
	
	if( !(mDump[ word_40 ] & byte_538A) ) {
		// 52cf
		mDump[ 0xBE01 + X ] = 0;
		mDump[ 0xBE02 + X ] = 0x78;
		mDump[ 0x6F2E ] = 0xC0;
		mDump[ 0x6F30 ] = 0x55;

		byte_5FD5 = (mDump[ word_40 + 1 ] >> 2) - 4;
		byte_5FD6 = mDump[ word_40 + 2 ] >> 3;

		sub_5FA3();

		mDump[ word_3C ] |= 0x04;
		mDump[ word_3C + 4 ] |= 0x40;

	} else {
		// 530F
		mDump[ 0xBE01 + X ] = 1;
		mDump[ 0xBE02 + X ] = 0x73;
		mDump[ 0x6F2E ] = 0x20;
		mDump[ 0x6F30 ] = 0xCC;
		
		byte_5FD5 = (mDump[ word_40 + 1 ] >> 2) - 4;
		byte_5FD6 = mDump[ word_40 + 2 ] >> 3;

		sub_5FA3();

		mDump[ word_3C ] &= 0xFB;
		mDump[ word_3C + 4 ] &= 0xBF;
	}

	// 534c
	byte gfxPosX = mDump[ word_40 + 3 ];
	byte gfxPosY = mDump[ word_40 + 4 ];

	drawGraphics( 0, 0x7A, gfxPosX, gfxPosY, 0 );

	word_40 = word_5383;
	word_3C = word_5385;
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
