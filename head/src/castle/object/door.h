

class cCastle_Object_Door : public cCastle_Object {
private:
	bool	 mOpen;

public:
			 cCastle_Object_Door( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 draw();
	void	 graphicUpdate();
	void	 prepare( );

};
