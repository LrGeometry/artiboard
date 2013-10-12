#include <tut/tut.hpp>
#include <board.h>
#include <exception>
#include <test_util.h>
#define TESTDATA BoardData
namespace tut {
	using namespace arti;

	struct BoardData {};
	test_group<BoardData> boardGameTests("015 BoardGame Tests");

	BEGIN(1, "Square equality and boundary")
		Board board;
		ensure_equals(board(1,1),board(0,1));
		ensure_equals(board(1,1),Piece::EMPTY);
		ensure_equals(board(-1,1),Piece::OUT_OF_BOUNDS);
		ensure_equals(board(11,1),Piece::OUT_OF_BOUNDS);
		ensure(board(1,1) == board(0,1));
	END

	BEGIN(2, "Square assignment")
		Board board;
		Piece test(9);
		board(1,1,test);
		ensure_equals(board(1,1),test);
	END

	BEGIN(3, "Bounds check")
		Board board;
		Piece test(9);
		try {
			board(3,8,test);
			fail("exception did not occur");
		} catch (std::runtime_error &ex) {
			ensure_contains(ex, "index out of bounds");
		}
	END

	BEGIN(4, "Bounds assignment")
		Board board;
		try {
			board(1,1,Piece::OUT_OF_BOUNDS);
			fail("exception did not occur");
		} catch (std::runtime_error &ex) {
			ensure_contains(ex, "cannot");
		}
	END

	BEGIN(5, "Neighbours of 0,7") 
		Region n;
		n.insert_neighbours(Square(0,7));
		ensure_equals(n.size(),3);
	END

	BEGIN(6, "Neighbours of 7,0") 
		Region n;
		n.insert_neighbours(Square(7,0));
		ensure_equals(n.size(),3);
	END

	BEGIN(7, "Neighbours of 0,0") 
		Region n;
		n.insert_neighbours(Square(0,0));
		ensure_equals(n.size(),3);
	END

	BEGIN(8, "Neighbours of 7,7") 
		Region n;
		n.insert_neighbours(Square(7,7));
		ensure_equals(n.size(),3);
	END

}
