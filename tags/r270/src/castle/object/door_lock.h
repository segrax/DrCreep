
class cCastle_Object_Door_Lock : public cCastle_Object {
private:
	byte	 mColor;

public:
			 cCastle_Object_Door_Lock( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 graphicUpdate();
	void	 prepare( );

};
