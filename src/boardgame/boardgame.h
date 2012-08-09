#pragma once
#include <array>
#include <forward_list>
#include <list>
#include <ostream>
#include <memory>

#define FOR_SQUARES(row,col) for (index_t row = 0; row < 8; row++) for (index_t col = 0; col < 8; col++)
/**
 * The board game implementation framework.
 * The framework presumes that a board game is played on a Board that contains 64 squares.
 * Each square on the board has a Piece.  There are two players, one on the south
 * Side (looking north) and one of the north Side.  Starting from from Ply zero, the
 * south player creates the first Move.
 *
 * A concrete implementation of GameSpecification defines the implemented game.
 *
 * TODO 200 describe how to implement a specific game
 */
namespace board_game {
	using std::unique_ptr;
	using std::shared_ptr;
	using std::ostream;

	/* TODO 100 How do we store the game tree?
	 Possibilities:
	 a. initial board -> move -> board  S = M + B; M = M + B
	 b. initial -> move (with undo) -> move S = M + U
	 */
	/**
	 * The integral value of a Piece
	 */
	typedef char square_value_t;

	/**
	 * The value that is placed on a square
	 *
	 */
	class Piece {
		public:
			/**
			 * Marks a square as empty
			 */
			const static Piece EMPTY;
			/**
			 * Signifies that the square has no value.
			 */
			const static Piece OUT_OF_BOUNDS;
			explicit Piece(square_value_t v = EMPTY._value);
			bool is_empty() const {
				return _value == EMPTY._value;
			}
			;
			bool is_out_of_bounds() const {
				return _value == OUT_OF_BOUNDS._value;
			}
			;
			bool operator !=(const Piece& other) const {
				return _value != other._value;
			}
			;
			bool operator ==(const Piece& other) const {
				return _value == other._value;
			}
			;
			square_value_t index() const {
				return _value;
			}
			;
		private:
			square_value_t _value;
	};

	ostream& operator <<(std::ostream& os, const Piece& v);

	typedef std::size_t index_t;
	/**
	 * An 8x8 matrix of Piece objects.
	 * Left bottom square is (0,0), and right to is (7,7).
	 */
	class Board {
		public:
			/**
			 * Construct an instance that contains a Piece#EMPTY in each square.
			 */
			Board();
			/**
			 * What is on a square?
			 * @param colIndex
			 * @param rowIndex
			 * @return Piece::OUT_OF_BOUNDS if indexes are invalid
			 */
			const Piece& operator ()(const index_t colIndex,
					const index_t rowIndex) const;
			/**
			 * Place as Piece on a square
			 * @param colIndex must be a valid index
			 * @param rowIndex must be a valid index
			 * @param value cannot be Piece::OUT_OF_BOUNDS
			 */
			void operator()(const std::size_t colIndex, const std::size_t rowIndex,
					const Piece &value);
		private:
			std::array<Piece, 64> _data;
		public:
			typedef std::unique_ptr<Board> u_ptr;
	};

	ostream& operator <<(std::ostream& os, const Board& v);

	typedef std::forward_list<std::unique_ptr<Board>> BoardOwnerList;
	/**
	 * Where the player sits
	 */
	enum Side {
		South,
		North
	};

	/**
	 * A view of the board from a particular position,
	 * from a particular vantage point
	 */
	class BoardView {
		public:
			BoardView(const Board &brd, Side side = Side::South, std::size_t col = 0, std::size_t row = 0)
				: _vantageCol(col), _vantageRow(row), _vantageSide(side), _board(brd) {}
			void go(std::size_t col, std::size_t row) {
				_vantageCol = col;
				_vantageRow = row;
			};
			const Piece& anchor() const {return _board(_vantageCol, _vantageRow);};
			const index_t & col() const {return _vantageCol;};
			const index_t & row() const {return _vantageRow;};
			const Piece& relative(const index_t colOffset, const index_t rowOffset) const {
				const index_t delta_col = (_vantageSide == Side::South) ? colOffset : -colOffset;
				const index_t delta_row = (_vantageSide == Side::South) ? rowOffset : -rowOffset;
				return _board(_vantageCol + delta_col, _vantageRow + delta_row);
			}
		private:
			index_t _vantageCol, _vantageRow;
			Side _vantageSide;
			const Board &_board;
	};

	/**
	 * A level in the game tree.
	 */
	class Ply {
		public:
			const static Ply ZERO;
			Ply(int index) {_index = index;}
			int index() const {return _index;}
			Side side_to_move() const {return (_index%2 == 0)?Side::South:Side::North;}
			Ply next() const { return Ply(_index+1); }
		private:
			int _index;
	};

	enum StepOutcome {
		/**
		 * Ends the move
		 */
		EndsMove,
		/**
		 * Produces a move, and move steps may follow
		 */
		EndsMoveAndContinue
	};

