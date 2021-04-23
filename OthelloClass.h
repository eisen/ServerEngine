#include <string>
#include <cstring>

enum TileState{
	EMPTY,
	BLACK,
	WHITE
};

class Board
{
public:
	TileState **tiles;
	TileState turn;
	int state;

	void setState(int);
	TileState ** getBoard(char*);
	TileState *** legalMoves(TileState **,TileState);
	TileState *** AIMoves(TileState **,TileState);
	int alphabeta(TileState**,int,int,int,TileState,TileState, int*, timeval, timeval);
	TileState changePiece(TileState);
	int h1(TileState**, TileState);
	int h(TileState**,TileState);
	int h2(TileState**,TileState);
	int moveSelect(int,int);
	int movesLeft(void);
	void switchTurn(void);
	TileState** pseudoplay(TileState**, int,int, TileState);
	int moveCount(TileState ***);
	int winCheck(TileState**);
	int init(void);
	TileState** initBoard(void);
	void reset(int);
	void display(void);
	void display2(TileState**,TileState);
	int isLegalMove(TileState **,TileState,int,int);
	void setBoard(TileState **);
	Board(void);	//constructor function
	~Board(void); // destructor function
};
