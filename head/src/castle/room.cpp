/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2010 Robert Crossfield
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
 *  Castle Room
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "room.h"
#include "castle.h"
#include "creep.h"
#include "object/object.h"
#include "object/objects.h"
#include "image.h"
#include <typeinfo>

cCastle_Room::cCastle_Room( cCastle *pCastle, byte *pRoomDirPtr  ) {

	mCastle = pCastle;

	mRoomDirPtr = pRoomDirPtr;
	
	mColor = *pRoomDirPtr;

	mMapTopLeftX	= *(pRoomDirPtr + 1 );
	mMapTopLeftY	= *(pRoomDirPtr + 2 );

	mMapWidthHeight = *(pRoomDirPtr + 3 );
	mMapWidth		=  (mMapWidthHeight & 7);
	mMapHeight		= ((mMapWidthHeight) >> 3) & 7;

	mRoomPtr		= mCastle->roomPtrGet( readWord( pRoomDirPtr + 6 ) );
	
	memset( &mObjectMap[0], 0, 0x800 );

	objectMapPtrPrepare();

	obj_Load();
}

void cCastle_Room::draw() {
	vector< cCastle_Object* >::iterator		objectIT;
	
	mCastle->creepGet()->screenClear();

	for( objectIT = mObjects.begin(); objectIT != mObjects.end(); ++objectIT ) {
		
		(*objectIT)->draw();
	}

}
cCastle_Object_Door	*cCastle_Room::objectDoorGet( word pCount ) {
	vector<cCastle_Object*>::iterator		objectIT;
	
	word count = 0;

	for( objectIT = mObjects.begin(); objectIT != mObjects.end(); ++objectIT ) {

		if( typeid( *(*objectIT) ) == typeid(cCastle_Object_Door)) {
			if( count == pCount )
				return (cCastle_Object_Door*) (*objectIT);

			++count;
		}
	}

	return 0;
}


void cCastle_Room::objectMapPtrPrepare() {

	// 0x08F9
	byte byte_30 = 0, byte_31 = 0;

	byte count = 0;
	while( count < 0x20 ) {
		byte_5CE6[ count ] = byte_30;
		byte_5D06[ count ] = byte_31;

		if( (byte_30 + 0x28) > 0xFF)
			++byte_31;

		byte_30 += 0x28;
		++count;
	}
}

byte *cCastle_Room::objectMapGet( byte byte_5FD5, byte byte_5FD6 ) {

	// 5fa6
	word word_3C = byte_5CE6[ byte_5FD6 ];
	word_3C += byte_5D06[ byte_5FD6 ] << 8;

	word_3C <<= 1;

	byte A = byte_5FD5 << 1;
	word_3C += A;

	return &mObjectMap[ word_3C ];
}

void cCastle_Room::objectAdd( cCastle_Object *pObject ) {
	mObjects.push_back( pObject);
}

bool cCastle_Room::objectRemove( cCastle_Object *pObject, bool pDelete ) {
	vector< cCastle_Object* >::iterator mObjectIT;

	for(mObjectIT = mObjects.begin(); mObjectIT != mObjects.end(); ++mObjectIT )
		if( pObject == (*mObjectIT) ) {
			mObjects.erase( mObjectIT );
			if( pDelete )
				delete pObject;

			return true;
		}

	return false;
}

void cCastle_Room::obj_Door_Load( byte *&pObjectBuffer ) {
	byte			 doorCount = *pObjectBuffer++;
	cCastle_Object	*object = 0;
	
	mDataDoorPtr = pObjectBuffer;

	for( byte count = 0; count != doorCount; ++count) {

		object = new cCastle_Object_Door( this, pObjectBuffer );

		mObjects.push_back( object );

		pObjectBuffer += 0x08;
	}

}

void cCastle_Room::obj_Walkway_Load( byte *&pObjectBuffer ) {
	byte					*buffer = pObjectBuffer;
	cCastle_Object_Walkway	*object = 0;

	while( (*pObjectBuffer)) {
		
		object = new cCastle_Object_Walkway( this, pObjectBuffer );
		
		mObjects.push_back( object );
		pObjectBuffer += 0x03;
	}
	
	++pObjectBuffer;
}

void cCastle_Room::obj_SlidingPole_Load( byte *&pObjectBuffer ) {
	byte						*buffer = pObjectBuffer;
	cCastle_Object_SlidingPole	*object = 0;

	while( (*pObjectBuffer)) {
		
		object = new cCastle_Object_SlidingPole( this, pObjectBuffer );
		
		mObjects.push_back( object );
		pObjectBuffer += 0x03;
	}
	
	++pObjectBuffer;
}

void cCastle_Room::obj_Ladder_Load( byte *&pObjectBuffer ) {
	byte						*buffer = pObjectBuffer;
	cCastle_Object_Ladder	*object = 0;

	while( (*pObjectBuffer)) {
		
		object = new cCastle_Object_Ladder( this, pObjectBuffer );
		
		mObjects.push_back( object );
		pObjectBuffer += 0x03;
	}
	
	++pObjectBuffer;
}

