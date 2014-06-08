#include <string>
#include <map>
#include <set>
#include "id3.h"
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
			const outcome_counts_t& outcome_counts() const {return outcomes_;}
			const square_pieces_t& square_pieces()  const {return square_pieces_;}
			std::vector<Piece> pieces() const;
			std::vector<MatchOutcome> outcomes() const;
			std::vector<Square> squares() const;
	};

	class OutcomeData : public outcome_map_t {
	public:
			OutcomeStats calculate_stats() const;
	};

	/**
	 * This class is responsible for 'breaking up' the outcome data into
	 * examples (think rows), attributes (think columns) and classification
	 * a special column.
	 */
	class OutcomeDataTable: public ID3NameResolver {
		public:
			/** The copy-from constructor; the data is copied into the table and can be discarded after the call */
			OutcomeDataTable(const OutcomeData &data) : OutcomeDataTable(data,data.calculate_stats()) {}
			/**
			 * Use this constructor if you have already calculated the statistics (saves a bit of time)
			 * The data is copied into this instance
			 */
			OutcomeDataTable(const outcome_map_t &data, const OutcomeStats &stats) : data_(data) {build_table(stats);}
		  std::string attribute_name(const int a) override;
		  std::string value_name(const int a, const int v) override;
		  std::string class_name(const int c) override;
		private:
			std::vector<Square> attributes_;
			std::vector<Piece> values_;
			std::vector<MatchOutcome> classes_;
			const outcome_map_t data_;
			void build_table(const OutcomeStats &stats);

	};
}

std::ostream& operator <<(std::ostream& os, const arti::OutcomeStats& v);
