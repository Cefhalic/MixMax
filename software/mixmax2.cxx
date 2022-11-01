#include <iostream>
#include <iomanip>
#include "mixmax2.hpp"

int main()
{
  std::cout << std::setfill('0');

  tRngState lState;
  for( int i(0) ; i!=70 ; ++i )
  {
    base_signal::clock();
    std::cout << std::hex << std::setw(16) << lState.get() << std::endl;
  }

  return 0;
}
