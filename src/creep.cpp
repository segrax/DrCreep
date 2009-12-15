#include "stdafx.h"
#include "creep.h"
#include "graphics/window.h"
#include "graphics/surface.h"

cCreep::cCreep() {
	
	mDumpSize = 0;
	mDump = fileRead( "object", mDumpSize );
	mLevel = 0;
	mQuit = false;

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
