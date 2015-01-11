#ifndef _MSC_BUILD
#include <test_util.h>
#endif
#include <experiment.h>

int main(int argc, char* argv[])
{
#ifndef _MSC_BUILD
	const std::string name(argv[1]);
#ifndef NDEBUG
	if (name == "test") {
  		test_main();
		return 0;
  } else
#endif
#endif
		return arti::experi_main(argc,argv);
}


