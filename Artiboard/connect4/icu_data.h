#pragma once
#include <outcomedata.h>
/** Data from http://archive.ics.uci.edu/ml/datasets/Connect-4 
  * This database contains all legal 8-ply positions in the game of 
  * connect-4 in which neither player has won yet, and in which the next move is not forced.
  *  x is the first player; o the second. 
  * The outcome class is the game theoretical value for the first player.
	*/
class IcuData : public arti::OutcomeData {
public:
	IcuData(const std::string& filename);
};

using arti::LocationEncoder;
using arti::LocationRegionEncoder;
using arti::Board;
using arti::Square;
using arti::Piece;
using std::string;

class EnhancedValue {
	protected:
		EnhancedValue(const size_t v) : value_(v) {};
		bool is_enh(const size_t v) const {return v == value_;}
		std::string enh_value_name(const size_t v) const {
			if (v==value_) return "R";
			else return "";
		}
		int enh_value_of(const Piece superp, const Board &brd, const Square& s, const int super) const {
			if (superp == '-') {
				if (s.is_bottom())
					return value_;
				else {
					const auto b = s.below();
					if (brd(b) != '-')
						return value_;
				}
			}
			return super;
		}
		int enh_value() const {return value_;}
private:
		const size_t value_;
};

class EnhancedLocationEncoder : public LocationEncoder, public EnhancedValue {
public:
		EnhancedLocationEncoder(const std::vector<Square> &a, const std::vector<Piece> &v) :
			LocationEncoder(a,v),EnhancedValue(v.size()+1) {}
		string value_name(const size_t a, const size_t v) const final {
			if (is_enh(v)) return enh_value_name(v);
			else return LocationEncoder::value_name(a,v);
		}
		int value_of(const Board &brd, const size_t a) const final {
			const int super = LocationEncoder::value_of(brd,a);
			return enh_value_of(piece(super),brd,square(a),super);
		}
};

class EnhancedLocationRegionEncoder : public LocationRegionEncoder, public EnhancedValue {
public:
		EnhancedLocationRegionEncoder(const std::vector<Square> &a, const std::vector<Piece> &v) :
			LocationRegionEncoder(a,v),EnhancedValue(1) {}
		string value_name(const size_t a, const size_t v) const final {
			if (is_enh(v)) return square(a).to_string() + enh_value_name(v);
			else return LocationRegionEncoder::value_name(a,v);
		}
		int attribute_count() const override {return LocationRegionEncoder::attribute_count() + square_count();}
		int value_of(const Board &brd, const size_t a) const final {
			if (a >= LocationRegionEncoder::attribute_count()) {
				const auto a1 = a - LocationRegionEncoder::attribute_count();
				const auto s = square(a1);
				return enh_value_of(brd(s),brd,s,0);
			} else
				return LocationRegionEncoder::value_of(brd,a);
		}
};


