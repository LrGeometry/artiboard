#include <string>
#include <map>
#include <set>
#include "game.h"
namespace arti {
  typedef std::map<Board,MatchOutcome> outcome_map_t;
  typedef std::map<MatchOutcome,long> outcome_counts_t;
  typedef std::set<Piece> piece_set_t;
  typedef std::map<Square,piece_set_t> square_pieces_t;

	class OutcomeStats {
		private:
  		long size_;
  		outcome_counts_t outcomes_;
  		square_pieces_t square_pieces_;
		public:
  		// calculates the statistics from the given data
			OutcomeStats(const outcome_map_t& data);
			long size() const {return size_;}
			const outcome_counts_t& outcomes() const {return outcomes_;}
			const square_pieces_t& square_pieces()  const {return square_pieces_;}
	};

	class OutcomeData : public outcome_map_t {
	public:
			OutcomeStats calculate_stats();
	};
}

std::ostream& operator <<(std::ostream& os, const arti::OutcomeStats& v);
