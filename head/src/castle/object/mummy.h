

class cCastle_Object_Mummy : public cCastle_Object {
private:

	void	 add( byte pA );

public:
			 cCastle_Object_Mummy( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 graphicUpdate();
	void	 prepare( );

};
