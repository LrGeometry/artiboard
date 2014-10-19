#include <experiment.h>
#include <negamax.h>
#include "connect4.h"
#include <log.h>
#include <thread>
#include <future>
using namespace arti;

class NegamaxExploration : public Experiment {
public:
	NegamaxExploration() : Experiment("c4-100","Does negamax alpha-beta have an impact on the items search? ") {}
protected:

	void do_step(int level, const char * fn_name, eval_function_t fn) {
		Board::u_ptr board(new Board());
		Connect4::spec.setup(*board);
		PositionThatOwns pos(0, std::move(board));
		Board::u_ptr_list boards;
		Connect4::spec.collectBoards(pos,boards);
		PickNegamaxAlphaBeta picker(&Connect4::spec,fn,level,false);
		picker.select(pos,boards);
		file() << fn_name << " " << level << " " << picker.walk_count() << " " << picker.value();
	}


	void do_run() override	{
		file() << "Function Depth Positions Value";
		for (int i=1;i<11;i++) 	{
			do_step(i,"WinLose",Connect4::win_lose);
			do_step(i,"IBEF",Connect4::StenMarkIBEF);
			do_step(i,"ADATE",Connect4::StenMarkADATE);
			do_step(i,"IBEFB",Connect4::StenMarkIBEFB);
			do_step(i,"ADATEB",Connect4::StenMarkADATEB);
		}
	}
} c4_100;

class NegamaxOrdered : Experiment {
	public:
		NegamaxOrdered() : Experiment("c4-200", "Explore the effect of ordering nodes in negamax"){}
	protected:
		void do_step(int level, const char * fn_name, eval_function_t fn, bool ordered) {
			Board::u_ptr board(new Board());
			Connect4::spec.setup(*board);
			PositionThatOwns pos(0, std::move(board));
			Board::u_ptr_list boards;
			Connect4::spec.collectBoards(pos,boards);
			PickNegamaxAlphaBeta picker(&Connect4::spec,fn,level,ordered);
			picker.select(pos,boards);
			file() << fn_name << (ordered?"-O ":" ") << level << " " << picker.walk_count() << " " << picker.value();
		}


		void do_run() override	{
			file() << "Function Depth Positions Value";
			for (int i=1;i<10;i++) 	{
				do_step(i,"WinLose",Connect4::win_lose,false);
				do_step(i,"IBEF",Connect4::StenMarkIBEF,false);
				do_step(i,"ADATE",Connect4::StenMarkADATE,false);
				do_step(i,"IBEFB",Connect4::StenMarkIBEFB,false);
				do_step(i,"ADATEB",Connect4::StenMarkADATEB,false);
				do_step(i,"WinLose",Connect4::win_lose,true);
				do_step(i,"IBEF",Connect4::StenMarkIBEF,true);
				do_step(i,"ADATE",Connect4::StenMarkADATE,true);
				do_step(i,"IBEFB",Connect4::StenMarkIBEFB,true);
				do_step(i,"ADATEB",Connect4::StenMarkADATEB,true);
			}
		  //pickLevel(2);
		}
} c4_200;

static arti::MatchOutcome play_m(Match& m) {
	return m.play();
}

// 7353:> Complete c4-350 - first and second player
class PerformanceMeasurements : Experiment {
	public:
		PerformanceMeasurements() : Experiment("c4-350","What affect does search depth have on performance? (2h)") {}
		void do_run() override {
			file() << "Function Depth Performance";
			do_step("IBEF-f",Connect4::StenMarkIBEF,true,false);
			do_step("ADATE-f",Connect4::StenMarkADATE,true,false);
			do_step("IBEF-s",Connect4::StenMarkIBEF,false,true);
			do_step("ADATE-s",Connect4::StenMarkADATE,false,true);
			// 1519:> Complete c4-350
		}
	private:
		void do_step(const string& fname, eval_function_t fn,const bool play_first, const bool play_second, const int s=1, const int e=6) {
			for (int p=s; p <= e;p++) {
				auto r = p_metric(fn,p,play_first,play_second);
				file() << fname << " " << p << " " << r;
				LOG << fname << " " << p << " " << r ;
			}
		}

		void adjust_counts(const MatchOutcome result,const MatchOutcome whoami, int &w, int &l) {
			CHECK(result != MatchOutcome::Unknown);
			if (result == whoami) w = w + 1;
			else if (result != MatchOutcome::Draw) l = l + 1;
		}

		float p_metric(eval_function_t fn, const int ply, const bool play_first, const bool play_second) {
			CHECK(play_first || play_second);
			const int N = 3000; // the number of 'pair' matches to play where pair = one match on each side
			const int LOAD = 16;  // the number of futures to submit asynchronously; depends on the number of cores available
			PickRandom randpick(Connect4::spec);
			int wp = 0;
			int lp = 0;
			int total = 0;
			using future_t = std::future<arti::MatchOutcome>;
			int count = 0;
			while (count < N) {
				std::list<future_t> flist,llist;
				int step = LOAD;
				if (step + count > N)
					step = N - count;
				for (int i=0;i<step;i++) {
					if (play_first) flist.emplace_front(
						std::async(std::launch::async,
							[&ply,&fn,&randpick](){
								PickNegamaxAlphaBeta negapick(&Connect4::spec,fn,ply);
								PickDual first(negapick,randpick);
								return Match(Connect4::spec,first).play();
							}));
					if (play_second) llist.emplace_front(
						std::async(std::launch::async,
							[&ply,&fn,&randpick](){
								PickNegamaxAlphaBeta negapick(&Connect4::spec,fn,ply);
								PickDual last(randpick,negapick);
								return Match(Connect4::spec,last).play();
							}));
				};
				for (auto &f : flist)
					adjust_counts(f.get(),SouthPlayerWins,wp,lp);
				for (auto &f : llist)
					adjust_counts(f.get(),NorthPlayerWins,wp,lp);
				count += step;
				total += (flist.size() + llist.size());
			}
			CHECK(count == N);
			return 100 * (total+wp-lp)/(total*2.0f);
		}
} c4_350;
