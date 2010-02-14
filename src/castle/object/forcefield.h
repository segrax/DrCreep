
class cCastle_Object_Forcefield : public cCastle_Object {
private:
	byte	 mActive;

	void	 create();

public:
			 cCastle_Object_Forcefield( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 draw();
	void	 graphicUpdate();
	void	 prepare( );

};
