#pragma once
#include <array>
#include <forward_list>
#include <list>
#include <ostream>
#include <memory>

#define FOR_SQUARES(row,col) for (std::size_t row = 0; row < 8; row++) for (std::size_t col = 0; col < 8; col++)
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
			square_value_t operator()() const {
				return _value;
			}
			;
		private:
			square_value_t _value;
	};

	inline std::ostream& operator <<(std::ostream& os, const Piece& v) {
		os << (int) v();
		return os;
	}

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
			const Piece& operator ()(const std::size_t colIndex,
					const std::size_t rowIndex) const;
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
			BoardView(const Board &brd, Side side, std::size_t col = 0, std::size_t row = 0)
				: _vantageCol(col), _vantageRow(row), _vantageSide(side), _brd(brd) {}
			void go(std::size_t col, std::size_t row) {
				_vantageCol = col;
				_vantageRow = row;
			};
		private:
			std::size_t _vantageCol, _vantageRow;
			Side _vantageSide;
			const Board &_brd;
	};

	/**
	 * A level in the game tree.
	 */
	class Ply {
		public:
			const static Ply ZERO;
			Ply(int index) {_index = index;}
			int index() const {return _index;}
			Side sideToMove() const {return (_index%2 == 0)?Side::South:Side::North;}
			Ply next() const { return Ply(_index+1); }
		private:
			int _index;
	};

	enum StepOutcome {
		/**
		 * The winning move / and step
		 */
		EndsGameInWin,
		/**
		 * Ends the game in a tie
		 */
		EndsGameInTie,
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
			void apply_on(Board &brd) const;
		private:
			const StepOutcome _outcome;
		public:
			typedef std::forward_list<std::shared_ptr<Step>> SharedFWList;
	};



	/**
	 * A move is a sequence of Step objects
	 */
	class Move {
		public:
			unique_ptr<Board> apply_to(const Board &brd) const;
			void add(shared_ptr<Step> step);
		private:
			Step::SharedFWList _steps;
	};

	typedef std::shared_ptr<Move> MoveSP;
	typedef std::forward_list<MoveSP> MoveList;



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
			virtual void collectMoves(const Position& pos, MoveList &result) const = 0;
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
			virtual void collectMoves(const Position& pos, MoveList &result) const override;
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
		private:
			std::list<std::shared_ptr<Position>> _plies;
	};

	enum MatchOutcome {
		Unknown, SouthPlayerWins, NorthPlayerWins, Draw
	};

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

		private:
			const GameSpecification& _spec;
			MoveChooser& _chooser;
			PlayLine _line;
			MatchOutcome _outcome;
		};

}
