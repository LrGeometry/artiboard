#include "board.h"
#include "systemex.h"
#include "log.h"


namespace arti {
	const char out_of_bounds = '*';
	const Piece Piece::EMPTY(' ');
	const Piece Piece::OUT_OF_BOUNDS(out_of_bounds);

	Piece::Piece(square_value_t v): _value(v) {};

	std::ostream& operator <<(ostream& os, const Piece& v) {
		os << v.index();
		return os;
	}

	Board::Board() : _data(), _it_b(*this,0,0), _it_e(*this,7,7) {};

	const Piece& Board::at(const std::size_t colIndex, const std::size_t rowIndex) const {
		if (colIndex < 0 || rowIndex < 0 || colIndex > 7 || rowIndex > 7)
			return Piece::OUT_OF_BOUNDS;
		else
			return _data[rowIndex * 8 + colIndex];
	}

	void Board::place(const std::size_t colIndex, const std::size_t rowIndex, const Piece &v) {
		if (colIndex < 0 || rowIndex < 0 || colIndex > 7 || rowIndex > 7)
			throw runtime_error_ex("index out of bounds: col=%d row=%d",colIndex,rowIndex);
		else if (v == Piece::OUT_OF_BOUNDS)
			throw runtime_error_ex("cannot set square to out of bounds: col=%d row=%d",colIndex,rowIndex);
		_data[rowIndex * 8 + colIndex] = v;
	}

	void Board::operator()(const Region& ss, const Piece &value) {
		for (auto s = ss.cbegin(); s != ss.cend(); s++)
			place(s->file(),s->rank(),value);
	}

	int Board::count(const Region& ss, const Piece &value) const {
		register int result = 0;
		for (auto s = ss.begin(); s != ss.end(); s++)
			if (at(s->file(),s->rank()) == value)
				result++;
		return result;	
	}

	Region::const_iterator Board::find(const Region& ss, const Piece &value) const {
		for (auto s = ss.cbegin(); s != ss.cend(); s++) {
			if (at(s->file(),s->rank()) == value)
				return s;
		}
		return ss.cend();	
	}

	void Board::apply(std::function<void (Square, Piece)> fn) const {
		for (auto s = begin(); s != end(); s++)
			fn(s.pos(),*s);
	}


	int Board::count_repeats(const Region& ss, const Piece &value) const {
		int result = 0;
		bool repeating = false;
		int count = 0;
		for (auto s = ss.cbegin(); s != ss.cend(); s++)
			if (at(s->file(),s->rank()) == value) {
				if (repeating)
					count++;
				else
					count = 1;
				if (count > result)
					result = count;
				repeating = true;
			} else
				repeating = false;
		return result;	
	}

	bool Board::operator<(const Board& o) const {
		for (size_t i = 0; i < _data.size(); i++ )
			if (_data[i] < o._data[i])
				return true;
		return false;	
	}

}
