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
} c4_010;



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
} c4_020;

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
 * class OutcomeDataClassifier
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
		typedef std::function<void (std::forward_list<size_t> &result)> selector_fn;

		ExampleStratExperiment() : C4IcuExperiment("c4-025","The effect of the test selection strategy on ID3 accuracy") {}
		void do_run() override {
			IcuData data(data_filename());
			OutcomeDataTable table(data);
			// calculate the sets
			ElementIndexList D,Dn,Ds,Dd;
			D.fill(data.size());
			table.collect(Dn,MatchOutcome::NorthPlayerWins);
			table.collect(Ds,MatchOutcome::SouthPlayerWins);
			table.collect(Dd,MatchOutcome::Draw);
			Dn.collect_random_subset(Un,(Dn.size()*1)/4);
			Ds.collect_random_subset(Us,(Ds.size()*1)/4);
			Dd.collect_random_subset(Ud,(Dd.size()*1)/4);
			U.prepend(Un);
			U.prepend(Us);
			U.prepend(Ud);
			LOG << "|U|=" << U.size() << " |T|=" << T.size() << " |D|=" << D.size();
			CHECK(Ud.size() + Us.size() + Un.size() == U.size());
			for (auto e : D) {
				if (!U.contains(e)) T.push_front(e);
			}
			LOG << "|U|=" << U.size() << " |T|=" << T.size() << " |D|=" << D.size();
			CHECK(U.size() + T.size() == D.size());
			// build the decision tree
			OutcomeDataClassifier fier(table,0);
			fier.train(T);
			LOG << "U accuracy = " << fier.accuracy(U);
			CHECK(fier.accuracy(T) == 100);
			file() << "Strategy Measurement Size";
			run_strategy(fier,"W",Us);
			run_strategy(fier,"L",Un);
			//run_strategy(fier,"D",Ud);
			run_strategy(fier,"M",U);
			run_strategy(fier,"B",
				[&](std::forward_list<size_t> &result) {
					auto fs = (Us.size() * 1000) / U.size();
					auto fn = (Un.size() * 1000) / U.size();
					auto fd = 1000 - fs - fn;
					Us.collect_random_subset(result,fs);
					Un.collect_random_subset(result,fn);
					Ud.collect_random_subset(result,fd);
				});
			//fier.root().to_stream(LOG, table);
//			LOG << data.calculate_stats();
			// TODO 200 implement example selection strategy experiment
		}

		void run_strategy(OutcomeDataClassifier& f, const char * name, const ElementIndexList& s) {
			run_strategy(f,name,[&s](std::forward_list<size_t> &result){s.collect_random_subset(result,1000);});
		}

		void run_strategy(OutcomeDataClassifier& f, const char * name, selector_fn fn) {
			for (int i=0;i<100;i++) {
				std::forward_list<size_t> ts;
				fn(ts);
				file() << name << " " << f.accuracy(ts) << " " << size_of(ts);
			}
		}

private:
		ElementIndexList U,T, Un, Us, Ud;
} c4_025;

class MOCutOff: public C4IcuExperiment {
private:
		ElementIndexList D,U,T,Dn,Ds,Dd;
public:
		MOCutOff(): C4IcuExperiment("c4-026","Influence of the MO cut-off on ID3 - balanced") {}
	void balance_selectUT() {
		ElementIndexList Un, Us, Ud;
		U.clear();
		T.clear();
		Dn.collect_random_subset(Un,(Dn.size()*1)/4);
		Ds.collect_random_subset(Us,(Ds.size()*1)/4);
		Dd.collect_random_subset(Ud,(Dd.size()*1)/4);
		U.prepend(Un);
		U.prepend(Us);
		U.prepend(Ud);
		for (auto e : D) {
			if (!U.contains(e)) T.push_front(e);
		}
	}
	void do_run() override {
		IcuData data(data_filename());
		OutcomeDataTable table(data);
		D.fill(data.size());
		table.collect(Dn,MatchOutcome::NorthPlayerWins);
		table.collect(Ds,MatchOutcome::SouthPlayerWins);
		table.collect(Dd,MatchOutcome::Draw);
		file() << "Cutoff Accuracy Size";
		for (int o = 0; o < 30; o++)
				for (int i = 0; i < 10; i++) {
					const int cutoff = (i+1) * 32;
					std::cout << " " << o  << ":" << i;
					OutcomeDataClassifier fier(table,cutoff);
					balance_selectUT();
					fier.train(T);
					fier.test(U);
					file() << cutoff << " " << fier.root().certainty() << " " << fier.root().size();
				}
	}
} c4_026;

