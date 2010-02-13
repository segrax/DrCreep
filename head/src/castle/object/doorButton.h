

class cCastle_Object_Door_Button : public cCastle_Object {
private:
	byte	 mColor, mLength;

	cCastle_Image	*mDoorImage;

public:
			 cCastle_Object_Door_Button( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 graphicUpdate();
	void	 prepare( );

};
