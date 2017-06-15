
#include "Match.h"


struct ResultData {
	int wins			= 1;
	int losses			= 1;
	int pointsFor		= 1;
	int pointsAgnst		= 1;
	double winRate		= 1;	//winrate will only be calcualted after 1 match atlist was played so no need to warry about divByZero errors	 //% = Wins/(Wins+Losses)
	//בתיקו שני השחקנים לא מקבלים הפסד ולא ניצחון, אך סך הנקודות שצברו ושספגו כן נספר 
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
	mutex wrMutex;
	vector<ResultData> ScoreLine;
	vector<bool> ScoreLineReady;
	TeamScoreLine() {};	//default constructor

	void writeNewScore(ResultData score) {
		wrMutex.lock();
		ScoreLine.push_back(score);
		ScoreLineReady.push_back(true);
		wrMutex.unlock();
	}

	ResultData readRoundScore(int roundNumber) {
		if (canReadRound(roundNumber)){	//writing the score happans before the ready, so if ready exists and is true then score is ready.
			return ScoreLine[roundNumber];
		}
		//if cant read
		return ResultData();	//to avoid this return , call canReadRound first to be sure you read valid data
	}

	bool canReadRound(int roundNumber) {
		if (ScoreLineReady[roundNumber] != NULL && ScoreLineReady[roundNumber]) {
			return true;
		} else {
			return false;
		}
	}
	
};

struct MatchData {
	string pathA, pathB, pathBoard;
	MatchData() {};	//default constructor to make it simpler to work with MatchData vectors
	MatchData(string pathA_, string pathB_, string pathBoard_) : pathA(pathA_), pathB(pathB_), pathBoard(pathBoard_) {};
};

class ThreadManager {
private:
	size_t numberOfThreads;
	vector<string> algoPaths;
	vector<string> boardPaths;
	vector<thread> vec_threads;
	size_t numDays;
	map<string, ResultData> scoreBoardSum;

public:
	atomic<int> currTask = 0;
	vector<string> teams;
	vector<MatchData> tasks;
	map<string, TeamScoreLine> scoreBoard;
	mutex scoreBoardMutex;
	size_t numOfTasks;

	bool canReadRound(int roundNumber) {
		if (scoreBoard.empty())
			return false;
		for (std::map<string, TeamScoreLine>::iterator it = scoreBoard.begin(); it != scoreBoard.end(); ++it) {
			if (!it->second.canReadRound(roundNumber)) {
				return false;
			}
		}
		return true;
	}

	// caller of this function must first call canReadRound to be sure the round resuals are ready
	void margeScores(int roundNumber){
		for (std::map<string, TeamScoreLine>::iterator it = scoreBoard.begin(); it != scoreBoard.end(); ++it) {
			string teamName = it->first;
			scoreBoardSum[teamName] = scoreBoardSum[teamName] + it->second.readRoundScore(roundNumber);
		}
	}

	ThreadManager(vector<string> algoPaths_, vector<string> boardPaths_, int NumberOfThreads_) : algoPaths(algoPaths_), boardPaths(boardPaths_), numberOfThreads(NumberOfThreads_), vec_threads(numberOfThreads) {
		//cout << "in ThreadManager constructor\n";
		numOfTasks = algoPaths.size() * (algoPaths.size() - 1) * boardPaths.size();
		//cout << "expected number of tasks : " << numOfTasks << "\n";
		//tasks.resize(numOfTasks);
		//make the task pool
		
		//cout << "on Board " << *boardItr << " \n";
		if (algoPaths.size() % 2 != 0) {
			algoPaths.push_back("dummy"); // If odd number of teams add a dummy
		}
		size_t numTeams = algoPaths.size();
		numDays = (numTeams - 1); // Days needed to complete tournament
		size_t halfSize = numTeams / 2;

		for (vector<string>::iterator teamsItr = algoPaths.begin(); teamsItr != algoPaths.end(); ++teamsItr) {
			if (teamsItr == algoPaths.end()-1) {
				continue;
			} else {
				teams.push_back(*teamsItr);
			}
		}

		size_t teamsSize = teams.size();
		//cout << "teams:\n";
		//for (vector<string>::iterator teamsItr = teams.begin(); teamsItr != teams.end(); ++teamsItr) {
		//	cout << "<" << *teamsItr << ">" << " , ";
		//}
		//cout << "\n";
		//std::cin.ignore();

		vector<MatchData> tempTasks;
		for (vector<string>::iterator boardItr = boardPaths.begin(); boardItr != boardPaths.end(); ++boardItr) {
			for (int day = 0; day < numDays; day++) {
				//			Console.WriteLine("Day {0}", (day + 1));
				int teamIdx = day % teamsSize;

				//			Console.WriteLine("{0} vs {1}", teams[teamIdx], ListTeam[0]);

				for (int idx = 1; idx < halfSize; idx++) {
					size_t firstTeam = (day + idx) % teamsSize;
					size_t secondTeam = (day + teamsSize - idx) % teamsSize;
					tempTasks.push_back(MatchData(teams[firstTeam], teams[secondTeam], *boardItr));
					tasks.push_back(MatchData(teams[firstTeam], teams[secondTeam], *boardItr));
					//Console.WriteLine("{0} vs {1}", teams[firstTeam], teams[secondTeam]);
				}
			}
		}
		//
		//now make the switched sides:
		for (vector<MatchData>::iterator tasksItr = tempTasks.begin(); tasksItr != tempTasks.end(); ++tasksItr) {
			auto item = *tasksItr;
			tasks.push_back(MatchData(item.pathB, item.pathA, item.pathBoard));
			//cout << item.pathA << " vs " << item.pathB << " , on " << item.pathBoard << "\n";
		}

		//cout << "printing tasks:\n";
		//for (vector<MatchData>::iterator tasksItr = tasks.begin(); tasksItr != tasks.end(); ++tasksItr) {
		//	auto item = *tasksItr;
		//	cout << item.pathA << " vs " << item.pathB << " , on " << item.pathBoard << "\n";
		//}
		//cout << "Done " << tasks.size() << "  \n";
		//std::cin.ignore();

	}