void cCastle_Room::obj_Door_Button_Load( byte *&pObjectBuffer ) {
	byte			 buttonCount = *pObjectBuffer++;
	cCastle_Object	*object = 0;

	for( byte count = 0; count != buttonCount; ++count) {

		object = new cCastle_Object_Door_Button( this, pObjectBuffer );

		mObjects.push_back( object );

		pObjectBuffer += 0x03;
	}
}

void cCastle_Room::obj_Lightning_Load( byte *&pObjectBuffer ) {
	cCastle_Object	*object = 0;
	
	mDataLightingPtr = pObjectBuffer;

	for( byte count = 0;; ++count) {

		if( *pObjectBuffer & byte_45DF )
			break;

		object = new cCastle_Object_Lightning( this, pObjectBuffer );

		mObjects.push_back( object );

		pObjectBuffer += 0x08;
	}

	++pObjectBuffer;
}

void cCastle_Room::obj_Forcefield_Load( byte *&pObjectBuffer ) {
	cCastle_Object *object = 0;

	for( byte count = 0;; ++count ) {
		
		if( *pObjectBuffer == 0 )
			break;

		object = new cCastle_Object_Forcefield( this, pObjectBuffer );
		mObjects.push_back( object );

		pObjectBuffer += 0x04;
	}

	++pObjectBuffer;
}

void cCastle_Room::obj_Mummy_Load( byte *&pObjectBuffer ) {
	cCastle_Object *object = 0;

	mDataMummyPtr = pObjectBuffer;

	for( byte count = 0;; ++count ) {
		
		if( *pObjectBuffer == 0x00 )
			break;

		object = new cCastle_Object_Mummy( this, pObjectBuffer );
		mObjects.push_back( object );

		pObjectBuffer += 0x07;
	}

	++pObjectBuffer;
}

void cCastle_Room::obj_Key_Load( byte *&pObjectBuffer ) {
	cCastle_Object *object = 0;

	for( byte count = 0;; ++count ) {
		
		if( *pObjectBuffer == 0x00 )
			break;

		object = new cCastle_Object_Key( this, pObjectBuffer );
		mObjects.push_back( object );

		pObjectBuffer += 0x04;
	}

	++pObjectBuffer;
}

void cCastle_Room::obj_Door_Lock_Load( byte *&pObjectBuffer ) {
	cCastle_Object *object = 0;

	for( byte count = 0;; ++count ) {
		
		if( *pObjectBuffer == 0x00 )
			break;

		object = new cCastle_Object_Door_Lock( this, pObjectBuffer );
		mObjects.push_back( object );

		pObjectBuffer += 0x05;
	}

	++pObjectBuffer;
}

void cCastle_Room::obj_RayGun_Load( byte *&pObjectBuffer ) {
	
}

void cCastle_Room::obj_Teleport_Load( byte *&pObjectBuffer ) {

}

void cCastle_Room::obj_TrapDoor_Load( byte *&pObjectBUffer ) {

}

void cCastle_Room::obj_Conveyor_Load( byte *&pObjectBuffer ) {

}

void cCastle_Room::obj_Frankie_Load( byte *&pObjectBuffer ) {

}

void cCastle_Room::obj_Load() {
	word			 func = 0x01;
	byte			*roomPtr = mRoomPtr;
	
	while(func) {

		func = readWord( roomPtr );
		roomPtr  += 2;

		switch( func ) {
			case 0:			// Finished
				return;

			case 0x0803:
				obj_Door_Load( roomPtr );
				break;

			case 0x0806:
				obj_Walkway_Load( roomPtr );
				break;

			case 0x0809:
				obj_SlidingPole_Load( roomPtr );
				break;

			case 0x080C:
				obj_Ladder_Load( roomPtr );
				break;

			case 0x080F:
				obj_Door_Button_Load( roomPtr );
				break;

			case 0x0812:
				obj_Lightning_Load( roomPtr );
				break;

			case 0x0815:
				obj_Forcefield_Load( roomPtr );
				break;

			case 0x0818:
				obj_Mummy_Load( roomPtr );
				break;

			case 0x081B:
				obj_Key_Load( roomPtr );
				break;

			case 0x081E:
				obj_Door_Lock_Load( roomPtr );
				break;

			case 0x0824:
				obj_RayGun_Load( roomPtr );
				break;

			case 0x0827:
				obj_Teleport_Load( roomPtr );
				break;

			case 0x082A:
				obj_TrapDoor_Load( roomPtr );
				break;

			case 0x082D:
				obj_Conveyor_Load( roomPtr );
				break;

			case 0x0830:
				obj_Frankie_Load( roomPtr );
				break;

			/*case 0x0833:
			case 0x2A6D:
				//obj_stringPrint();
				break;
			case 0x0836:
				//obj_Image_Draw();
				break;
			case 0x0821:
			case 0x160A:	// Intro
				//obj_MultiDraw( );
				break;*/

			default:
				cout << "objectsLoad: 0x";
				cout << std::hex << func << "\n";

				break;
		}

	}
}

