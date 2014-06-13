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
  typedef std::pair<Board,MatchOutcome> board_outcome_t;

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
			OutcomeDataTable(const outcome_map_t &data, const OutcomeStats &stats) : data_(data.size()) {build_table(data,stats);}
		  std::string attribute_name(const size_t a) override;
		  std::string value_name(const size_t a, const size_t v) override;
		  std::string class_name(const size_t c) override;
			int value_of(const size_t i, const size_t a) const;
			int class_of(const size_t i) const;
			int data_count() const {return data_.size();}
			int attribute_count() const {return attributes_.size();}
		private:
			std::vector<Square> attributes_;
			std::vector<Piece> values_;
			std::vector<MatchOutcome> classes_;
			std::vector<board_outcome_t> data_;
			void build_table(const outcome_map_t &data,const OutcomeStats &stats);

	};

	class OutcomeDataClassifier : public ID3Classifier {
		public:
			OutcomeDataClassifier(const OutcomeDataTable& table, size_t cc = 0) : ID3Classifier(cc), table_(table) {}
			int value_of(const size_t element, const size_t attribute) override {return table_.value_of(element,attribute);}
			int class_of(const size_t element) override {return table_.class_of(element);}
			void train() {ID3Classifier::train(table_.data_count(), table_.attribute_count());}
		private:
			const OutcomeDataTable &table_;
	};
}

std::ostream& operator <<(std::ostream& os, const arti::OutcomeStats& v);
