
#include "Match.h"


struct ResultData {
	int wins			= 0;
	int losses			= 0;
	int pointsFor		= 0;
	int pointsAgnst		= 0;
	double winRate		= 0;	//winrate will only be calcualted after 1 match atlist was played so no need to warry about divByZero errors	 //% = Wins/(Wins+Losses)
	
	ResultData() {};	//default constructor, with all 0
	ResultData(int win, int lose, int pFor, int pAg) : wins(win), losses(lose), pointsFor(pFor), pointsAgnst(pAg) {};

	ResultData operator+(ResultData A) {
		wins = wins + A.wins;
		losses = losses + A.losses;
		pointsFor = pointsFor + A.pointsFor;
		pointsAgnst = pointsAgnst + A.pointsAgnst;
		winRate = (wins + losses) > 0 ? wins / (wins + losses) : 0;
		return *this;
	}
};

//atomic<bool> ready = 0;	// update this at the end of a write to allow ready to know data is ready

//each ResultData object will have the resualt of a single match
//a mutex prevent multi writing confilcts
class TeamScoreLine {
public:
	/*mutex wrMutex;
	condition_variable wrCond;*/
	vector<ResultData> ScoreLine;
	vector<bool> ScoreLineReady;
	size_t numberOfMatchs;
	TeamScoreLine() { 
		numberOfMatchs = 0;
		ScoreLine.size();
		ScoreLineReady.size();
	};	//default empty constructor, used just for easy vecotr init
	TeamScoreLine(size_t numberOfMatchs_) {
		numberOfMatchs = numberOfMatchs_;
		for (unsigned int i = 0; i < numberOfMatchs; i++) {
			ScoreLine.push_back(ResultData());
			ScoreLineReady.push_back(false);
		}
	}	

	void writeNewScore(ResultData score) {
		//wrMutex.lock();
		//cout << "Sanity: " << numberOfMatchs << " @" << &numberOfMatchs << endl;
		unsigned int i = 0;
		while(i<numberOfMatchs){
			if (!ScoreLineReady[i]) {	//if score in round i is false then thats the round we need to put score in now.
				break;
			}
			i += 1;
		}		
		//cout << "Sanity II: " << score.losses << endl;
		ScoreLine.insert(ScoreLine.begin() + i,score);
		ScoreLineReady.insert(ScoreLineReady.begin() + i, true);
		//cout << "Sanity III" << endl;
		//wrMutex.unlock();
	}

	ResultData readRoundScore(int roundNumber) {
		if (canReadTeamRound(roundNumber)){	//writing the score happans before the ready, so if ready exists and is true then score is ready.
			return ScoreLine[roundNumber];
		}
		//if cant read
		return ResultData();	//to avoid this return , call canReadRound first to be sure you read valid data
	}

	bool canReadTeamRound(int roundNumber) {
		//cout << "entered canReadTeamRound "<< roundNumber << endl;
		if ((ScoreLineReady[roundNumber] != NULL) && (ScoreLineReady[roundNumber])) {
			//cout << "return true" << endl;
			return true;
		} else {
			return false;
		}
	}
	
};

struct MatchData {
	string pathA, pathB;
	Board board;
	MatchData(string pathA_, string pathB_, Board board_) : pathA(pathA_), pathB(pathB_), board(board_) {};
};

class ThreadManager {
private:
	size_t numberOfThreads;
	vector<string> algoPaths;
	vector<Board> boards;
	vector<thread> vec_threads;
	size_t numDays;
	map<string, ResultData> scoreBoardSum;

public:
	atomic<int> currTask = 0;
	vector<string> teams;
	vector<MatchData> tasks;
	mutex wrmutex;
	map<string, TeamScoreLine> scoreBoard;
	mutex scoreBoardMutex;
	condition_variable scoreBoardCond;
	size_t numOfTasks;

	bool canReadRound(int roundNumber) {
		//cout << "entered CanReadRound" << endl;
		wrmutex.lock();
		bool result;
		if (scoreBoard.empty())
			result = false;
		for (map<string, TeamScoreLine>::iterator it = scoreBoard.begin(); it != scoreBoard.end(); ++it) {
			if (!it->second.canReadTeamRound(roundNumber)) {
				result = false;
			}
		}
		result = true;
		wrmutex.unlock();
		return result;
	}

	// caller of this function must first call canReadRound to be sure the round resuals are ready
	void mergeScores(int roundNumber){
		for (map<string, TeamScoreLine>::iterator it = scoreBoard.begin(); it != scoreBoard.end(); ++it) {
			string teamName = it->first;
			scoreBoardSum[teamName] = (scoreBoardSum[teamName]) + (it->second.readRoundScore(roundNumber));
		}
	}

