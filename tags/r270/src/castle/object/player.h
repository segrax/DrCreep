class cCastle_Object;

class cCastle_Object_Player : public cCastle_Object {

private:


public:
				cCastle_Object_Player( cCastle_Room *pRoom, cCastle_Object_Door *pDoor );
 
				
	void		graphicUpdate();

	void		roomChange( cCastle_Room *pRoom );
};
