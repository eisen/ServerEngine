#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>
#include "OthelloClass.h"
#define NOHEURVAL 2000000000
#define RETOVERHEAD 50
#define POSINF 2000000000

using namespace std;
static int movetime = 5000;

Board::Board(void)
{
	tiles = new TileState *[8];
	for (int i = 0; i < 8; i++) {
		tiles[i] = new TileState [8];
	}
	for(int i = 0;i<8;i++) {
		for(int j=0;j<8;j++) {
			tiles[i][j] = EMPTY;
		}
	}
	tiles[4][3] = BLACK;
	tiles[3][4] = BLACK;
	tiles[3][3] = WHITE;
	tiles[4][4] = WHITE;
	state = 0;
	turn = BLACK;
}
Board::~Board(void) // destructor
{
	;
}
void Board::display()
{
	static char row[] =  "   \033[40;32;2;7m|    ||    |"
						    "\033[40;32;2;7m|    ||    |"
							"\033[40;32;2;7m|    ||    |"
							"\033[40;32;2;7m|    ||    |\033[0m\n";
	static char row2[] = "   \033[40;32;2;7m|____||____|"
						    "\033[40;32;2;7m|____||____|"
							"\033[40;32;2;7m|____||____|"
							"\033[40;32;2;7m|____||____|\033[0m\n";
	static char row3[] = "   \033[40;32;2;7m____________"
						    "\033[40;32;2;7m____________"
							"\033[40;32;2;7m____________"
							"\033[40;32;2;7m____________\033[0m\n";
	static char spot[] = "\033[40;32;2;7m ";
	static char p1[] = "\033[47;30;7m  ";
	static char p2[] = "\033[40;37;7m  ";
	static char normal[] = "\033[0m";
	cout << endl << "Player 1 -> \033[47;30;7mBlack" << normal << endl;
	cout << "Player 2 -> \033[40;37;7mWhite" << normal << endl << endl;
	cout << "     00    01    02    03    04    05    06    07"<<endl;
	cout << row3;
	for (int i = 0; i < 8; i++) {
	cout << row;
	cout << " " << i << " ";
	for (int j = 0; j < 8; j++) {
		cout << "\033[40;32;2;7m|" << spot << normal;
		if (tiles[i][j] == BLACK) {
		  cout << p1;
		} else if (tiles[i][j] == WHITE) {
		  cout << p2;
		} else{
		  cout << spot << spot << normal;
		}
		cout << spot << "|";
	}
	cout << "\033[0m\n";
	cout << row2;
  }
}
int Board::isLegalMove(TileState** b, TileState p, int i, int j)
{
	//pointer to a pointer to a pointer of an 64x8x8 tilestates
	int legalmoven = 0;
	int tc1;
	int tc2;
	int xchange;
	int ychange;
	int rc;
	int mc = 0;

	for (tc1 = 0;tc1<8;tc1++) {
		for (tc2 = 0;tc2<8;tc2++) {
			if (b[tc1][tc2] == EMPTY) {
				for(xchange = -1;xchange < 2;xchange++) {
					for (ychange = -1;ychange < 2;ychange++) {

						//if tile is a valid board space
						if (tc1+xchange >= 0 && tc1+xchange <=7 && tc2+ychange >= 0 && tc2+ychange <=7 ) {

							//if the next tile is empty or the same as the players piece
							if(p == b[tc1+xchange][tc2+ychange] || b[tc1+xchange][tc2+ychange] == EMPTY) {
								;//do nothing since cant move do to this direction
							}
							else //the next tile is the opponents piece
							{

								for (rc=2;rc<8;rc++) { //search for a piece of the same type in a direction

									//if tile is a valid board space
									if(tc1+rc*xchange >= 0 && tc1+rc*xchange <=7 && tc2+rc*ychange >= 0 && tc2+rc*ychange <=7 ) {

										//if a same piece is found in direction
										if(b[tc1+rc*xchange][tc2+rc*ychange] == p) {
											mc++;
											if (tc1 == i && tc2 == j) {
												legalmoven = mc;
											}
											goto NextTile;
										}

										//if empty spot found in direction
										else if (b[tc1+rc*xchange][tc2+rc*ychange] == EMPTY) {
											break;
										}

									}
									else {
										break;
									}
								}
							}
						}
					}
				}
			}
			NextTile:
			;
		}
	}
	return legalmoven;
}
void Board::display2(TileState** b,TileState p)
{
	int mn;
	ostringstream strm;
	string numStr1;
	string numStr2;

	static char row[] =  "   \033[40;32;2;7m|    ||    |"
						    "\033[40;32;2;7m|    ||    |"
							"\033[40;32;2;7m|    ||    |"
							"\033[40;32;2;7m|    ||    |\033[0m\n";
	static char row2[] = "   \033[40;32;2;7m|____||____|"
						    "\033[40;32;2;7m|____||____|"
							"\033[40;32;2;7m|____||____|"
							"\033[40;32;2;7m|____||____|\033[0m\n";
	static char row3[] = "   \033[40;32;2;7m____________"
						    "\033[40;32;2;7m____________"
							"\033[40;32;2;7m____________"
							"\033[40;32;2;7m____________\033[0m\n";
	static char spot[] = "\033[40;32;2;7m ";
	static char p1[] = "\033[47;30;7m  ";
	static char p2[] = "\033[40;37;7m  ";
	static char m1[] = "\033[47;32;2;7m";
	static char m2[] = "\033[40;32;2;7m";
	static char normal[] = "\033[0m";
	cout << endl << "Player 1 -> \033[47;30;7mBlack" << normal << endl;
	cout << "Player 2 -> \033[40;37;7mWhite" << normal << endl << endl;
	cout << "     00    01    02    03    04    05    06    07"<<endl;
	cout << row3;
	for (int i = 0; i < 8; i++) {
	cout << row;
	cout << " " << i << " ";
	for (int j = 0; j < 8; j++) {
		cout << "\033[40;32;2;7m|" << spot << normal;
		if (tiles[i][j] == BLACK) {
		  cout << p1;
		}
		else if (tiles[i][j] == WHITE) {
		  cout << p2;
		}
		else if ((mn = isLegalMove(b,p,i,j)) != 0) {
			strm.str("");
			strm.clear();
			if (floor(mn/10) != 0) {
				strm << floor(mn/10);
				numStr1 = strm.str();
			}
			else {
				strm << " ";
				numStr1 = strm.str();
			}
			strm.str("");
			strm.clear();
			strm << (mn % 10);
			numStr2 = strm.str();

			if (p == WHITE) {
				cout << m1 << numStr1 << numStr2;
			}
			else {
				cout << m2 << numStr1 << numStr2;
			}
		}
		else {
			cout << spot << spot << normal;
		}
		cout << spot << "|";
	}
	cout << "\033[0m\n";
	cout << row2;
  }
}
void Board::setState(int s)
{
	state = s;
}
int Board::init(void)
{
	char ans1 = '\0', ans2 = '\0';
	float inputtime = 0;
	while (ans1 != 'y' && ans1 != 'Y' && ans1 != 'n' && ans1 != 'N')
	{
		cout << "is Black Human? (y/n): " << flush;
		cin >> ans1;
	}

	while (ans2 != 'y' && ans2 != 'Y' && ans2 != 'n' && ans2 != 'N')
	{
		cout << "is White Human? (y/n): " << flush;
		cin >> ans2;
	}

	if ((ans1 == 'y' || ans1 == 'Y') && (ans2 == 'y' || ans2 == 'Y')) {
		cout << "Player 1 \033[47;30;7mBlack\033[0m is Human" << endl;
		cout << "Player 2 \033[40;37;7mWhite\033[0m is Human" << endl<<endl;
		return 0;
	}
	else if ((ans1 == 'y' || ans1 == 'Y') && (ans2 == 'n' || ans2 == 'N')) {
		cout << "Player 1 \033[47;30;7mBlack\033[0m is Human" << endl;
		cout << "Player 2 \033[40;37;7mWhite\033[0m is Computer" << endl <<endl;
		cout << "How much time should the AI take on its turn (in seconds): " << flush;
		cin >> inputtime;
		if(!inputtime || inputtime <= 0)
		{
			cout << "NOT A VALID ENTRY, Selecting AI move time to: 5 Seconds" << endl;
		}
		else {
			movetime = (int)(1000*inputtime);
		}
		cout << endl;
		return 1;
	}
	else if ((ans1 == 'n' || ans1 == 'N') && (ans2 == 'y' || ans2 == 'Y')) {
		cout << "Player 1 \033[47;30;7mBlack\033[0m is Computer" << endl;
		cout << "Player 2 \033[40;37;7mWhite\033[0m is Human" << endl<<endl;
		cout << "How much time should the AI take on its turn (in seconds): " << flush;
		cin >> inputtime;
		if(!inputtime || inputtime <= 0)
		{
			cout << "NOT A VALID ENTRY, Selecting AI move time to: 5 Seconds" << endl;
		}
		else {
			movetime = (int)(1000*inputtime);
		}
		cout << endl;
		return 2;
	}
	else if ((ans1 == 'n' || ans1 == 'N') && (ans2 == 'n' || ans2 == 'N')) {
		cout << "Player 1 \033[47;30;7mBlack\033[0m is Computer" << endl;
		cout << "Player 2 \033[40;37;7mWhite\033[0m is Computer" << endl<<endl;
		cout << "How much time should the AI take on its turn (in seconds): " << flush;
		cin >> inputtime;
		if(!inputtime || inputtime <= 0)
		{
			cout << "NOT A VALID ENTRY, Selecting AI move time to: 5 Seconds" << endl;
		}
		else {
			movetime = (int)(1000*inputtime);
		}
		cout << endl;
		return 3;
	}
	else {
		cout << "Unknown Gametype" <<endl;
		return 5;
	}
}
TileState ** Board::initBoard()
{
	char filename[100];
	cout << "Do you want to input a non-default inital board state? (N/y): " << flush;
	cin.clear();
	cin.ignore(10000, '\n');
	if (tolower(cin.get()) == 'y')
	{
		cout << "Input the name of the file containing the board state: " << flush;
		cin.clear();
		cin.ignore(10000, '\n');
		cin >> filename;
		tiles = getBoard(filename);
		//get file name and fill in board to tiles
	}
	return tiles;
}
TileState** Board::getBoard(char * filename)
{

	int lcount = 0;
	int colcount = 0;
	int piececount = 0;
	float AItime = 5;
	TileState ** tempb;
	tempb = new TileState *[8];
	for (int i = 0; i < 8; i++) {
		tempb[i] = new TileState [8];
	}

	cout << endl << "Opening and initializing based on file " << filename << endl;
	ifstream infile(filename);

	string line;
	while (getline(infile, line))
	{
			lcount++;
			istringstream iss(line);
			if (lcount > 0 && lcount < 9)
			{
				colcount = 0;
				piececount = 0;
				while (piececount < 9) {
					if(line[colcount] == '0' )
					{
						tempb[lcount-1][piececount] = EMPTY;
						piececount++;
					}
					else if ( line[colcount] == '1') {
						tempb[lcount-1][piececount] = BLACK;
						piececount++;
					}
					else if (line[colcount] == '2') {
						tempb[lcount-1][piececount] = WHITE;
						piececount++;
					}
					colcount++;
				}
			}
			else if (lcount == 9)
			{
				colcount = 0;
				piececount = 0;
				while (piececount < 1)
				{
					if(line[colcount] == '1')
					{
						turn = BLACK;
						piececount++;
					}
					else if (line[colcount] == '2')
					{
						turn = WHITE;
						piececount++;
					}
					colcount++;
				}
			}
			else if (lcount == 10)
			{ //need to convert string to float
				AItime = atof(line.c_str());
				if(!AItime || AItime <= 0)
				{
					cout << "NOT A VALID ENTRY, Selecting AI move time to: 5 Seconds" << endl;
				}
				else {
					movetime = (int)(1000*AItime);
					cout << "Computer Time: " << AItime << " Seconds" << endl;
				}
			}
	}
	cout << endl;
	return tempb;
}
void Board::setBoard(TileState **brd)
{
	for(int i = 0;i<8;i++) {
		for(int j=0;j<8;j++) {
			tiles[i][j] = brd[i][j];
		}
	}
}
TileState *** Board::legalMoves(TileState** b, TileState p)
{
	//pointer to a pointer to a pointer of an 64x8x8 tilestates
	TileState*** moves = new TileState**[64];
	for(int k = 0; k < 64; ++k) {
		moves[k] = NULL;
	}

	moves[0] = NULL;

	int tc1;
	int tc2;
	int xchange;
	int ychange;
	int rc;
	int mc = 0;

	for (tc1 = 0;tc1<8;tc1++) {
		for (tc2 = 0;tc2<8;tc2++) {
			if (b[tc1][tc2] == EMPTY) {
				for(xchange = -1;xchange < 2;xchange++) {
					for (ychange = -1;ychange < 2;ychange++) {

						//if tile is a valid board space
						if (tc1+xchange >= 0 && tc1+xchange <=7 && tc2+ychange >= 0 && tc2+ychange <=7 ) {

							//if the next tile is empty or the same as the players piece
							if(p == b[tc1+xchange][tc2+ychange] || b[tc1+xchange][tc2+ychange] == EMPTY) {
								;//do nothing since cant move do to this direction
							}
							else //the next tile is the opponents piece
							{

								for (rc=2;rc<8;rc++) { //search for a piece of the same type in a direction

									//if tile is a valid board space
									if(tc1+rc*xchange >= 0 && tc1+rc*xchange <=7 && tc2+rc*ychange >= 0 && tc2+rc*ychange <=7 ) {

										//if a same piece is found in direction
										if(b[tc1+rc*xchange][tc2+rc*ychange] == p) {
											cout << (mc+1) << ". move at (" << tc1 << "," << tc2 << ")" <<  endl;
											moves[mc] = pseudoplay(b,tc1,tc2,p);

											mc++;
											moves[mc] = NULL;
											goto NextTile;
										}

										//if empty spot found in direction
										else if (b[tc1+rc*xchange][tc2+rc*ychange] == EMPTY) {
											break;
										}

									}
									else {
										break;
									}
								}
							}
						}
					}
				}
			}
			NextTile:
			;
		}
	}
	if (false) {//}moves->size() == 0) {
		return NULL;
	}
	else {
		return moves;
	}
}
int Board::moveCount(TileState*** moves)
{
	int count;
	for(count=0;moves[count]!=NULL && count < 65;count++)
	{
		;
	}
	return count;
}
TileState Board::changePiece(TileState p)
{
	TileState newp;
	if (p == WHITE)
		newp=BLACK;
	else
		newp=WHITE;

	return newp;
}
void Board::switchTurn(void)
{
	if (turn == BLACK)
	{
		turn = WHITE;
	}
	else if (turn == WHITE)
	{
		turn = BLACK;
	}
}
int Board::winCheck(TileState** brd)
{
	int w=0,b=0;
	for(int i=0;i<8;i++) {
		for(int j=0;j<8;j++) {
			if (brd[i][j] == WHITE) {
				w++;
			}
			else if (brd[i][j] == BLACK) {
				b++;
			}
		}
	}
	if (b > w)
	{
		cout << "black wins: " << b << " - " << w <<  endl;
		return 1;
	}
	else if (w > b) {
		cout << "white wins: " << w << " - " << b << endl;
		return 2;
	}
	else if (w == b) {
		cout << "tie: " << w << " - " << b << endl;
		return 3;
	}
	else {
		return 0;
	}
}
int Board::movesLeft(void)
{
	int movL = 0;
	for(int i=0;i<8;i++) {
		for(int j=0;j<8;j++) {
			if(tiles[i][j] == EMPTY) {
				movL++;
			}
		}
	}
	return movL;
}
TileState** Board::pseudoplay(TileState** b, int i,int j, TileState p)
{
	int xchange, ychange,rc,rc2;
	TileState** pseudoboard = new TileState*[8];
		for(int k = 0; k < 8; ++k)
			pseudoboard[k] = new TileState[8]();

	for (int k=0; k< 8; k++) {
		for (int l=0; l < 8; l++) {
			pseudoboard[k][l] = b[k][l];
		}
	}

	for(xchange = -1;xchange < 2;xchange++) {
		for (ychange = -1;ychange < 2;ychange++) {
			//if tile is a valid board space
			if (i+xchange >= 0 && i+xchange <=7 && j+ychange >= 0 && j+ychange <=7 ) {
				//if the next tile is empty or the same as the players piece
				if(p == b[i+xchange][j+ychange] || b[i+xchange][j+ychange] == EMPTY) {
					;//do nothing since cant move do to this direction
				}
				else //the next tile is the opponents piece
				{
					for (rc=2;rc<8;rc++) { //search for a piece of the same type in a direction
						//if tile is a valid board space
						if(i+rc*xchange >= 0 && i+rc*xchange <=7 && j+rc*ychange >= 0 && j+rc*ychange <=7 ) {
							//if a same piece is found in direction
							if(b[i+rc*xchange][j+rc*ychange] == p) {
								for(rc2 = 0;rc2<rc;rc2++) {
									pseudoboard[i+rc2*xchange][j+rc2*ychange] = p;
								}
								break;
							}
							//if empty spot found in direction
							else if (b[i+rc*xchange][j+rc*ychange] == EMPTY) {
								break;
							}
						}
						else {
							break;
						}
					}
				}
			}
		}
	}
	return pseudoboard;
}
TileState *** Board::AIMoves(TileState** b, TileState p)
{
	//pointer to a pointer to a pointer of an 64x8x8 tilestates
	TileState*** moves = new TileState**[64];
	for(int k = 0; k < 64; ++k) {
		moves[k] = NULL;
	}

	moves[0] = NULL;

	int tc1;
	int tc2;
	int xchange;
	int ychange;
	int rc;
	int mc = 0;

	for (tc1 = 0;tc1<8;tc1++) {
		for (tc2 = 0;tc2<8;tc2++) {
			if (b[tc1][tc2] == EMPTY) {
				for(xchange = -1;xchange < 2;xchange++) {
					for (ychange = -1;ychange < 2;ychange++) {

						//if tile is a valid board space
						if (tc1+xchange >= 0 && tc1+xchange <=7 && tc2+ychange >= 0 && tc2+ychange <=7 ) {

							//if the next tile is empty or the same as the players piece
							if(p == b[tc1+xchange][tc2+ychange] || b[tc1+xchange][tc2+ychange] == EMPTY) {
								;//do nothing since cant move do to this direction
							}
							else //the next tile is the opponents piece
							{

								for (rc=2;rc<8;rc++) { //search for a piece of the same type in a direction

									//if tile is a valid board space
									if(tc1+rc*xchange >= 0 && tc1+rc*xchange <=7 && tc2+rc*ychange >= 0 && tc2+rc*ychange <=7 ) {

										//if a same piece is found in direction
										if(b[tc1+rc*xchange][tc2+rc*ychange] == p) {
											moves[mc] = pseudoplay(b,tc1,tc2,p);
											mc++;
											moves[mc] = NULL;
											goto NextTileAI;
										}

										//if empty spot found in direction
										else if (b[tc1+rc*xchange][tc2+rc*ychange] == EMPTY) {
											break;
										}

									}
									else {
										break;
									}
								}
							}
						}
					}
				}
			}
			NextTileAI:
			;
		}
	}
	return moves;
}
int Board::alphabeta(TileState** brd, int d, int a, int b, TileState p, TileState pt, int* ind, timeval start, timeval end)
{
	//ind is the pointer to moven only passed in when first called
	int tempv;
	int v = 0;
	int i = 0;
	long mtime, seconds, useconds;
	TileState *** nextMoves;

	gettimeofday(&end,NULL);
	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0);

	if (mtime + RETOVERHEAD > movetime) {
		return NOHEURVAL;
	}

	//delete AI moves somewhere where its not needed: after depth is searched?
	//modify AIMoves to only return a specific move number and if its NULL then
	//exit the loop  and return v
	nextMoves = AIMoves(brd,pt);
	if (d == 0 || nextMoves[0] == NULL) {
		if (p == BLACK) {
			v = h2(brd,p);
		}
		if (p == WHITE) {
			v = h2(brd,p);
		}
		while (nextMoves[i] != NULL)
		{
			i++;
		}
		for(int k = 0; k < i; ++k) {
			for (int l = 0;l<8;++l) {
				delete [] nextMoves[k][l];
			}
			delete [] nextMoves[k];
		}
		delete [] nextMoves;
	}
	else if (pt == p) {
		v = -1*POSINF;
		pt = changePiece(pt);
		while (nextMoves[i] != NULL)
		{
			tempv = alphabeta(nextMoves[i],d-1,a,b,p,pt,NULL,start,end);
			if (v < tempv) {
				v = tempv;
				if (ind != NULL) {
					*ind = i;
				}
			}
			a = (a < v) ? v : a;
			if (b <= a)
				break;
			i++;
		}
		while (nextMoves[i] != NULL)
		{
			i++;
		}
		for(int k = 0; k < i; ++k) {
			for (int l = 0;l<8;++l) {
				delete [] nextMoves[k][l];
			}
			delete [] nextMoves[k];
		}
		delete [] nextMoves;
	}
	else {
		v = POSINF;
		pt = changePiece(pt);
		while (nextMoves[i] != NULL)
		{
			tempv = alphabeta(nextMoves[i],d-1,a,b,p,pt,NULL,start,end);
			if (v >= tempv) {
				v = tempv;
			}
			b = (b < v) ? b : v;
			if (b <= a)
				break;
			i++;
		}
		while (nextMoves[i] != NULL)
		{
			i++;
		}
		for(int k = 0; k < i; ++k) {
			for (int l = 0;l<8;++l) {
				delete [] nextMoves[k][l];
			}
			delete [] nextMoves[k];
		}
		delete [] nextMoves;
	}
	return v;
}
int Board::moveSelect(int gametype, int movemax)
{
	struct timeval start, end;
	long mtime, seconds, useconds;
	int moven = 0; // pass in &moven for alphabet
	int tempmoven = 0;
	int movesleft;
	string input;

	if(gametype == 0 || (gametype == 1 && turn == BLACK) || (gametype == 2 && turn == WHITE))
	{
		cout << endl << "Player " << turn <<" Enter move: " << flush;
		NewIn:
		cin >> moven;
		if(!moven || moven < 1 || moven > movemax)
		{
			cout << "Not a Valid Entry, Select another move: " << flush;
			cin.clear();
			cin.ignore(10000, '\n');
			goto NewIn;
		}
	}
	else if (gametype == 3 || (gametype == 2 && turn == BLACK) || (gametype == 1 && turn == WHITE))
	{
		cout << endl << "Computer " << turn <<"'s turn: " << endl;

		//prune goes in here and check time
		int depth = 1;
		int a = -1*POSINF;
		int b = POSINF;
		int hval;

		TileState cplayer = turn;
		movesleft = movesLeft();
		gettimeofday(&start,NULL);
		gettimeofday(&end,NULL);
		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0);

		while (1) {
				//figure out what moven we should chose
				if (movemax == 1) {
					moven = 1;
					break;
				}
				hval = alphabeta(tiles, depth, a, b, turn, cplayer, &tempmoven,start,end);
				if (hval == NOHEURVAL || depth > movesleft) {
					break;
				}
				else {
					moven = tempmoven + 1;
					//cout << "At depth: " << depth << " move number: " << moven << " hval: " << hval << endl;
					depth++;
				}

		}

		gettimeofday(&end,NULL);
		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0);

		cout << "At depth: " << depth-1 << ", Selecting Move: " << moven << endl;
		cout << "Elapsed time: " << mtime << " milliseconds" <<endl;
	}
	return moven;
}
int Board::h1(TileState** b, TileState p)
{
	//ensure the heurstic is zero sum!
	int val = 0;
	for(int i = 0;i<8;i++) {
		for(int j=0;j<8;j++) {
			if (b[i][j] == p)
			{
				val++;
			}
			else if (b[i][j] != EMPTY)
			{
				val--;
			}
		}
	}
	val = val*10 + (rand() % 10);
	return val;
}
int Board::h(TileState** b, TileState player)
{
	int my_tiles = 0, opp_tiles = 0, my_front_tiles = 0, opp_front_tiles = 0, x, y;
	TileState *** mymoves;
	TileState *** oppmoves;
	int p = 0, c = 0, l = 0, m = 0, f = 0, d = 0;
	TileState opponent;
	opponent = changePiece(player);

	int X1[] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int Y1[] = {0, 1, 1, 1, 0, -1, -1, -1};
	int V[8][8];

	//assigning board values
	V[0][0] = V[0][7] = V[7][0] = V[7][7] = 25;
	V[0][1] = V[1][0] = V[0][6] = V[6][0] = V[1][7] = V[7][1] = V[6][7] = V[7][6] = -3;
	V[0][2] = V[2][0] = V[0][5] = V[5][0] = V[2][7] = V[7][2] = V[5][7] = V[7][5] = 11;
	V[0][3] = V[3][0] = V[0][4] = V[4][0] = V[3][7] = V[7][3] = V[4][7] = V[7][4] = 8;
	V[1][1] = V[6][6] = V[6][1] = V[1][6] = -10;
	V[1][2] = V[2][1] = V[1][5] = V[5][1] = V[2][6] = V[6][2] = V[5][6] = V[6][5] = -4;
	V[3][1] = V[1][3] = V[4][1] = V[1][4] = V[3][6] = V[6][3] = V[6][4] = V[4][6] = 1;
	V[2][2] = V[2][3] = V[2][4] = V[2][5] = V[3][2] = V[4][2] = V[5][2] = 2;
	V[5][5] = V[5][4] = V[5][3] = V[4][5] = V[3][5] = 2;
	V[3][3] = V[4][4] = V[3][4] = V[4][3] = -3;

	//V[0] = {20, -3, 11,  8,  8, 11, -3, 20};
	//V[1] = {-3, -7, -4,  1,  1, -4, -7, -3};
	//V[2] = {11, -4,  2,  2,  2,  2, -4, 11};
	//V[3] = {8,   1,  2, -3, -3,  2,  1,  8};
	//V[4] = {8,   1,  2, -3, -3,  2,  1,  8};
	//V[5] = {11, -4,  2,  2,  2,  2, -4, 11};
	//V[6] = {-3, -7, -4,  1,  1, -4, -7, -3};
	//V[7] = {20, -3, 11,  8,  8, 11, -3, 20};

	// Piece difference, frontier disks and disk squares
	for(int i=0; i<8; i++) {
		for(int j=0; j<8; j++)  {
			if (b[i][j] == player)  {
				d = d + V[i][j];
				my_tiles++;
			}
			else if (b[i][j] == opponent)  {
				d = d - V[i][j];
				opp_tiles++;
			}

			if(b[i][j] != EMPTY)   {
				for(int k=0; k<8; k++)  {
					x = i + X1[k];
					y = j + Y1[k];
					if(x >= 0 && x < 8 && y >= 0 && y < 8 && b[x][y] == EMPTY) {
						if(b[i][j] == player)  {
							my_front_tiles++;
						}
						else {
							opp_front_tiles++;
						}
						break;
					}
				}
			}
		}
	}
	d = d*100; //10 originally
	p = 100 * (my_tiles - opp_tiles);
	// cout << "spot weighting value: " << d <<endl;
	//cout << "more pieces for board value: " << p << endl;

	if(my_front_tiles > opp_front_tiles) { //7440 front tiles
		f = -(100 * my_front_tiles)/(my_front_tiles + opp_front_tiles);
	}
	else if(my_front_tiles < opp_front_tiles) {
		f = (100 * opp_front_tiles)/(my_front_tiles + opp_front_tiles);
	}
	else {
		f = 0;
	}
	//cout << "front tiles for board: " << f << endl;

	// Corner occupancy
	my_tiles = opp_tiles = 0;
	if(b[0][0] == player) {
		my_tiles++;
	}
	else if(b[0][0] == opponent) {
		opp_tiles++;
	}
	if(b[0][7] == player) {
		my_tiles++;
	}
	else if(b[0][7] == opponent) {
		opp_tiles++;
	}
	if(b[7][0] == player) {
		my_tiles++;
	}
	else if(b[7][0] == opponent) {
		opp_tiles++;
	}
	if(b[7][7] == player) {
		my_tiles++;
	}
	else if(b[7][7] == opponent) {
		opp_tiles++;
	}
	//20000
	c = 500 * (my_tiles - opp_tiles);
	//cout << "corner value: " << c <<endl;

	// Corner closeness
	my_tiles = opp_tiles = 0;
	if(b[0][0] == EMPTY)   {
		if(b[0][1] == player) {
			my_tiles++;
		}
		else if(b[0][1] == opponent) {
			opp_tiles++;
		}
		if(b[1][1] == player) {
			my_tiles++;
		}
		else if(b[1][1] == opponent) {
			opp_tiles++;
		}
		if(b[1][0] == player) {
			my_tiles++;
		}
		else if(b[1][0] == opponent) {
			opp_tiles++;
		}
	}
	if(b[0][7] == EMPTY)   {
		if(b[0][6] == player) {
			my_tiles++;
		}
		else if(b[0][6] == opponent) {
			opp_tiles++;
		}
		if(b[1][6] == player) {
			my_tiles++;
		}
		else if(b[1][6] == opponent) {
			opp_tiles++;
		}
		if(b[1][7] == player) {
			my_tiles++;
		}
		else if(b[1][7] == opponent) {
			opp_tiles++;
		}
	}
	if(b[7][0] == EMPTY)   {
		if(b[7][1] == player) {
			my_tiles++;
		}
		else if(b[7][1] == opponent){
			opp_tiles++;
		}
		if(b[6][1] == player) {
			my_tiles++;
		}
		else if(b[6][1] == opponent) {
			opp_tiles++;
		}
		if(b[6][0] == player) {
			my_tiles++;
		}
		else if(b[6][0] == opponent) {
			opp_tiles++;
		}
	}
	if(b[7][7] == EMPTY)  {
		if(b[6][7] == player) {
			my_tiles++;
		}
		else if(b[6][7] == opponent) {
			opp_tiles++;
		}
		if(b[6][6] == player) {
			my_tiles++;
		}
		else if(b[6][6] == opponent) {
			opp_tiles++;
		}
		if(b[7][6] == player) {
			my_tiles++;
		}
		else if(b[7][6] == opponent) {
			opp_tiles++;
		}
	}
	//4770 closeness val
	l = -200 * (my_tiles - opp_tiles);
	//cout << "corner closeness value: " << l << endl;

	// Mobility check whose moves array we are looking at
	mymoves = AIMoves(b,player);
	oppmoves = AIMoves(b,opponent);
	my_tiles = moveCount(mymoves);
	opp_tiles = moveCount(oppmoves);

	for(int k = 0; k < my_tiles; ++k) {
		for (int l = 0;l<8;++l) {
			delete [] mymoves[k][l];
		}
		delete [] mymoves[k];
	}
	delete [] mymoves;

	for(int k = 0; k < opp_tiles; ++k) {
		for (int l = 0;l<8;++l) {
			delete [] oppmoves[k][l];
		}
		delete [] oppmoves[k];
	}
	delete [] oppmoves;

	if(my_tiles > opp_tiles) { //7890 coef
		m = 1000 * ((my_tiles)/(my_tiles + opp_tiles));
	}
	else if(my_tiles < opp_tiles) {
		m = -1000 * ((opp_tiles)/(my_tiles + opp_tiles));
	}
	else {
		m = 0;
	}
	//cout << "mobility value: " << m << endl;
	//impement stable pieces
	// final weighted score
	int randm = 0;
	randm = (rand() % 10) - 4 - (rand() % 2); // add randomness to change the game up
	int score = c + l + m + f + p + d + randm;
	return score;

}
int Board::h2(TileState** b, TileState player)
{
	TileState *** mymoves;
	TileState *** oppmoves;

	//mp is my pieces
	//op is opp pieces

	//mc is my corner
	//oc is opp corner

	//ml is my corner closeness
	//ol is opp corner closeness

	//mm is my mobility
	//om is opp mobility

	//ms is my stable
	//os is opp stable
	//d is board benefit

	int p, m, d, l, s, c;
	int mp, op, mc, oc, ml, ol, mm, om, ms, os;
	mp = op = mc = oc = ml = ol = mm = om = ms = os = d = 0;
	TileState opponent;
	TileState stable[8][8];
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			stable[i][j] = EMPTY;
		}
	}
	opponent = changePiece(player);

	int V[8][8];
	//assigning board values
	V[0][0] = V[0][7] = V[7][0] = V[7][7] = 20;
	V[0][1] = V[1][0] = V[0][6] = V[6][0] = V[1][7] = V[7][1] = V[6][7] = V[7][6] = -3;
	V[0][2] = V[2][0] = V[0][5] = V[5][0] = V[2][7] = V[7][2] = V[5][7] = V[7][5] = 11;
	V[0][3] = V[3][0] = V[0][4] = V[4][0] = V[3][7] = V[7][3] = V[4][7] = V[7][4] = 8;
	V[1][1] = V[6][6] = V[6][1] = V[1][6] = -7;
	V[1][2] = V[2][1] = V[1][5] = V[5][1] = V[2][6] = V[6][2] = V[5][6] = V[6][5] = -4;
	V[3][1] = V[1][3] = V[4][1] = V[1][4] = V[3][6] = V[6][3] = V[6][4] = V[4][6] = 1;
	V[2][2] = V[2][3] = V[2][4] = V[2][5] = V[3][2] = V[4][2] = V[5][2] = 2;
	V[5][5] = V[5][4] = V[5][3] = V[4][5] = V[3][5] = 2;
	V[3][3] = V[4][4] = V[3][4] = V[4][3] = -3;

	//V[0] = {20, -3, 11,  8,  8, 11, -3, 20};
	//V[1] = {-3, -7, -4,  1,  1, -4, -7, -3};
	//V[2] = {11, -4,  2,  2,  2,  2, -4, 11};
	//V[3] = {8,   1,  2, -3, -3,  2,  1,  8};
	//V[4] = {8,   1,  2, -3, -3,  2,  1,  8};
	//V[5] = {11, -4,  2,  2,  2,  2, -4, 11};
	//V[6] = {-3, -7, -4,  1,  1, -4, -7, -3};
	//V[7] = {20, -3, 11,  8,  8, 11, -3, 20};

	// Piece difference, frontier disks and disk squares
	for(int i=0; i<8; i++) {
		for(int j=0; j<8; j++)  {
			if (b[i][j] == player)  {
				d = d + V[i][j];
				mp++;
			}
			else if (b[i][j] == opponent)  {
				d = d - V[i][j];
				op++;
			}
		}
	}

	// Corner occupancy
	int maxcol;
	if(b[0][0] == player) {
		maxcol = 7;
		mc++;
		for(int i = 0; i < 8; i++) {
			if (b[i][0] != player) {
				break;
			}
			for (int j = 0; j < maxcol + 1;j++) {
				if (b[i][j] == player) {
					stable[i][j] = player;
				}
				else {
					maxcol = min(maxcol,j - 1);
					break;
				}
			}
		}
	}
	else if(b[0][0] == opponent) {
		maxcol = 7;
		oc++;
		for(int i = 0; i < 8; i++) {
			if (b[i][0] != opponent) {
				break;
			}
			for (int j = 0; j < maxcol + 1;j++) {
				if (b[i][j] == opponent) {
					stable[i][j] = opponent;
				}
				else {
					maxcol = min(maxcol,j - 1);
					break;
				}
			}
		}
	}
	if(b[0][7] == player) {
		maxcol = 0;
		mc++;
		for(int i = 0; i < 8; i++) {
			if (b[i][7] != player) {
				break;
			}
			for (int j = 7; j > maxcol-1;j--) {
				if (b[i][j] == player) {
					stable[i][j] = player;
				}
				else {
					maxcol = max(maxcol,j + 1);
					break;
				}
			}
		}
	}
	else if(b[0][7] == opponent) {
		maxcol = 0;
		oc++;
		for(int i = 0; i < 8; i++) {
			if (b[i][7] != opponent) {
				break;
			}
			for (int j = 7; j > maxcol-1; j--) {
				if (b[i][j] == opponent) {
					stable[i][j] = opponent;
				}
				else {
					maxcol = max(maxcol,j + 1);
					break;
				}
			}
		}

	}
	if(b[7][0] == player) {
		maxcol = 7;
		mc++;
		for(int i = 7; i > -1; i--) {
			if (b[i][0] != player) {
				break;
			}
			for (int j = 0; j < maxcol+1;j++) {
				if (b[i][j] == player) {
					stable[i][j] = player;
				}
				else {
					maxcol = min(maxcol,j - 1);
					break;
				}
			}
		}
	}
	else if(b[7][0] == opponent) {
		maxcol = 7;
		oc++;
		for(int i = 7; i > -1; i--) {
			if (b[i][0] != opponent) {
				break;
			}
			for (int j = 0; j < maxcol+1;j++) {
				if (b[i][j] == opponent) {
					stable[i][j] = opponent;
				}
				else {
					maxcol = min(maxcol, j- 1);
					break;
				}
			}
		}
	}
	if(b[7][7] == player) {
		maxcol = 0;
		mc++;
		for(int i = 7; i > -1; i--) {
			if (b[i][7] != player) {
				break;
			}
			for (int j = 7; j > maxcol-1;j--) {
				if (b[i][j] == player) {
					stable[i][j] = player;
				}
				else {
					maxcol= max(maxcol,j+1);
					break;
				}
			}
		}
	}
	else if(b[7][7] == opponent) {
		maxcol = 0;
		oc++;
		for(int i = 7; i > -1; i--) {
			if (b[i][7] != opponent) {
				break;
			}
			for (int j = 7; j > maxcol-1;j--) {
				if (b[i][j] == opponent) {
					stable[i][j] = opponent;
				}
				else {
					maxcol = max(maxcol,j+1);
					break;
				}
			}
		}
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (stable[i][j] == player) {
				ms++;
			}
			else if (stable[i][j] == opponent) {
				os++;
			}
		}
	}

	// Corner closeness
	if(b[0][0] == EMPTY)   {
		if(b[0][1] == player) {
			ml++;
		}
		else if(b[0][1] == opponent) {
			ol++;
		}
		if(b[1][1] == player) {
			ml++;
		}
		else if(b[1][1] == opponent) {
			ol++;
		}
		if(b[1][0] == player) {
			ml++;
		}
		else if(b[1][0] == opponent) {
			ol++;
		}
	}
	if(b[0][7] == EMPTY)   {
		if(b[0][6] == player) {
			ml++;
		}
		else if(b[0][6] == opponent) {
			ol++;
		}
		if(b[1][6] == player) {
			ml++;
		}
		else if(b[1][6] == opponent) {
			ol++;
		}
		if(b[1][7] == player) {
			ml++;
		}
		else if(b[1][7] == opponent) {
			ol++;
		}
	}
	if(b[7][0] == EMPTY)   {
		if(b[7][1] == player) {
			ml++;
		}
		else if(b[7][1] == opponent){
			ol++;
		}
		if(b[6][1] == player) {
			ml++;
		}
		else if(b[6][1] == opponent) {
			ol++;
		}
		if(b[6][0] == player) {
			ml++;
		}
		else if(b[6][0] == opponent) {
			ol++;
		}
	}
	if(b[7][7] == EMPTY)  {
		if(b[6][7] == player) {
			ml++;
		}
		else if(b[6][7] == opponent) {
			ol++;
		}
		if(b[6][6] == player) {
			ml++;
		}
		else if(b[6][6] == opponent) {
			ol++;
		}
		if(b[7][6] == player) {
			ml++;
		}
		else if(b[7][6] == opponent) {
			ol++;
		}
	}

	// Mobility check whose moves array we are looking at
	mymoves = AIMoves(b,player);
	oppmoves = AIMoves(b,opponent);
	mm = moveCount(mymoves);
	om = moveCount(oppmoves);

	for(int k = 0; k < mm; ++k) {
		for (int l = 0;l<8;++l) {
			delete [] mymoves[k][l];
		}
		delete [] mymoves[k];
	}
	delete [] mymoves;

	for(int k = 0; k < om; ++k) {
		for (int l = 0;l<8;++l) {
			delete [] oppmoves[k][l];
		}
		delete [] oppmoves[k];
	}
	delete [] oppmoves;

	int randm = 0;
	randm = (rand() % 10) - 4 - (rand() % 2); // add randomness to change the game up

	// final weighted score 633600 is the score for a perfect game
	/*
	mp = mp*900;
	op = op*900;
	ms = ms*900;
	os = os*900;
	mc = mc*14400;
	oc = oc*14400;
	mm = mm*1800;
	om = om*1800;
	ml = ml*4800;
	ol = ol*4800;
	d = d*160;
	*/
	int score = 0;
	if ((mp - op) > 0) {
		p = 10000*(mp - op)/(mp+op);
		if (p == 10000)
		{
			score = score + 600000000;
		}
	}
	else if ((mp - op) < 0) {
		p = -10000*(op - mp)/(mp+op);
		if (p == -10000)
		{
			score = score - 600000000;
		}
	}
	else {
		p = 0;
	}

	if ((ms - os) > 0) {
		s = 10000*(ms - os)/(ms+os);
		if (ms > 32)
		{
			score = score + 600000000;
		}
	}
	else if ((ms - os) < 0) {
		s = -10000*(os - ms)/(ms+os);
		if (os > 32)
		{
			score = score - 600000000;
		}
	}
	else {
		s = 0;
	}

	if ((mc - oc) > 0) {
		c = 160000*(mc - oc)/(mc+oc);
	}
	else if ((mc - oc) < 0) {
		c = -160000*(oc - mc)/(mc+oc);
	}
	else {
		c = 0;
	}

	//reveresed for closeness
	if ((ml - ol) > 0) {
		l = -50000*(ml - ol)/(ml+ol);
	}
	else if ((ml - ol) < 0) {
		l = 50000*(ol - ml)/(ml+ol);
	}
	else {
		l = 0;
	}

	if ((mm - om) > 0) {
		m = 20000*(mm - om)/(mm+om);
	}
	else if ((mm - om) < 0) {
		m = -20000*(om - mm)/(mm+om);
	}
	else {
		m = 0;
	}
	d = d*1700;

	score = 10*p + 2000*s + 2000*c + 200*m + 400*l + 10*d + randm;

	// add no move bonus
	if (om == 0) {
		score = score + 2000*160000;
	}
  if (mm == 0) {
		score = score - 2000*160000;
	}
	//if both have no moves then they cancel


	//cout heurstic block
	/*
	cout <<endl;
	cout << "Player " << player << " has " << mp << " pieces taken" <<endl;
	cout << "Player " << opponent << " has " << op << " pieces taken" <<endl;
	cout << "Player " << player << " has " << mc << " corners" <<endl;
	cout << "Player " << opponent << " has " << oc << " corners" <<endl;
	cout << "Player " << player << " has " << ms << " stable pieces"<<endl;
	cout << "Player " << opponent << " has " << os << " stable pieces"<<endl;
	cout << "Player " << player << " has " << mm << " moves to play" <<endl;
	cout << "Player " << opponent << " has " << om << " moves to play"<<endl;
	cout << "Player " << player << " has " << ml << " corner closeness pieces"<<endl;
	cout << "Player " << opponent << " has " << ol << " corner closness pieces"<<endl;
	cout << "Random Number Generated: " << randm <<endl;
	cout << "Board Value Sum for player " << player << " is: " << d <<endl;
	cout << "Final Score: " << score << endl;
	*/

	return score;

}