	void printTeam(int place, string teamName) {
		printf("%d.\t%100s%8d%8d\t%3.2f\t%8d\t%8d", place, teamName.c_str(), scoreBoardSum[teamName].wins, scoreBoardSum[teamName].losses, scoreBoardSum[teamName].winRate, scoreBoardSum[teamName].pointsFor, scoreBoardSum[teamName].pointsAgnst);
	}

	void runThreads() {
		int id = 1;
		//run threads
		for (auto & t : vec_threads) {
			t = thread(&ThreadManager::ThreadTaskRunning, this, id);
			++id;
		}
		//manage the scoreboard
		int waitngForRound = 0;
		
		while (waitngForRound < numDays) {
			if ( canReadRound(waitngForRound) ){	//round <waitngForRound> is done and we can read/print resualts
				margeScores(waitngForRound);	//first marge the scores to the sum board
				map<double, string> sortingByWinRate;	//create new map to sort the table by winRate
				
				//map<string, ResultData> scoreBoardSum;
				for (std::map<string, ResultData>::iterator it = scoreBoardSum.begin(); it != scoreBoardSum.end(); ++it) {
					sortingByWinRate.insert(pair<double, string>(it->second.winRate, it->first));
				}
				// now go over the sortingByWinRate and print the scores
				int place = 1;
				cout << "Round " << waitngForRound << " \n";
				printf("#\t%100sWins\tLosses\t%%\t\tPts For  Pts Against\n", "Team Name");
				for (std::map<double, string>::iterator it = sortingByWinRate.begin(); it != sortingByWinRate.end(); ++it) {
					printTeam(place, it->second);
					place += 1;
				}
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
			int taskToRun = atomic_fetch_add(&currTask, 1);
			if (taskToRun >= numOfTasks) {
				return;
			}
			//else run task[taskToRun]
			auto coming_match = tasks[taskToRun];
			pair<int, int> gameScore = run_thread_match(coming_match.pathA, coming_match.pathB, coming_match.pathBoard);
			cout << gameScore.first << " / " << gameScore.second << endl;
			//TODO : switch from full path to name when reporting scores
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
			scoreBoard[coming_match.pathA].writeNewScore(AResultData);	//write players A score to the board
			scoreBoard[coming_match.pathB].writeNewScore(BResultData);	//write players B score to the board

			// done with this game , Ready to pick next task
		}


		cout << "thread " << id << " finished\n";
	}


	pair<int, int> run_thread_match(string playerAPath, string playerBPath, string boardPath) {
		Match A(playerAPath, playerBPath, boardPath);
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
	if (algoPaths_.size() < 3)
		return ERR_NUM_OF_ALGO;
	//TODO: send boards and not paths
	vector<string> boardPaths_ = get_all_files_names_within_folder(path, "sboard");
	ThreadManager tm( algoPaths_, boardPaths_,num_of_threads);
	tm.runThreads();	
	system("pause");
}
