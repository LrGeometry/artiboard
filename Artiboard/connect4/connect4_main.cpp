#include <test_util.h>
#include <experiment.h>

int main(int argc, char* argv[])
{
	const std::string name(argv[1]);
#ifndef NDEBUG
	if (name == "test") {
  		test_main();
		return 0;
  } else
#endif
		return arti::experi_main(argc,argv);
}


