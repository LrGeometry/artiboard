#include "outcomedata.h"
#include "log.h"
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

void DataTable::init(const outcome_map_t &data,const OutcomeStats& stats) {
	classes_ = stats.outcomes();
	// copy data from source map into flat list
	size_t i=0;
	for (const auto &e : data)
		data_[i++] = e;
	ASSERT(i == data.size());
}

LocationEncoder::LocationEncoder(const std::vector<Square> &a, const std::vector<Piece> &v)
: SquareEncoder(a) {
	values_ = v;
}

int LocationEncoder::value_of(const Board &b, const size_t a) const {
	const Piece oc = b(attribute(a));
	for (size_t k=0;k<values_.size();k++)
		if (values_[k] == oc)
			return k;
	throw runtime_error_ex("data value not found for attribute '%s' element", attribute(a).to_string().c_str());
	return -1;
}

int DataTable::class_of(const size_t i) const {
	auto oc = data_[i].second;
	for (size_t i=0;i<classes_.size();i++)
		if (classes_[i] == oc)
			return i;
	FAIL("data class not found");
	return -1;
}


std::string SquareEncoder::attribute_name(const size_t a) const{
	return attributes_[a].to_string();
}


std::string DataTable::class_name(const size_t c){
	return to_string(classes_[c]);
}

void DataTable::collect_if(element_index_list_t &result, pred_board_outcome_t fn) const {
		for (size_t i=0; i<data_.size(); i++)
			if (fn(data_[i].first, data_[i].second))
				result.push_front(i);
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