class MOCutOffT: public C4IcuExperiment {
public:
		MOCutOffT(): C4IcuExperiment("c4-027","Influence of the MO cut-off on ID3 - trained") {}
	void do_run() override {
		ElementIndexList training_set;
		IcuData data(data_filename());
		OutcomeDataTable table(data);
		training_set.fill(data.size());
		file() << "Cutoff Accuracy Size";
		const int steps = 30;
		set_steps(steps);
		for (int i = 0; i < steps; i++) {
			const int cutoff = i * 8;
			OutcomeDataClassifier fier(table,cutoff);
			fier.train(training_set);
			fier.test(training_set);
			file() << cutoff << " " << fier.root().certainty() << " " << fier.root().size();
			step();
		}
	}
} c4_027;

class EncodingExp: public C4IcuExperiment {
public:
		EncodingExp(): C4IcuExperiment("c4-028","Influence of the encoding") {}
	void do_run() override {
		ElementIndexList training_set;
		IcuData data(data_filename());
		training_set.fill(data.size());
		std::cout << "calculating stats\n";
		const auto stats = data.calculate_stats();
		for (auto &e : stats.pieces())
			std::cout << e;

		typedef std::pair<std::string,DataTableEncoder*> encoder_t;
		std::vector<encoder_t> encoders{
			{"SLE", new LocationEncoder(stats.squares(),stats.pieces())},
			{"ELE", new EnhancedLocationEncoder(stats.squares(),stats.pieces())},
			{"SRE", new LocationRegionEncoder(stats.squares(),stats.pieces())},
			{"ERE", new EnhancedLocationRegionEncoder(stats.squares(),stats.pieces())}
		};
		file() << "Cutoff Encoding Accuracy Size";
		const int increments = 12;
		const int steps = increments*encoders.size();
		set_steps(steps);
		for (int i = 0; i < increments; i++) {
			const int cutoff = i * 16;
			for (auto &e : encoders) {
				DataTableWithEncoder table(*e.second,data,stats);
				OutcomeDataClassifier fier(table,cutoff);
				fier.train(training_set);
				fier.test(training_set);
				LOG << cutoff << " " << e.first;
				file() << cutoff << " " << e.first << " " << fier.root().certainty() << " " << fier.root().size();
				step();
			}
		}

		for (auto &e : encoders)
			delete e.second;
	}
} c4_028;

class FeatureStatistics: public Experiment {
public:
	FeatureStatistics(): Experiment("c4-030","Display Connect-4 ICU data feature statistics") {}
	void do_run() override {
		const IcuData data(args()["icu_file"]);
		file() << "region piece count wins losses draws";
		const std::vector<Piece> pieces({Piece('-'), Piece('o'), Piece('x')});
		auto program = load_program(data_dir()+"\\regions.txt");
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
} c4_030;


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
		std::string value_name(const size_t a, const size_t v) override {
			const auto &result = names[v];
			if (result.empty()) {
				return names[v] = string_from_format("%d", v);
			} else return result;
		}
		std::string attribute_name(const size_t a) override {
			return attribs[a].first;
		}
		std::string class_name(const size_t c) override {
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
	int class_of(const size_t element) override {
		return db->items[element].outcome;
	}

	int value_of(const size_t element, const size_t attribute) override	{
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
} c4_300;



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


