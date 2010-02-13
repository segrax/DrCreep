

class cCastle_Object_Walkway : public cCastle_Object {
private:
	byte	 mLength;

public:
			 cCastle_Object_Walkway( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 graphicUpdate();
	void	 prepare( );

};
