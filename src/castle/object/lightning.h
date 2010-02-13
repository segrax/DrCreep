

class cCastle_Object_Lightning : public cCastle_Object {
private:
	byte	 mLength;

public:
			 cCastle_Object_Lightning( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 graphicUpdate();
	void	 prepare( );

};
