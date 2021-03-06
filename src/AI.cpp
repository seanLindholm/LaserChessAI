
#include <iostream>
#include <string.h>
#include <vector>
#include <iomanip>      // std::setprecision


#include "AI.h"
#include "Board.h"

using namespace std;

//AI constructor - Sets the colour of the computer player
AI::AI(bool isBlue) {
	this->isBlue = isBlue;
};

//AI utility - Evaluation function of the state of the game Board.
int AI::utility(Board state){
	int score;
	isBlue ?  score = (-1)*state.score : score =  (state.score);
	std::vector <piece*> turn;
	isBlue ? turn=state.BlueActive : turn=state.RedActive;
	return score;//+turn.size();
}


//Minimax algorithm function
int AI::miniMax(Board board, int depth, bool MaxPlayer){
	//Current value of board
	int score = utility(board);
	//Temporary vectors to keep track of which turn it is
	std::vector <piece*> turn;
	std::vector <piece*> notTurn;
	isBlue ? turn=board.BlueActive : turn=board.RedActive;
	isBlue ? notTurn=board.RedActive : notTurn=board.BlueActive;
	
	//Terminal test
	if(score > 900 || score < -900){
		return score;
	}
	//This is the cut-off depth to the search, the depth is +1
	//Because the first Max step is done in the find move function
	if (depth == 2){
		return score;
	}

	if(MaxPlayer){
		int bestValue = -1000;
		//For every move possible and for each piece
		//Piece
		for(int i=0; i<turn.size(); i++){
			//For each piece, total number of moves
			for(int j=0; j<10; j++){
				COUNT++;
				//don't try and turn the king(redundant move)
				if (!strcmp(typeid(turn[i][0]).name(),"4King") && j == 8){
					break;
				}
				//Copy of the current game state
				Board Temp_board = board;
				//Keeps track of current turn in the game, to search for new move
				isBlue ? Temp_board.Blue_turn=true : Temp_board.Blue_turn=false ;
				//Do the action
				int res = Temp_board.Do_action(i, j);
				//If result is -1, continue to next move
				if (res != 0){
					continue;
				}
				//Else we update and evaluate board state
				else{
					Temp_board.update_laser(false);
					//Recursive call to miniMax
					bestValue = max(bestValue, miniMax(Temp_board, depth+1, false));
					Temp_board.~Board();
				}
			}
		}
		return bestValue;
	}
	//If minimizing player
	else{
		int bestValue = 1000;
		//For every move possible - for each piece
		//Piece
		for(int i= 0; i < notTurn.size(); i++){
			//For each piece, total number of moves
			for(int j=0; j<10; j++){
				COUNT++;
				//Don't try and turn the king
				if (!strcmp(typeid(notTurn[i][0]).name(),"4King") && j == 8){
					break;
				}
				Board Temp_board = board;
				isBlue ? Temp_board.Blue_turn=false : Temp_board.Blue_turn=true;

				int res = Temp_board.Do_action(i, j);
				if (res != 0){
					continue;
				}else{
					Temp_board.update_laser(false);
					bestValue = min(bestValue, miniMax(Temp_board, depth+1, true));
					Temp_board.~Board();
				}
			}
		}
		return bestValue;
	}
}

//Essentially just the first MAX step in miniMax
//Made seperately to extract the move, which max the evaluation function
Move AI::findMove(Board board){
    int bestValue = -1000;
	Move bestMove;
	bestMove.piece = -1;
	bestMove.move = -1;
	std::vector <piece*> turn;
	isBlue ? turn=board.BlueActive : turn=board.RedActive;
	//Piece
	for(int i=0; i<turn.size(); i++){
		//Move
		for(int j=0; j<10; j++){
			COUNT++;

			//don't try and turn the king
			if (!strcmp(typeid(turn[i][0]).name(),"4King") && j == 8){
				break;
			}
			Board Temp_board = board;
			isBlue ? Temp_board.Blue_turn=true : Temp_board.Blue_turn=false;

			// Make the move
			int res = Temp_board.Do_action(i, j);
			if (res != 0){
				//Don't check for invalid moves
				continue;
			}
			else{
				Temp_board.update_laser(false);
				int minvalue = miniMax(Temp_board, 0, false);
				int value = max(bestValue, minvalue);
				Temp_board.~Board();

				//If the value is higher than the curren bestValue is is stored
				//And the current move that max the bestValue is stored in the Move struct
				if(value > bestValue){
					bestMove.piece = i;
					bestMove.move = j;
					bestValue = value;
				}
			}
		}

		cout << "PiecesChecked: " << i+1 << "/" << turn.size() << endl;
	}
	cout<<"Best value is "<<bestValue<<endl;
	//Best move is returned
	return bestMove;
}

int AI::findMove_AB_2(Board board, int depth, int alpha, int beta,bool MaxPlayer){
	COUNT++;
	std::vector <piece*> turn;
	std::vector <piece*> notTurn;
	isBlue ? turn=board.BlueActive : turn=board.RedActive;
	isBlue ? notTurn=board.RedActive : notTurn=board.BlueActive;
	int score = utility(board);

	if (depth == 0 || score > 900 || score < -900){
		return score;
	}
	if (MaxPlayer){
		int value = -2000;
		//for each possible action, find the new state.
		for (int i=0; i<turn.size(); i++){
			for (int j=0;j<10;j++){
				Board node = board;
				isBlue ? node.Blue_turn=true : node.Blue_turn=false ;

				if (!strcmp(typeid(turn[i][0]).name(),"4King") && j == 8){
					break;
				}
				int res = node.Do_action(i,j);
				if(res != 0){
					//Don't check invalid moves
					continue;
				}else{
					node.update_laser(false);
					value = max(value,findMove_AB_2(node,depth-1,alpha,beta,false));
					alpha = max(alpha,value);				
					if (bestValMax < value && depth == depth_cutoff){
						cout << "found new best" << endl;
						bestValMax = value;
						bestMove2.piece = i;
						bestMove2.move = j;
					}
					if (value >= beta){
						return value;
					}
				node.~Board();
				}
			}
		}
		return value;
	}else{
		int value = 2000;
		//for each possible action, find the new state.
		for (int i=0; i<notTurn.size(); i++){
			for (int j=0;j<10;j++){
				Board node = board;
				isBlue ? node.Blue_turn=false : node.Blue_turn=true ;
				if (!strcmp(typeid(notTurn[i][0]).name(),"4King") && j == 8){
					break;
				}
				int res = node.Do_action(i,j);
				if(res != 0){
					//Don't check invalid moves
					continue;
				}else{
					node.update_laser(false);
					value = min(value,findMove_AB_2(node,depth-1,alpha,beta,true));
					beta = min(beta,value);
					if (value<=alpha){
						return value;
					}
				node.~Board();
				}
			}
	}


		return value;
	}
}


