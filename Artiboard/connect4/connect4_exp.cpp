#include <experiment.h>
#include <feat.h>
#include "connect4.h"
#include "icu_data.h"
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

struct DataStat {
	DataStat() : wins(0), losses(0), draws(0) {};
	void inc(const IcuEntry& e) {
			if (e.outcome == NorthPlayerWins)
				wins++;
			else if (e.outcome == SouthPlayerWins)
				losses++;
			else
				draws++;		
	}

	int wins;
	int losses;
	int draws;
};

std::ostream& operator << (std::ostream& os, const DataStat& s) {
	os << " " << s.wins << " " << s.losses << " " << s.draws;
	return os;
} 

class DataStatistics: public Experiment {
public:
	DataStatistics(): Experiment("c4-020","Connect-4 ICU data") {}
	void doRun() override {
		file() << "ply wins losses draws";
		const IcuData& data = IcuData::instance();
		std::map<int,DataStat> stats;
		for_each(p,data) {
			int ply = ply_of(p->first);
			stats[ply].inc(p->second);
		}
		for_each(s,stats)
			file() << s->first << s->second;
		file() << data.size();
	}
};


class FeatureStatistics: public Experiment {
public:
	FeatureStatistics(): Experiment("c4-030","Connect-4 ICU data features") {}
	void doRun() override {
		const IcuData& data = IcuData::instance();
		file() << "region piece count wins losses draws";
		const std::vector<Piece> pieces({Piece('-'), Piece('o'), Piece('x')});
		auto program = load_program("../connect4/data/regions.txt");
		for_each(nr,program->regions()) {
			auto n = nr->first;
			auto r = nr->second;
			for_each(p,pieces) {
				std::map<int,DataStat> counts;
				for_each(b,data) 
					counts[b->first.count(r,*p)].inc(b->second);
				for_each(c,counts)
					file() << n << " " << *p << " " << c->first << " " << c->second;	
			}
		}
	}
};

// register experiments
FairnessExperiment ex1;
DataStatistics ex2;
FeatureStatistics ex3;