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
FairnessExperiment ex1;



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
DataStatistics ex2;

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
FeatureStatistics ex3;


typedef std::pair<arti::Board, MatchOutcome> element_type;
typedef std::pair<arti::Region,arti::Piece> attrib_type;
typedef std::pair<std::string,arti::Piece> attrib_type2;

template<class S, class T> std::ostream& operator<<(std::ostream& os, const std::pair<S,T> & v) {
	os << "(" << v.first << ":" << v.second << ")";
	return os; 
}

struct AnnotatedData {
	AnnotatedData(AnnotatedBoard b, MatchOutcome o) : board(b), outcome(o) {}
	const AnnotatedBoard board;
	const MatchOutcome outcome;
};

class AnnotatedDatabase: public ID3NameResolver {
	private:
		const std::string region_file_name_;
	public:
		std::vector<AnnotatedData> items;
		std::vector<attrib_type2> attribs;
		std::unique_ptr<FeatureProgram> program;
		std::map<int, std::string> names;
		AnnotatedDatabase(const std::string& region_file_name) : region_file_name_(region_file_name) {
			collect_annos();
			collect_attribs();
		}
		const std::string& value_name(const int a, const int v) {
			const auto &result = names[v];
			if (result.empty()) {
				return names[v] = string_from_format("%d", v);
			} else return result;
		}
		const std::string& attribute_name(const int a) {
			return attribs[a].first;
		}
		const std::string& class_name(const int c) {
			const auto &result = names[c];
			if (result.empty()) {
				return names[c] = string_from_format("%d", c);
			} else return result;
		}
		virtual ~AnnotatedDatabase() {
		}
	private:
		void collect_annos() {
			const IcuData& data = IcuData::instance();
			items.reserve(data.size());
			FOR_EACH(i,data)
				items.emplace_back(AnnotatedBoard(i->first), i->second);
		}
		void collect_attribs() {
			const auto &pieces = annotation_pieces();
			program = std::move(load_program(region_file_name_));
			attribs.reserve(program->regions().size() * size_of(pieces));
			FOR_EACH(nr,program->regions())
				FOR_EACH(p,pieces)
				{
					attribs.emplace_back(nr->first, *p);
				}
		}

};

class AnnotatedClassifier : public ID3Classifier {
public:
	AnnotatedDatabase *db;
public:
	explicit AnnotatedClassifier(AnnotatedDatabase *data, const size_t count_cut) : ID3Classifier(count_cut), db(data) {}
  int class_of(const int element) override {
  	return db->items[element].outcome;
  }

	int value_of(const int element, const int attribute) override	{
		const auto &b = db->items[element].board;
		const auto &r = db->program->regions()[db->attribs[attribute].first];
		const auto &p = db->attribs[attribute].second;
		return b.count(r,p) > 0 ? 1 : 0;
	}
};

class Classify: public Experiment {
public:
	Classify(): Experiment("c4-300","Classify the ICU data set") {}
	void doRun() override {
		// datadir = ../connect4/data/
		auto datadir = args()["data_dir"];
		AnnotatedDatabase db(datadir + "regions.txt");
		file() << "fraction cutoff size certainty";
		for (int f = 3; f < 10; f++) {
			for (int i = 0; i < 30; i++) {
				const int cutoff = i * 32;
				LOG << "At " << f << ":" << i;
				std::cout << "At " << f << ":" << i;
				AnnotatedClassifier cf(&db,cutoff);
				cf.train_and_test(db.items.size(),db.attribs.size(),f);
				file() << f << " " << cutoff << " " << cf.root().size() <<" " << cf.root().certainty();
				cf.root().to_stream(LOG,db);
			}
		}
	}
} classfy;



