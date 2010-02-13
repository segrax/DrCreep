
class cCastle_Object_SlidingPole : public cCastle_Object {
private:
	byte	 mLength;

public:
			 cCastle_Object_SlidingPole( cCastle_Room *pRoom, byte *&pObjectBuffer );

	void	 draw();
	void	 graphicUpdate();
	void	 prepare( );

};
