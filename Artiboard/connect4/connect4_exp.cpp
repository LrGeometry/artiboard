#include <experiment.h>
#include <feat.h>
#include <id3.h>
#include <forward_list>
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
	void inc(const MatchOutcome& e) {
			if (e == NorthPlayerWins)
				wins++;
			else if (e == SouthPlayerWins)
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
		FOR_EACH(p,data) {
			int ply = ply_of(p->first);
			stats[ply].inc(p->second);
		}
		FOR_EACH(s,stats)
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
		FOR_EACH(nr,program->regions()) {
			auto n = nr->first;
			auto r = nr->second;
			FOR_EACH(p,pieces) {
				std::map<int,DataStat> counts;
				FOR_EACH(b,data) 
					counts[b->first.count(r,*p)].inc(b->second);
				FOR_EACH(c,counts)
					file() << n << " " << *p << " " << c->first << " " << c->second;	
			}
		}
	}
};

typedef std::pair<arti::Board, MatchOutcome> element_type;
typedef std::pair<arti::Region,arti::Piece> attrib_type;
typedef std::pair<std::string,arti::Piece> attrib_type2;

template<class S, class T> std::ostream& operator<<(std::ostream& os, const std::pair<S,T> & v) {
	os << "(" << v.first << ":" << v.second << ")";
	return os; 
}

class Connect4Classifier : public Classifier<attrib_type, element_type, MatchOutcome, int> {
	int value_of(const element_type& e, const attrib_type &a) override {
			return e.first.count_repeats((a.first),a.second);
	};

  MatchOutcome class_of(const element_type &e) override {
			return e.second;
	};
};

class FeatureCheck: public Experiment {
public:
	FeatureCheck(): Experiment("c4-040","Impoved Connect-4 ICU data features") {}
	void doRun() override {
		const IcuData& data = IcuData::instance();
		file() << "region is_mixed";
		const std::vector<Piece> pieces({Piece('-'), Piece('o'), Piece('x')});
		auto program = load_program("../connect4/data/regions.txt");
		Connect4Classifier cf;
		FOR_EACH(nr,program->regions()) 
			FOR_EACH(p,pieces) {
				attrib_type a(nr->second,*p);
				file() << nr->first << " " << *p << " " << (cf.is_mixed(a,data.begin(),data.end())?"yes":"no"); 
			}
	}
};

struct AnnotatedData {
	AnnotatedData(AnnotatedBoard b, MatchOutcome o) : _board(b), _outcome(o) {}
	const AnnotatedBoard _board;
	const MatchOutcome _outcome;
};

class AnnotatedClassifier : public Classifier<attrib_type2, AnnotatedData, MatchOutcome, int> {
	FeatureProgram::u_ptr _program;
public:
	AnnotatedClassifier(FeatureProgram::u_ptr& p) : _program(std::move(p)) {}
	AnnotatedClassifier() : _program(std::move(load_program("../connect4/data/regions.txt"))) {}
  MatchOutcome class_of(const AnnotatedData &e) override {
			return e._outcome;
	};

	int value_of(const AnnotatedData& e, const attrib_type2 &a) override {
			return e._board.count(program().regions().at(a.first),a.second);
			//return e._board.count(program().regions().at(a.first),a.second) > 0 ? 1 : 0;
			//return e._board.count_repeats(program().regions().at(a.first),a.second);
	};

	const FeatureProgram& program() const {return *_program;}


	std::list<attrib_type2> collect_attribs() {
		std::list<attrib_type2> result;
		auto pieces = annotation_pieces();
		FOR_EACH(nr,program().regions()) 
			FOR_EACH(p,pieces) {
				result.emplace_front(attrib_type2(nr->first,*p));
			}
		return result;	
	}

};

std::forward_list<AnnotatedData> collect_annos() {
	std::forward_list<AnnotatedData> result;
	const IcuData& data = IcuData::instance();
	FOR_EACH(i,data) 
		result.emplace_front(AnnotatedData(AnnotatedBoard(i->first),i->second));
	return result;	
}

class AnnotatedFeatureCheck: public Experiment {
public:
	AnnotatedFeatureCheck(): Experiment("c4-050","AnnotatedFeatureCheck Connect-4 ICU data features") {}
	void doRun() override {
		auto annos = collect_annos();
		file() << "region is_mixed";
		auto pieces = annotation_pieces();
		AnnotatedClassifier cf;
		FOR_EACH(nr,cf.program().regions()) 
			FOR_EACH(p,pieces) {
				attrib_type2 a(nr->first,*p);
				file() << nr->first << " " << *p << " " << (cf.is_mixed(a,annos.begin(),annos.end())?"yes":"no"); 
			}
	}
};

class AnnotatedFeatureStatistics: public Experiment {
public:
	AnnotatedFeatureStatistics(): Experiment("c4-060","Connect-4 ICU data features") {}
	void doRun() override {
		auto annos = collect_annos();
		file() << "region piece count wins losses draws";
		AnnotatedClassifier cf;
		auto attribs = cf.collect_attribs();
		FOR_EACH(a,attribs) {
			std::map<int,DataStat> counts;
			FOR_EACH(b,annos) 
				counts[cf.value_of(*b,*a)].inc(b->_outcome);
			FOR_EACH(c,counts)
				file() << a->first << " " << a->second << " " << c->first << " " << c->second;	
		}
	}
};

std::list<attrib_type> collect_attribs() {
	std::list<attrib_type> result;
	auto pieces = annotation_pieces();
	auto program = load_program("../connect4/data/regions.txt");
	FOR_EACH(nr,program->regions()) 
		FOR_EACH(p,pieces) {
			result.emplace_front(attrib_type(nr->second,*p));
		}
	return result;	
}

// register experiments
FairnessExperiment ex1;
DataStatistics ex2;
FeatureStatistics ex3;
FeatureCheck ex4;
AnnotatedFeatureCheck ex5;
AnnotatedFeatureStatistics ex6;

class AnnotatedEntropy: public Experiment {
public:
	AnnotatedEntropy(): Experiment("c4-070","AnnotatedEntropy Connect-4 ICU data features") {}
	void doRun() override {
		auto annos =	collect_annos();
		file() << "region subset_entropy";
		AnnotatedClassifier cf;
		auto attribs = cf.collect_attribs();
		auto entrops = cf.collect_entropies_sorted(attribs.begin(),attribs.end(),annos.begin(),annos.end());
		FOR_EACH(e,entrops) {
			file() << e->first.first << " " << e->first.second << " " << e->second; 
		}
	}
};
AnnotatedEntropy ex7;