	/**
	 * A change in a board position.
	 * Also keeps 'result' of the step: last_step_in_mo
	 */
	class Step {
		public:
			Step(StepOutcome outcome) : _outcome(outcome) {}
			StepOutcome outcome() const { return _outcome; }
			virtual void apply_on(Board &brd) const = 0;
			virtual ~Step() {};
		private:
			const StepOutcome _outcome;
		public:
			typedef std::forward_list<std::shared_ptr<Step>> SharedFWList;
	};

	class StepWithCoords : public Step {
		public:
			StepWithCoords(const index_t &col, const index_t &row, StepOutcome outcome)
				: Step(outcome), _col(col), _row(row) {};
		protected:
			const index_t _col;
			const index_t _row;
	};

	class StepToPlace : public StepWithCoords {
		public:
			StepToPlace(const BoardView &view, const Piece &piece, StepOutcome outcome = StepOutcome::EndsMove)
			   : StepWithCoords(view.col(),view.row(),outcome), _piece(piece) {};
			void apply_on(Board &brd) const override {
				brd(_col,_row, _piece);
			};
		private:
			const Piece _piece;
	};





	/**
	 * A move is a sequence of Step objects
	 */
	class Move {
		public:
			explicit Move(shared_ptr<Step>& step) {_steps.push_front(step);};
			unique_ptr<Board> apply_to(const Board &brd) const;
			void add(shared_ptr<Step>& step);
		private:
			Step::SharedFWList _steps;
		public:
			typedef std::shared_ptr<Move> s_ptr;
			typedef std::forward_list<s_ptr> SharedFWList;
	};



	/**
	 * The state of the game at a particular Ply.
	 */
	class Position {
		public:
			Position(const Ply &ply, unique_ptr<Board> &brd) : _ply(ply), _board(std::move(brd)) {}
			const Ply& ply() const {return _ply;}
			const bool is_root() const {return ply().index() == 0;}
			const Board& board() const {return *_board;}
		private:
			Ply _ply;
			unique_ptr<Board> _board;
		public:
			typedef std::shared_ptr<Position> s_ptr;
			typedef std::list<std::shared_ptr<Position>> SharedList;
	};

	ostream& operator <<(std::ostream& os, const Position& v);

	enum MatchOutcome {
		Unknown, SouthPlayerWins, NorthPlayerWins, Draw
	};
	/**
	 * Abstract class that describes the rules of a game.
	 */
	class GameSpecification {
		public:
			unique_ptr<Board> initialBoard() const;
			/**
			 * Collect all moves possible from ply into result
			 * @param ply
			 * @param result
			 */
			virtual void collectMoves(const Position& pos, Move::SharedFWList &result) const = 0;
			virtual MatchOutcome outcome_of(const Position& pos) const = 0;
			virtual ~GameSpecification() {};
		protected:
			/**
			 * Place the pieces at their initial position.
			 * @param board is initially empty, and must be updated.
			 */
			virtual void setup(Board& board) const = 0;
	};

	/**
	 * Use this for games that have local rules.
	 * These games can make move decisions by considering possible steps, one
	 * square at a time.
	 */
	class GameSpecificationWithLocalSteps: public GameSpecification {
		public:
			virtual void collectMoves(const Position& pos, Move::SharedFWList &result) const override;
		protected:
			virtual void collectSteps(const Position& pos, const BoardView& view, int stepIndex, Step::SharedFWList &result) const = 0;

		};


	/**
	 * A sequence of Position objects  that describes a match.
	 */
	class PlayLine {
		public:
			/**
			 * Constructs an empty play line
			 */
			PlayLine(unique_ptr<Board> initial);
			/**
			 * The current Position
			 * @return
			 */
			const Position& last() const {return *_plies.back();}
			const Position& root() const {return *_plies.front();}
			void add(unique_ptr<Board> &brd);
			const Position::SharedList& sequence() const {return _plies;}
		private:
			Position::SharedList _plies;
	};

	ostream& operator <<(std::ostream& os, const PlayLine& v);
	/**
	 * Makes the choice of which move to pick next
	 */
	class MoveChooser {
		public:
			virtual unique_ptr<Board>& select(const Position & current, BoardOwnerList &list) = 0;
			virtual ~MoveChooser() {
			}
	};

	/**
	 * A single play of a GameSpecification
	 *
	 */
	class Match {
		public:
			Match(const GameSpecification &spec, MoveChooser &chooser);
			/**
			 * Play the game until an outcome is reached.
			 */
			void play();
			MatchOutcome outcome() const {
				return _outcome;
			}

			const PlayLine& line() const {return _line;}

		private:
			const GameSpecification& _spec;
			MoveChooser& _chooser;
			PlayLine _line;
			MatchOutcome _outcome;
		};

	ostream& operator <<(std::ostream& os, const Match& v);
}
