#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>
#include "OthelloClass.h"

using namespace std;

int main() {
	Board b1;
	int gametype;
	int movemax = 0, noMoveCount = 0, moven;
	TileState*** mymoves;
	TileState** newb;

	//initial functions
	srand(time(NULL));
	gametype = b1.init();
	newb = b1.initBoard();
	b1.setBoard(newb);
	b1.display2(b1.tiles,(b1.turn));
	b1.h2(b1.tiles,b1.turn);
/*
	int hval;
	hval =b1.h2(b1.tiles,b1.turn);
	cout << "Black Heuristic: " << hval <<endl;;
	b1.turn = b1.changePiece(b1.turn);
	hval =b1.h2(b1.tiles,b1.turn);
	cout << "White Heuristic: " << hval <<endl;
	b1.state = 1;
*/

	while(b1.state != 1) {

		mymoves = b1.legalMoves((b1.tiles),b1.turn);
		movemax = b1.moveCount(mymoves);

		if (movemax != 0) {
			moven = b1.moveSelect(gametype, movemax);
			newb = mymoves[moven-1];
			b1.setBoard(newb);
			b1.display2(b1.tiles,b1.changePiece(b1.turn));
			//b1.h2(b1.tiles,b1.changePiece(b1.turn));
			/*
			hval =b1.h2(b1.tiles,b1.turn);
			cout << "Black Heuristic: " << hval <<endl;;
			b1.turn = b1.changePiece(b1.turn);
			hval =b1.h2(b1.tiles,b1.turn);
			cout << "White Heuristic: " << hval <<endl;
			b1.turn = b1.changePiece(b1.turn);
			*/

			noMoveCount = 0;
			cout << endl;
		}
		else {
			noMoveCount++;
			if (noMoveCount >= 2)
			{
				b1.setState(1);
			}
			if (noMoveCount == 1)
			{
				cout << "Player " << b1.turn << " has no moves." << endl;
				cout << "Skipping Turn: it is now Player " << b1.changePiece(b1.turn) << "'s turn" <<endl;
				b1.display2(b1.tiles,b1.changePiece(b1.turn));
			}
		}
		b1.switchTurn();
		for(int k = 0; k < movemax; ++k) {
			for (int l = 0;l<8;++l) {
				delete [] mymoves[k][l];
			}
			delete [] mymoves[k];
		}
		delete [] mymoves;

	}
	b1.winCheck(b1.tiles);
	cout << endl;
	return 0;
}
