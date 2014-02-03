#include <experiment.h>
#include "tictactoe.h"
using namespace arti;


class MinimaxExperiment : public Experiment {
public:
	MinimaxExperiment() : Experiment("t3-100","Create the minimax tree for tic-tac-toe") {}
protected:
	void doRun() override	{
		// TODO implement this experiment
	}
};

static MinimaxExperiment minimax;