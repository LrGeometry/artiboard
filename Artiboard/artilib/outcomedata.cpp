#include "outcomedata.h"

namespace arti {
OutcomeStats::OutcomeStats(const outcome_map_t &data) {
	size_ = data.size();
	for (auto &e : data) {
		outcomes_[e.second]++;
		e.first.apply(
			[&](Square s, Piece p) {
				if (!p.is_empty())
					square_pieces_[s].insert(p);
		}
		);
	}
}

std::vector<Piece> OutcomeStats::pieces() const {
	std::set<Piece> t;
	for (const auto &e : square_pieces_)
		for (const auto &p : e.second)
			t.insert(p);
	std::vector<Piece> result;
	for (const auto &e : t) result.push_back(e);
	return result;
}

std::vector<MatchOutcome> OutcomeStats::outcomes() const {
	std::vector<MatchOutcome> result;
	for (const auto &e : outcomes_)
		result.push_back(e.first);
	return result;
}

std::vector<Square> OutcomeStats::squares() const {
	std::vector<Square> result;
	for (const auto &e : square_pieces_)
		result.push_back(e.first);
	return result;
}


OutcomeStats OutcomeData::calculate_stats() const {
	return OutcomeStats(*this);
}



void OutcomeDataTable::build_table(const OutcomeStats& stats) {
	attributes_ = stats.squares();
	classes_ = stats.outcomes();
	values_ = stats.pieces();
}


std::string OutcomeDataTable::attribute_name(const int a){
	return attributes_[a].to_string();
}

std::string OutcomeDataTable::value_name(const int a, const int v){
	return values_[v].to_string();

}
std::string OutcomeDataTable::class_name(const int c){
	return to_string(classes_[c]);
}

}


std::ostream& operator <<(std::ostream& os, const arti::OutcomeStats& v) {
	os << "Size: " << v.size() << std::endl;
	os << "Outcomes: ";
	for (auto e : v.outcome_counts())
		os << e.first << "=" << e.second << " (" << e.second*100.0/v.size() << "%) ";
	os << std::endl;
	os << "Square pieces: ";
	for (auto e : v.square_pieces())
		os << "\t" << e.first << "=" << e.second.size();
	return os;
}

