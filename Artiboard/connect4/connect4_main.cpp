#include <test_util.h>
#include <experiment.h>

int main(int argc, char* argv[])
{
	const std::string name(argv[1]);
	if (name == "test")
		test_main();
	else
		return arti::experi_main(argc,argv);
  return 0;
}
