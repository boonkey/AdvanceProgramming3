#include "Match.h"

//constr
	Match::Match(string playerAPath,string playerBPath,Board board) : matchBoard(board){
		if (!initplayer(playerAPath,true, matchBoard.getSidedBoard(true))) {
			playerA = NULL;
		}
		if (!initplayer(playerBPath, false, matchBoard.getSidedBoard(false))) {
			playerB = NULL;
		}	
	}

//other functions

	pair<int, int> Match::runMatch() {
		if (playerA == NULL || playerB == NULL) {
			return pair<int, int>(-1, -1);
		}

		// else both players are ready and we can play
		bool stillPlayingA = true;
		bool stillPlayingB = true;

		//true for A , false for B
		int isTurnB = 0;

		while (true) {
			//system("pause");
			//cout << "==============================" << endl;
			//cout << "A " << scoreA << " playing? " << stillPlayingA << endl;
			//cout << "B " << scoreB << " playing? " << stillPlayingB << endl;

			//check if game ended 
			if (matchBoard.gameOver(true) || matchBoard.gameOver(false)) {
				stillPlayingA = false;
				stillPlayingB = false;
			}

			if ((stillPlayingA == false) && (stillPlayingB == false)) {	//both players stoped playing (game ended or no more moves
				return pair<int, int>(scoreA,scoreB);
			}

			int turnResult = do_turn(matchBoard, isTurnB);
			if (turnResult == -1 && isTurnB) {
				stillPlayingB = false;
				isTurnB = 0;
			} else if (turnResult == -1 && !isTurnB) {
				stillPlayingA = false;
				isTurnB = 1;
			} else
				isTurnB = turnResult;
		}
	}

	bool Match::initplayer(string dllPath, bool isA,const Board& sidedboard) {
		HINSTANCE dllLib = LoadLibraryA(dllPath.c_str());
		if (!dllLib) {
			std::cout << "could not load the dynamic library" << std::endl;
			return false;
		}

		auto loadedAlgo = (GetAlgo)GetProcAddress(dllLib, "GetAlgorithm");
				
		if (!loadedAlgo) {
			return false;	
		}

		IBattleshipGameAlgo* loadedPlayer = loadedAlgo();
		loadedPlayer->setBoard(sidedboard);
		if (isA) {
			loadedPlayer->setPlayer(0);
			playerA = unique_ptr<IBattleshipGameAlgo>(move(loadedPlayer));
		}else {
			loadedPlayer->setPlayer(1);
			playerB = unique_ptr<IBattleshipGameAlgo>(move(loadedPlayer));
		}
		return true;
	}

	int Match::do_turn(Board &mainGameBoard, int isTurnB) {
		Coordinate thisTurnAttack = isTurnB? playerB->attack() : playerA->attack();

		if ((thisTurnAttack.row == -1) && (thisTurnAttack.col == -1) && (thisTurnAttack.depth == -1) ) {	//  no moves left and did not attack
			return -1;
		} else {	//attack was made -> process and notify players
			if ( (thisTurnAttack.row >= 1) && (thisTurnAttack.row <= mainGameBoard.rows()) &&
				 (thisTurnAttack.col >= 1) && (thisTurnAttack.col <= mainGameBoard.cols()) &&
				 (thisTurnAttack.depth >= 1) && (thisTurnAttack.depth <= mainGameBoard.depth()) ) {	//valid attack possition

				
				for (size_t i = 0; i < mainGameBoard.ships.size(); i++) {	// go over all ships, check if one was hit
					Ship &ship = mainGameBoard.ships[i];
					if (ship.checkAttack(thisTurnAttack)) {	//if hit was made
						if (ship.checkAlive() == false) { //if ship sunk
														  //let the players know a ship was sunk
							playerA->notifyOnAttackResult(isTurnB, thisTurnAttack, AttackResult::Sink);
							playerB->notifyOnAttackResult(isTurnB, thisTurnAttack, AttackResult::Sink);
							if (ship.isSideA() == false) {
								scoreA += ship.getShipScore();
								return 0;
							} else {
								scoreB += ship.getShipScore();
								return 1;
							}
						} else { //hit made but ship did not sank
								 //let the players know A hit a ship
							playerA->notifyOnAttackResult(isTurnB, thisTurnAttack, AttackResult::Hit);
							playerB->notifyOnAttackResult(isTurnB, thisTurnAttack, AttackResult::Hit);

							if (ship.isSideA() == false) {
								return 0;
							} else {
								return 1;
							}
						}
					}
				}
				//if we get here then no ship was hit
				//let the players know shot Missed
				playerA->notifyOnAttackResult(isTurnB, thisTurnAttack, AttackResult::Miss);
				playerB->notifyOnAttackResult(isTurnB, thisTurnAttack, AttackResult::Miss);
				return 1 - isTurnB;
			} else {	//invalid attack
				return  1 - isTurnB;
			}
		}
	}
