#include "stdafx.h"
#include "castle/objects/object.hpp"
#include "creep.h"
#include "builder.hpp"
#include "castle/room.hpp"

void cObjectText::stateChange() {
	
	if(mTextColor < 15)
		++mTextColor;
	else
		mTextColor = 1;
}

void cObjectText::textSet() {
	mRoom->mBuilder->screenClear();
	mRoom->mBuilder->stringSet( 0x34, 0x00, 0x01, "ENTER TEXT" );

	mRoom->mBuilder->roomPrepare( 0xB000 );

	*mRoom->mBuilder->memory( 0x2788 ) = 0x20;
	*mRoom->mBuilder->memory( 0x2789 ) = 0x48;
	*mRoom->mBuilder->memory( 0x278C ) = 0x10;
	*mRoom->mBuilder->memory( 0x278A ) = 0x01;
	*mRoom->mBuilder->memory( 0x278B ) = 0x02;

	mRoom->mBuilder->textShow();

	if(mRoom->mBuilder->mStrLengthGet() > 0)
		mString = string( (char*) mRoom->mBuilder->memory( 0x278E ), (unsigned char) mRoom->mBuilder->mStrLengthGet() );
}