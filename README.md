# ServerEngine
Instruction for compiling:
	Place all four files (makefile, OthelloGame.cpp, OthelloClass.cpp, and OthelloClass.h) in the same folder and type “make” in the command line when in the directory where the files are located. Once compiled, type “./OthelloGame” to play the game where you will be prompted who is player 1 and who is player 2 how much time for the computer to play and whether or not you want a previous game state to input.
The Program:
For the Othello program the AI uses the minimax algorithm with Alpha-beta pruning to search the depths of the move tree. The depths will be searched starting from depth 1 and will continue incrementing by 1 until the allotted time expires. Once the time expires the program returns the selected move and the depth which it searched to select that move.
The User Interface:
	A player plays the program by entering the move number which they want. The moves are automatically listed for the current players turn. The user can see where the legal moves are on the board or in a list under the board for the current player. The computer will take up all of its allowed time unless there is only one move available or it has searched to the bottom of the move tree (usually towards the end of the game in Othello).
The Heuristic:
	The heuristic calculates the following parameters for both players and they are numbered by their importance in the final weighting:
				Parameter						          variable
1.	The amount of pieces currently on the board for each player				p
2.	The sum of weighted board spots (each board position is assigned a value)	 	d
3.	The amount of pieces which are boarding an unoccupied corner			l
4.	The amount of moves available for each player					m
5.	The amount of stable pieces on the board for each player			 	s
6.	The amount of corners occupied by each player				 	c
Each parameter is calculated by the ratio of the difference over the total of a given parameter (i.e for the amount of pieces a player has it is player (pieces – opponent pieces) / total pieces and if the value is negative then it is made positive). The final returned value is then calculated by a weighted sum of these values according to their importance. The randm is a value from -5 to 5 which randomizes the move selection.
value = 10*p + 2000*s + 2000*c + 200*m + 400*l + 10*d + randm;
Two bonuses are given to the heuristic value:
1.	win state is detected (33 stable pieces or all pieces are the current players pieces)  this adds a value greater than the best case scenario which is not a win state and negates this value if detects a loss
2.	Double move available (if opponent player does not have a move)  this adds a value equal to getting a corner and negates if detected current player does not have a move
After all of these calculation the heuristic value is returned.
