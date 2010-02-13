

class cCastle_Object_Ladder : public cCastle_Object {
private:
	byte	 mLength;

public:
			 cCastle_Object_Ladder( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 graphicUpdate();
	void	 prepare( );

};
