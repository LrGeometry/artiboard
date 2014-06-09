#include <experiment.h>
#include <feat.h>
#include <id3.h>
#include <forward_list>
#include <log.h>
#include "connect4.h"
#include "icu_data.h"
using namespace arti;

class FairnessExperiment: public Experiment {
	public:
		FairnessExperiment() : Experiment("c4-010","Connect-4 fairness") {}
	protected:
		void do_run() override{
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
	DataStatistics(): Experiment("c4-020","Display Connect-4 ICU data statistics") {}
	void do_run() override {
		file() << "ply wins losses draws";
		const IcuData data(args()["icu_file"]);
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

class C4IcuExperiment : public Experiment {
	public:
		C4IcuExperiment(const char * name, const string &desc) : Experiment(name,desc) {}
	protected:
		std::string data_filename() {return args()["icu_file"];}
};

/**
 * http://plantuml.sourceforge.net/classes.html
 * @startuml
 * class ExampleStratExperiment *-> IcuData
 * IcuData -|> arti.OutcomeData
 * ExampleStratExperiment *-> arti.OutcomeDataClassifier
 * ExampleStratExperiment *-> arti.OutcomeDataTable
 * package arti {
 * class ID3Classifier
 * OutcomeDataClassifier --|> ID3Classifier
 * class OutcomeDataTable
 * class OutcomeDataClassifier << todo >>
 * OutcomeDataClassifier o-->  OutcomeDataTable
 * OutcomeData <.. OutcomeDataTable : construct
 * ID3NameResolver <|-- OutcomeDataTable
 * OutcomeStats <.. OutcomeData : calculate_stats()
 * OutcomeData "+" *--> MapElement
 * OutcomeDataTable *--> MapElement : data
 * MapElement *--> Board
 * MapElement *-->MatchOutcome
 * OutcomeDataTable ..> OutcomeStats
 * OutcomeDataTable "+" *--> Piece : values
 * OutcomeDataTable "+" *--> MatchOutcome : classes
 * OutcomeDataTable "+" *--> Square : attributes
 * }
 * @enduml
 */
class ExampleStratExperiment : public C4IcuExperiment {
public:
		ExampleStratExperiment() : C4IcuExperiment("c4-025","The effect of the test selection strategy?") {}
		void do_run() override {
			IcuData data(data_filename());
			OutcomeDataTable table(data);
			OutcomeDataClassifier fier(table);
			fier.train();
			fier.root().to_stream(LOG, table);
			// LOG << data.calculate_stats();
			// TODO 200 implement example selection strategy experiment
		}
} c4_025;


class FeatureStatistics: public Experiment {
public:
	FeatureStatistics(): Experiment("c4-030","Display Connect-4 ICU data feature statistics") {}
	void do_run() override {
		const IcuData data(args()["icu_file"]);
		file() << "region piece count wins losses draws";
		const std::vector<Piece> pieces({Piece('-'), Piece('o'), Piece('x')});
		auto program = load_program("../connect4/data/squares.txt"); 
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
		const IcuData& data_;
		AnnotatedDatabase(const std::string& region_file_name, const IcuData& data) :
			region_file_name_(region_file_name), data_(data) {
			collect_annos();
			collect_attribs();
		}
		std::string value_name(const int a, const int v) {
			const auto &result = names[v];
			if (result.empty()) {
				return names[v] = string_from_format("%d", v);
			} else return result;
		}
		std::string attribute_name(const int a) {
			return attribs[a].first;
		}
		std::string class_name(const int c) {
			const auto &result = names[c];
			if (result.empty()) {
				return names[c] = string_from_format("%d", c);
			} else return result;
		}
		virtual ~AnnotatedDatabase() {
		}
	private:
		void collect_annos() {

			items.reserve(data_.size());
			FOR_EACH(i,data_)
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
	Classify(): Experiment("c4-300","Find a good cutoff value for ID3") {}
	void do_run() override {
		auto datadir = args()["data_dir"];
		IcuData data(datadir + "/downloaded/connect-4.data");
		AnnotatedDatabase db(datadir + "\\regions.txt", data);
		file() << "fraction cutoff size certainty";
		for (int f = 3; f < 10; f++) {
			for (int i = 0; i < 10; i++) {
				const int cutoff = i * 32;
				LOG << "At " << f << ":" << i;
				std::cout << "At " << f << ":" << i;
				AnnotatedClassifier cf(&db,cutoff);
				cf.train_and_test(db.items.size(),db.attribs.size(),f);
				file() << f << " " << cutoff << " " << cf.root().size() <<" " << cf.root().certainty();
			}
		}
	}
} classfy;

class ClassifyRegions: public Experiment {
public:
		ClassifyRegions() : Experiment("c4-400","Which regions classifies better?") {}
		void do_run() override {
			IcuData data(data_fn("downloaded/connect-4.data"));
			file() << "region size certainty";
			do_step("regions.txt","all-lines",data);
			do_step("regions-diag.txt","diagonal-lines",data);
			do_step("regions-sq.txt", "squares",data);
			do_step("regions-cr.txt", "adjacent-lines",data);
		}
private:
		void do_step(const string& filename, const string& regionname, const IcuData& data) {
			AnnotatedDatabase db(data_fn(filename), data);
			AnnotatedClassifier cf(&db,64);
			cf.train_and_test(db.items.size(),db.attribs.size(),9);
			file() << regionname << " " << cf.root().size() <<" " << cf.root().certainty();
		}
} c4_400;


