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

	class DataTable : public ID3NameResolver {
		public:
			DataTable(const outcome_map_t &data, const OutcomeStats &stats) : data_(data.size()) {init(data,stats);}
			int class_of(const size_t i) const;
			int data_count() const {return data_.size();}
			virtual int value_of(const size_t i, const size_t a) const = 0;
			virtual	int attribute_count() const = 0;
			std::string class_name(const size_t c) final;
			void collect_if(element_index_list_t &result, pred_board_outcome_t fn) const;
			void collect(element_index_list_t &result, const MatchOutcome poc) {collect_if(result,
				[&poc](const Board& brd, const MatchOutcome &oc) {return oc == poc;});
			}

		protected:
			const board_outcome_t& data(size_t i) const {return data_[i];}
		private:
			void init(const outcome_map_t &data, const OutcomeStats &stats);
		private:
			std::vector<MatchOutcome> classes_;
			std::vector<board_outcome_t> data_;
	};

	class DataTableEncoder {
		public:
			virtual int value_of(const Board& b, const size_t a) const = 0;
			virtual	int attribute_count() const = 0;
		  virtual std::string attribute_name(const size_t a) const = 0;
		  virtual std::string value_name(const size_t a, const size_t v) const = 0;
	};

	class DataTableWithEncoder: public DataTable {
		public:
			DataTableWithEncoder(DataTableEncoder &encoder, const outcome_map_t &data, const OutcomeStats &stats) : DataTable(data,stats), encoder_(encoder) {}
		  std::string attribute_name(const size_t a) final {return encoder_.attribute_name(a);}
		  std::string value_name(const size_t a, const size_t v) {return encoder_.value_name(a,v);}
			int attribute_count() const final {return encoder_.attribute_count();}
			int value_of(const size_t i, const size_t a) const final {return encoder_.value_of(data(i).first,a);};
		private:
			const DataTableEncoder &encoder_;
	};

	class DataTableWithOwnerEncoder: public DataTableWithEncoder {
		public:
			DataTableWithOwnerEncoder(DataTableEncoder *encoder, const outcome_map_t &data, const OutcomeStats &stats) : DataTableWithEncoder(*encoder,data,stats), encoder_p(encoder) {}
			virtual ~DataTableWithOwnerEncoder(){delete encoder_p;}
		protected:
			DataTableEncoder * encoder_p;
	};

	class SquareEncoder: public DataTableEncoder {
		public:
			SquareEncoder(const std::vector<Square> &a) {attributes_ = a;}
		  std::string attribute_name(const size_t a) const final;
			int attribute_count() const final {return attributes_.size();}
		protected:
			const Square& attribute(const size_t i) const {return attributes_[i];}
		private:
			std::vector<Square> attributes_;
	};

	class LocationEncoder: public SquareEncoder {
		public:
			LocationEncoder(const std::vector<Square> &a, const std::vector<Piece> &v);
		  std::string value_name(const size_t a, const size_t v) const final {return values_[v].to_string();}
			int value_of(const Board &b, const size_t a) const final;
		private:
			std::vector<Piece> values_;
	};

	class OutcomeDataTable: public DataTableWithOwnerEncoder {
		public:
			/** The copy-from constructor; the data is copied into the table and can be discarded after the call */
			OutcomeDataTable(const OutcomeData &data) : OutcomeDataTable(data,data.calculate_stats()) {}
			/**
			 * Use this constructor if you have already calculated the statistics (saves a bit of time)
			 * The data is copied into this instance
			 */
			OutcomeDataTable(const outcome_map_t &data, const OutcomeStats &stats) : DataTableWithOwnerEncoder(new LocationEncoder(stats.squares(),stats.pieces()),data,stats) {}
	};

	class OutcomeDataClassifier : public ID3Classifier {
		public:
			OutcomeDataClassifier(const DataTable& table, size_t cc = 0) : ID3Classifier(cc), table_(table) {}
			int value_of(const size_t element, const size_t attribute) override {return table_.value_of(element,attribute);}
			int class_of(const size_t element) override {return table_.class_of(element);}
			void train_and_test() {ID3Classifier::train_and_test(table_.data_count(), table_.attribute_count());}
			void train(std::forward_list<size_t>  &elements) {ID3Classifier::train(elements,table_.attribute_count());}
		private:
			const DataTable &table_;
	};
}

std::ostream& operator <<(std::ostream& os, const arti::OutcomeStats& v);
