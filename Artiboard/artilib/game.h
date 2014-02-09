#pragma once
#include "board.h"
#include "systemex.h"
namespace arti {
	/**
	 * A level in the game tree, there are two plies for a move.
	 */
	class Ply {
		public:
			const static Ply ZERO;
			Ply(int index) {_index = index;}
			int index() const {return _index;}
			Side side_to_move() const {return (_index%2 == 0)?Side::South:Side::North;}
			Ply next() const { return Ply(_index+1); }
			bool is_odd() const {return _index%2 == 1;}
			bool operator==(int v) const {return _index == v; }
			int operator+(int v) const {return _index + v; }
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
			StepToPlace(const Square &s, const Piece &piece, StepOutcome outcome = StepOutcome::EndsMove)
			   : StepWithCoords(s.file(),s.rank(),outcome), _piece(piece) {};
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
			Position(const Ply &ply, unique_ptr<Board> brd) : _ply(ply), _board(std::move(brd)) { ASSERT(_board);}
			const Ply& ply() const {return _ply;}
			const bool is_root() const {return ply().index() == 0;}
			const Board& board() const {return *_board;}
			unique_ptr<Board>& board_p() {return _board;}
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

	ostream& operator <<(std::ostream& os, const MatchOutcome& v);

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
			/* Add next Boards to result, return the number of Boards appended */
			int collectBoards(const Position& pos, Board::u_ptr_list &result) const;
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
			void add(unique_ptr<Board> brd);
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
			/** Choose a child from the list of children */
			virtual Board::u_ptr_it select(const Position & current, Board::u_ptr_list &children) = 0;
			virtual ~MoveChooser() {
			}
	};

	class PickFirst: public MoveChooser {
		  /** Choose the first child in the list of children */
			Board::u_ptr_it select(const Position & current,Board::u_ptr_list &children) override {
				return children.begin();
			}
			;
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
