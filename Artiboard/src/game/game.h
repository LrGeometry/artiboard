#pragma once
#include <ostream>
#include <istream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include "../systemex/systemex.h"

namespace game {
	typedef short ordinal_t; 
	/*
	* A square identifies a position on the gameboard
	* A square is the ordered pair (file,rank) where the left-bottom
	* is (1,1) and top-right is (8,8).
	* A square is either light or dark. left-bottom is light.
	* Squares are also indexed in to ways: 0 to 63, and by color index
	* 0 to 31.
	*/
	class Square
	{
	public:
		// check if f and r is from 1 to 8
		static bool in_bounds(ordinal_t f, ordinal_t r)	{ return (f > 0) & (f < 9)& (r > 0) & (r < 9); }
		enum color_t {Light, Dark};
		// index for a valid square is 0 to 63
		ordinal_t index(void) const;
		ordinal_t rank() const {return _rank;}
		ordinal_t file() const {return _file;}
		bool is_valid() const {return in_bounds(_file,_rank);}
		ordinal_t color_index(const bool flip = false) const;
		color_t color() const;

		// normal constructor
		Square(const ordinal_t file, const ordinal_t rankValue);
		// contruct a flip of another square
		Square(const Square& source, bool flip);
		// constucts an offset square
		Square(const Square& source, 
			const ordinal_t offset_file, 
			const ordinal_t offset_rank);
		Square(const ordinal_t dark_color_index);
		enum { NOT_VALID = 99};
		//default constructor creates an invalid square
		// (0,0)
		Square();

	private:
		/* updates */
		// change to the mirror of the square
		void flip()
		{
			_file = 9 - _file;
			_rank = 9 - _rank;
		}
		/* queries */
		// return true if params equals file and rank
		bool is(ordinal_t f, ordinal_t r) const
		{
			return (_file == f && _rank == r);
		}

	private:
		ordinal_t _file, _rank;
	};
	std::ostream & operator  <<(std::ostream &, const Square&) ;

	class SquareFactory {
		static Square createFromStream(std::istream &);
	};


    inline bool operator < (const Square & lhs, const Square & rhs)
	{
		return lhs.index() < rhs.index();
	}


	// union = U
	// intersect = I
	class SquareSet:public std::set<Square>
	{
	public:
		// inserts the valid neighbours of the middle square into this
		void insert_diag_neighbours(const Square& middle);
		// inserts the valid second neighbours of the middle square into this
		void insert_diag_second_neighbours(const Square& middle);
		// adds s to this if s is valid
		void operator += (const Square &s)
		{ if (s.is_valid()) insert(s); }
		// this = this U (color squares of rank(r))
		void insert_rank(const ordinal_t r, const Square::color_t color);
	private:

		bool contains(const Square & s) const
		{
			return find(s) != end();
		}
		/* intersect_count returns the number of elements in this that * that 
		are also in s. */
		int intersect_count(const SquareSet & s) const;
		// this = this U s
		void insert_set(const SquareSet & s);

		// t = this I row(r): rows starts from 1
		void intersect_rank(SquareSet & t, const ordinal_t r) const;

		// t = this I s
		void set_intersect(SquareSet & t, const SquareSet & s) const;

		// t = this U s
		void set_union(SquareSet & t, const SquareSet & s) const;

		/* flip Changes the state to the other players view */
		void flip();

		

		// removes the squares of the given color
		void remove_by_color(const Square::color_t color);

	};
	std::ostream & operator  <<(std::ostream &, const SquareSet &);
	std::istream & operator  >>(std::istream &, SquareSet &);

	// A MoveString provides a string representation for a move
	// A move contains commands and squares
	class MoveString 
	{
	public:
		// the text of the move
		const std::string& text() const; 
		void add(const Square & from, const Square & to);
		void prepend(const Square & from);
	private:
		void add(char command);
		void prepend(char command);
	private:
		std::string _text;
	};
	std::ostream & operator  <<(std::ostream & o, const MoveString & s);


	class State;
	class StateCollection : public std::list<State *> {
		PREVENT_COPY(StateCollection)
	public:
		StateCollection() {}
		// returns the state with the maximum value. 
		// it the collection is empty, returns end()
		iterator get_max_valued();
		iterator get_min_valued();
		iterator get_random_max_valued();
		void sort_value_increasing();
		void sort_value_decreasing();
		void sort_ply_decreasing();
		void append(const StateCollection& s)
		{ insert(end(),s.begin(),s.end()); }
	};

	std::ostream & operator  <<(std::ostream &, const StateCollection &);

	enum outcome_type { otNone, otWin, otLose, otDraw, otUnknown };

	std::ostream & operator  <<(std::ostream &, const outcome_type &);