// 580D
void cCastle_Room::screenDraw(  word pDecodeMode, word pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtX_0, byte pTxtY_0, byte pTxtCurrentID ) {
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
	byte drawingFirst = 0;

	byte mTxtPosLowerY, mTxtDestXLeft, mTxtDestXRight;
	byte mTxtEdgeScreenX, mTxtDestX, mTxtWidth, mTxtHeight, mCount;
	byte mGfxWidth, mGfxHeight, mGfxEdgeOfScreenX;
	byte byte_5CE2, byte_5CE5;

	word word_30, word_32;

	if( pDecodeMode	!= 0 ) {
		// Draw Text
		word word_38 = pTxtCurrentID;
		
		word_38 <<= 1;
		word_38 += 0x603B;

		word_30 = readWord( mCastle->creepGet()->gameData( word_38 ) );
		
		mTxtWidth = *mCastle->creepGet()->gameData( word_30 );
		mTxtHeight = *mCastle->creepGet()->gameData( word_30 + 1);
		mCount = *mCastle->creepGet()->gameData( word_30 + 1);

		mTxtPosLowerY = pTxtY_0 + mTxtHeight;
		--mTxtPosLowerY;

		if( pTxtX_0 < 0x10 ) {
			mTxtDestX = 0xFF;
			mTxtEdgeScreenX = 0xFF;
		} else {
			mTxtDestX = pTxtX_0 - 0x10;
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
		
		word_32 = readWord( mCastle->creepGet()->gameData( byte_38 ));
		
		mGfxWidth = *mCastle->creepGet()->gameData( word_32 );
		mGfxHeight = *mCastle->creepGet()->gameData( word_32 + 1 );
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
		
		videoPtr0 = byte_5CE6[A];
		videoPtr1 = byte_5D06[A];
		
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

		gfxPosTopY = pTxtY_0;
		gfxBottomY = mTxtPosLowerY;

	} else if( pDecodeMode == 2 ) {
		byte A = 0;

		if( pGfxPosY != pTxtY_0 ) {
			if( pGfxPosY >= pTxtY_0 ) {
				
				if( pGfxPosY >= 0xDC || pTxtY_0 < 0xDC ) {
					 A = pTxtY_0;
				}  
			} else {
				// 5a17
				if( pTxtY_0 >= 0xDC && pGfxPosY < 0xDC ) {
					A = pTxtY_0;
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
	
	word byte_34 = *mCastle->creepGet()->memory( 0xBC00 + gfxCurrentPosY );
	byte_34 |= (*mCastle->creepGet()->memory( 0xBB00 + gfxCurrentPosY )) << 8;

	// 5A77
	for(;;) {
		
		if( pDecodeMode != 0 && mCount != 0 ) {
			
			if( drawingFirst != 1 ) {
				if( pTxtY_0 == gfxCurrentPosY ) 
					drawingFirst = 1;
				else
					goto s5AED;
			}
			//5A97
			--mCount;

			if( gfxCurrentPosY < 0xC8 ) {
				
				gfxCurPos = mTxtDestXLeft;

				word word_36 = (byte_34 + mTxtDestX) + (mTxtEdgeScreenX << 8);


				// 5AB8
				for( byte Y = 0;; ++Y ) {
					if( gfxCurPos < 0x28 ) {
						
						byte A = *mCastle->creepGet()->gameData( word_30 + Y );
						A ^= 0xFF;
						A &= *mCastle->creepGet()->memory( word_36 + Y);
						*mCastle->creepGet()->memory( word_36 + Y) = A;
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
s5AED:;
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
						byte A = *mCastle->creepGet()->gameData( word_32 + Y );
						A |=*mCastle->creepGet()->memory( byte_36 + Y);
						*mCastle->creepGet()->memory( byte_36 + Y) = A;
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
		byte_5CE5 = 1;
	else
		byte_5CE5 = 0;

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
					*mCastle->creepGet()->memory( word_30 + Y ) = *mCastle->creepGet()->gameData( word_32 + Y );
			
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
			if( byte_5CE5 != 1 )
				break;

			byte_5CE5 = 0;

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
		byte_5CE5 = 1;
	else
		byte_5CE5 = 0;

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
					*mCastle->creepGet()->memory( word_30 + Y ) = *mCastle->creepGet()->gameData( word_32 + Y );
				
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
			if( byte_5CE5 != 1 )
				return;

			byte_5CE5 = 0;
			if( gfxCurrentPosY != 0xFF )
				if( gfxCurrentPosY >= 0x18 )
					return;
		}
		// 5CB0
		word_30 += 0x28;
	}
}
