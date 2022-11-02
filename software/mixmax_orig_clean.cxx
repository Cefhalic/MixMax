#include "mixmax_orig_clean.hpp"
#include <iostream>
#include <iomanip>

int main()
{
  std::cout << std::setfill('0');

  clean::rng_state_t lState;
  for( int i(0) ; i!=70 ; ++i )
  {
    std::cout << std::hex << std::setw(16) << lState.get() << std::endl;
  }

  return 0;
}
