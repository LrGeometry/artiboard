#pragma once
#include "../experiment/experiment.h"
#include "checkers.h"

namespace experiments {
	class FairnessExperiment: public Experiment {
		public:
			FairnessExperiment();
		protected:
			void doRun();
		private:
			void runAndDelete(game::IGame * game);
	};

}