	class State {
		PREVENT_COPY(State);
		friend class IGame; // for access to destroy_ply()
	public: // types
		enum {NO_VALUE = -32000 };
	public:		
		// a description of the move to get to this state
		// from its parent
		const MoveString& move() const	{ return _move; }
		// the collection of states that follow this one
		// next_ply generates the next ply if it does not exist
		StateCollection & next_ply();
		const State *parent() const {return _parent;}
		State *root() { if (_parent) return _parent->root(); else return this; }  
		State *parent() {return _parent;}
		int ply() const { return _ply; }
		virtual ~State();
		virtual void write_to(std::ostream &s) const = 0;
		outcome_type endgame() const
		{ 
			if (_end_game == otUnknown) _end_game = get_endgame();
			return _end_game;
		}

		bool is_endgame_node() const
		{ return endgame() != otNone; }

		void set_score(const int s) { _score = _value = s; }
		int score() const { return _score; }
		bool has_score() const { return _score != NO_VALUE; }
		void set_value(const int v) { _value = v; }
		int value() const { return _value; }
		bool is_first_player_active() const { return _ply%2 == 0; }
		size_t next_count() const { return _next_ply.size(); }
		virtual unsigned int phase() const = 0;
	protected:
		virtual outcome_type get_endgame() const = 0;
		MoveString _move;
		bool moves_generated() const
		{ return _moves_generated; }
		State();
		void set_parent(State * p);

	protected:  // to be overridden
		virtual void get_available_moves(StateCollection & target) = 0;
		mutable outcome_type _end_game;
	private:
		// the score is the independant value of the state
		// intial value is NO_VALUE.
		// user has_score() to determine if value is set.
		int _score;  

		int _value;  // the value is the rolled_up value of the state 
		// (initially the same as score
		bool _moves_generated; 
		State * _parent;
		int _ply;
		StateCollection _next_ply;
		// free all states in _next_ply, except for keep
		//void destroy_ply(State *keep);  
		void destroy_ply();  
	};


	class Player
	{
	public:
		/* select the next move from pos */
		virtual State * select_move(State & pos) = 0;
		virtual bool need_text() const
		{
			return false;
		}
		virtual ~Player() {} ;
		virtual void write_to(std::ostream &os) const = 0;
	};

	inline std::ostream& operator << (std::ostream& os, const Player& p)
	{ p.write_to(os); return os; }


	class PlayLine
	{
		PREVENT_COPY(PlayLine)
	public:
		enum eOutcome { ocWin, ocLose, ocDraw };
	public:
		/* this constructor constructs a playline from the last move backward */
		PlayLine(State &final_state, const bool first_player_moved_last);
		~PlayLine();
		// outcome from first player's perspective
		eOutcome outcome() const		{ return _outcome; }
		int move_count() const		{ return _moves.size(); }
		const StateCollection & moves() const		{ return _moves; }
	private:
		StateCollection _moves;
		eOutcome _outcome;
	};

	std::ostream & operator  <<(std::ostream &, const PlayLine &);

	class IGame {
		PREVENT_COPY(IGame);
	public:
		// returns the playline of two competitors
		PlayLine* compete(Player * first, Player * second);
		virtual void start() = 0;
		// describes the game type
		const char * description() {return mDescription.c_str(); };
		virtual ~IGame() {}
	protected:
		IGame(const std::string description);
		virtual void set_current(State * value) = 0;
		virtual State & current() = 0;
		virtual const State & current() const = 0;
	private:
		void apply_move(State * next);
		const std::string mDescription;
	};

	class MatchResults
	{
	public:
		MatchResults() { clear(); }
		void clear() {wins = loses = draws = 0;}
		size_t wins;
		size_t loses;
		size_t draws;
		size_t games() const { 
			return wins+loses+draws; 
		}
		float score() const { 
			size_t t = games();
			return ((t*1.0F + wins - loses)*100.0F) / (t*2.0F); 
		}
		MatchResults& operator += (const MatchResults& rhs)
		{
			wins += rhs.wins;
			draws += rhs.draws;
			loses += rhs.loses;
			return *this;
		}
		bool won() const {return wins > loses; }
		bool lost() const {return wins < loses; }
	};

	/* 
	A contest involves at n * 2 games.
	The second provided in the contest must has some random element
	in its strategy.  The score is given from the first player's perspective
	*/
	class Contest
	{
	public:
		// g is owned by the caller.
		Contest(IGame * g, const int _game_count);
		void compete(Player * p1, Player * p2, MatchResults &target);
	private:
		IGame *_game;
		int _game_count;
	};


	class Competitors
	{
	public:
		Competitors() { _age = 0; }
		// add a player -- owned by this
		void add(Player * source);
		// remove all evaluators from the competitor list
		void clear();
		// play against competitors
		void get_score_of(game::IGame * game, Player * player, MatchResults &r) const;
		// play all competitors against each other and set results
		// the result vecot is reinitialised
		void get_scores(game::IGame * game, std::vector<MatchResults> &results) const;
		~Competitors();
		size_t size() const { return _players.size(); }
		int age() const {return _age; }
	private:
		typedef std::vector<Player *> player_list_t;
		player_list_t _players;
		int _age;

	};

}


