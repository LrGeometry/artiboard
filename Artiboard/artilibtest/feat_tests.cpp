#include <feat.h>
#include <test_util.h>
#include <log.h>

#define TESTDATA FeatData

using namespace arti;
const string base_dir("../artilibtest/data/");

namespace tut {

struct FeatData{
	FeatData() {
	}
};

test_group<FeatData> featTests("100 Feat Language Tests");

BEGIN(1,"Set of correct statements must parse OK") 
  auto program = load_program(base_dir + "feat_language_test.txt");
  LOG << *program;
  ensure_equals(4,program->states().locate("a1").size());
  ensure_equals(2,program->regions().locate("a2").size());
END


}