	ThreadManager(vector<string> algoPaths_, vector<Board> boards_, int NumberOfThreads_) : algoPaths(algoPaths_), boards(boards_), numberOfThreads(NumberOfThreads_), vec_threads(numberOfThreads) {
		cout << "in ThreadManager constructor with "<< algoPaths_.size() << " teams" << endl;
		numOfTasks = algoPaths.size() * (algoPaths.size() - 1) * boards.size();
		//cout << "expected number of tasks : " << numOfTasks << "\n";
		//tasks.resize(numOfTasks);
		//make the task pool
		for (auto &a : algoPaths)
			cout << a << endl;
		//cout << "on Board " << *boardItr << " \n";
		if (algoPaths.size() % 2 != 0) {
			algoPaths.push_back("dummy"); // If odd number of teams add a dummy
		}
		size_t numTeams = algoPaths.size();
		numDays = (numTeams - 1); // Days needed to complete tournament once
		size_t halfSize = (numTeams) / 2;

		for (vector<string>::iterator teamsItr = algoPaths.begin()+1; teamsItr != algoPaths.end(); ++teamsItr) {
			string name = *teamsItr;
			/*if (!strcmp(name.c_str(),"dummy")) {
				continue;*/
			//} else {
				teams.push_back(*teamsItr);
			//}
		}

		size_t teamsSize = teams.size();
		
		vector<MatchData> tempTasks;
		for (vector<Board>::iterator boardItr = boards.begin(); boardItr != boards.end(); ++boardItr) {
			for (unsigned int day = 0; day < numDays; day++) {
				int teamIdx = day % teamsSize;
				tasks.push_back(MatchData(algoPaths[0], teams[teamIdx], *boardItr));
				tempTasks.push_back(MatchData(algoPaths[0], teams[teamIdx], *boardItr));
				for (unsigned int idx = 1; idx < halfSize; idx++) {
					size_t firstTeam = (day + idx) % teamsSize;
					size_t secondTeam = (day + teamsSize - idx) % teamsSize;
					tempTasks.push_back(MatchData(teams[firstTeam], teams[secondTeam], *boardItr));
					tasks.push_back(MatchData(teams[firstTeam], teams[secondTeam], *boardItr));
				}
			}
		}

		//
		//now make the switched sides:
		for (vector<MatchData>::iterator tasksItr = tempTasks.begin(); tasksItr != tempTasks.end(); ++tasksItr) {
			auto item = *tasksItr;
			tasks.push_back(MatchData(item.pathB, item.pathA, item.board));
			//cout << item.pathA << " vs " << item.pathB << " , on " << item.pathBoard << "\n";
		}

		//TODO: remove dummmies from play
		
		//build the scoreboard
		cout << "init the scoreboards with " << algoPaths.size() << " teams:" << endl;
		for (unsigned int j = 0; j < numTeams; j++) {
			cout << "building for: " << algoPaths[j] << endl;
			//cout << "number of tasks: " << numOfTasks << endl;
			scoreBoard.insert(pair<string, TeamScoreLine>(algoPaths[j], TeamScoreLine(numOfTasks)));
			scoreBoardSum.insert(pair<string, ResultData>(algoPaths[j], ResultData()));
		}
		system("pause");
		cout << "printing tasks:\n";
		for (vector<MatchData>::iterator tasksItr = tasks.begin(); tasksItr != tasks.end(); ++tasksItr) {
			auto item = *tasksItr;
			cout << item.pathA.substr(item.pathA.find_last_of('\\') + 1).c_str() << " vs " << item.pathB.substr(item.pathB.find_last_of('\\') + 1).c_str() << endl;
		}
		cout << "Done " << tasks.size() << "  \n";
		system("pause");

	}

	void printTeam(int place, string teamName) {
		printf("%d.\t%50s %8d %8d \t%.2f %8d \t%8d\n", place, teamName.substr(teamName.find_last_of('\\') + 1).c_str(), scoreBoardSum[teamName].wins, scoreBoardSum[teamName].losses, scoreBoardSum[teamName].winRate, scoreBoardSum[teamName].pointsFor, scoreBoardSum[teamName].pointsAgnst);
	}

