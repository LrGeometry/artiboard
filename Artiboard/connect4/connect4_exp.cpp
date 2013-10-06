#include <experiment.h>
#include "connect4.h"

using namespace arti;

class FairnessExperiment: public Experiment {
	public:
		FairnessExperiment() : Experiment("c4-010","Connect-4 fairness") {}
	protected:
		void doRun() override{
			file() << "sampleSize south north";
			Connect4 spec;
			PickFirst picker;
			int south = 0;
			int north = 0;
			int draw = 0;
			int runs = 0;
			while (runs < 10000) {
				runs++;
				Match match(spec,picker);
				match.play();
				auto result = match.outcome();
				if (result == SouthPlayerWins)
					south++;
				else if (result == NorthPlayerWins)
					north++;
				else 
					draw++;
				if (runs % 1000 == 0) 
					file() << runs << " " << (north * 100) / runs  << " " << (south * 100) / runs;
			}
		}
};

// register experiments
FairnessExperiment ex1;