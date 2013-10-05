#include <test_util.h>

int main(int argc, char* argv[])
{
  try {
  	test_main();
  } catch (std::exception &ex) {
    std::cout << "*** ERROR ***: " << ex.what() << std::endl;
  }
  return 0;
}