	void runThreads() {
		int id = 1;
		//run threads
		for (auto & t : vec_threads) {
			t = thread(&ThreadManager::ThreadTaskRunning, this, id);
			++id;
		}
		//manage the scoreboard
		unsigned int waitngForRound = 0;
		
		while (waitngForRound < numDays*2) {
			if ( canReadRound(waitngForRound) ){	//round <waitngForRound> is done and we can read/print resualts
				//cout << "ready to write round " << waitngForRound << " have " << scoreBoardSum.size() << " teams" << endl;
				mergeScores(waitngForRound);	//first marge the scores to the sum board
				vector<pair<double, string>> sortingByWinRate;	//create new map to sort the table by winRate
				//cout << "after marge have " << scoreBoardSum.size() << " teams" << endl;

				//map<string, ResultData> scoreBoardSum;
				for (std::map<string, ResultData>::iterator it = scoreBoardSum.begin(); it != scoreBoardSum.end(); ++it) {
					sortingByWinRate.push_back(pair<double, string>(it->second.winRate, it->first));
				}
				sort(sortingByWinRate.begin(), sortingByWinRate.end(), [](auto &left, auto &right) {
					return left.first < right.first;
				});
				cout << "Round " << waitngForRound + 1 << "/" << numDays * 2 << endl;
				printf("#\t%50s\tWins\tLosses\t  %%\tPts For  Pts Against\n", "Team Name");
				for (unsigned int p = 0; p < sortingByWinRate.size(); p++) {
					printTeam(p + 1, sortingByWinRate[p].second);
				}
				//cout << "### " << sortingByWinRate.size() << " ###" << endl;
				//// now go over the sortingByWinRate and print the scores
				//int place = 1;
			
				
				//for (std::map<double, string>::iterator it = sortingByWinRate.begin(); it != sortingByWinRate.end(); ++it) {
				//	printTeam(place, it->second);
				//	place += 1;
				//}
				cout << "\n\n";	
				waitngForRound += 1;
			}

		}

		//wait for all to end
		for (auto & t : vec_threads) {
			t.join();
		}
	}

	~ThreadManager() {};

	void ThreadTaskRunning(int id) {
		while (true) {
			unsigned int taskToRun = atomic_fetch_add(&currTask, 1);
			if (taskToRun >= numOfTasks) {
				return;
			}
			//cout << numOfTasks << endl;
			//else run task[taskToRun]
			auto coming_match = tasks[taskToRun];
			pair<int, int> gameScore = run_thread_match(coming_match.pathA, coming_match.pathB, coming_match.board);
			//cout << gameScore.first << " / " << gameScore.second << endl;
			//set the ResultData
			ResultData AResultData;
			ResultData BResultData;
			if (gameScore.first > gameScore.second) {			//A won
				AResultData = ResultData(1, 0, gameScore.first, gameScore.second);
				BResultData = ResultData(0, 1, gameScore.second, gameScore.first);
			} else if (gameScore.first < gameScore.second) {	//B won
				AResultData = ResultData(0, 1, gameScore.first, gameScore.second);
				BResultData = ResultData(1, 0, gameScore.second, gameScore.first);
			} else {	//tie score	- not sure how a game ends with tie but forum commented about this
				AResultData = ResultData(0, 0, gameScore.first, gameScore.second);
				BResultData = ResultData(0, 0, gameScore.second, gameScore.first);
			}
	
			wrmutex.lock();
		//	cout << coming_match.pathA << " | " << coming_match.pathB << endl;
		//	cout << "Adress: " << &AResultData << endl;
			scoreBoard[coming_match.pathA].writeNewScore(AResultData);	//write players A score to the board
			scoreBoard[coming_match.pathB].writeNewScore(BResultData);	//write players B score to the board
			wrmutex.unlock();
			// done with this game , Ready to pick next task
		}


		cout << "thread " << id << " finished\n";
	}


	pair<int, int> run_thread_match(string playerAPath, string playerBPath, Board board) {
		Match A(playerAPath, playerBPath, board);
		auto score = A.runMatch();
		return score;
	}

};



int main(int argc, char* argv[]) {
	string path = ".";
	int num_of_threads = 4;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--threads")) {
			if (argc > i + 1) {
				num_of_threads = atoi(argv[i + 1]);
				i++;
			}
			else
				return ERR_WRONG_NUM_OF_ARGS;
		}
		else
			path = argv[i];
	}
	cout << path << endl;
	system("pause");

	vector<string> algoPaths_ = get_all_files_names_within_folder(path, "dll");
	if (algoPaths_.size() < 2) {
		for (auto &a : algoPaths_)
			cout << a << endl;
		return ERR_NUM_OF_ALGO;
	}
	vector<string> boardPaths_ = get_all_files_names_within_folder(path, "sboard");
	vector<Board> boards;
	for (auto &b : boardPaths_) {
		boards.push_back(Board(b));
	}
	ThreadManager tm( algoPaths_, boards ,num_of_threads);
	tm.runThreads();	
	system("pause");
}
