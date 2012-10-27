#include "experimentfairness.h"
#include <iostream>
#include "../player/player.h"
#include "../systemex/log.h"

namespace experiments {
	FairnessExperiment::FairnessExperiment() : Experiment("010", "010 Fairness Tests") {
	}


	void FairnessExperiment::doRun() {
		file() << "game sampleSize wins losses";
		runAndDelete(new checkers::CheckersGame(false,false));
		runAndDelete(new checkers::CheckersGame(false,true));
		runAndDelete(new checkers::CheckersGame(true,false));
		runAndDelete(new checkers::CheckersGame(true,true));
	}

	/**
	 * Initial benchmark is about 1000 games per second
	 */
	void FairnessExperiment::runAndDelete(game::IGame * game) {
		auto player1 = new player::RandomPlayer(1L);
		auto player2 = new player::RandomPlayer(2L);
		int runCount = 40;
		int maxSamples = 1000;
		int count = 0;
		for (int run = 0; run < runCount; run++) {
			int wins = 0;
			int losses = 0;
			int i = 0;
			while (wins + losses < maxSamples) {
				game->start();
				auto result = game->compete(player1, player2);
				if (result->outcome() == game::PlayLine::ocWin)
					wins++;
				else if (result->outcome() == game::PlayLine::ocLose)
					losses++;
				delete result;
				int tot = wins+losses;
				if ((tot > 0) & (tot % 1000 == 0))
					file() << game->description() << " " << (i + 1) << " " << wins << " " << losses;
				i++;
				count++;
			}
			LOG << run * 100.0f / runCount << "%";
		}
		LOG << count << " games played";
		delete game;
	}


	FairnessExperiment ex1; // instantiate experiment to register it

	}
