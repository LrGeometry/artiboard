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

OutcomeStats OutcomeData::calculate_stats() {
	return OutcomeStats(*this);
}
}


std::ostream& operator <<(std::ostream& os, const arti::OutcomeStats& v) {
	os << "Size: " << v.size() << std::endl;
	os << "Outcomes: ";
	for (auto e : v.outcomes())
		os << e.first << "=" << e.second << " (" << e.second*100.0/v.size() << "%) ";
	os << std::endl;
	os << "Square pieces: ";
	for (auto e : v.square_pieces())
		os << "\t" << e.first << "=" << e.second.size();
	return os;
}
