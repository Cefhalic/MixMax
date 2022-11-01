#include "mixmax2.hpp"
#include "mixmax_orig.hpp"

#include <iostream>
#include <iomanip>

int main()
{
    std::cout << "Comparing original implementation to cycle-accurate implementation \n                 iterations\r" << std::flush;

    rng_state_t lStateOrig{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
    lStateOrig.sumtot = iterate_raw_vec( lStateOrig.V , lStateOrig.sumtot );

    tRngState lState;

    for( int i(0) ; i!=4 ; ++i )
    { 
      // The clock-cycle accurate implementation
      base_signal::clock();
      auto lNew  = lState.get();
    }


    for( uint64_t i(0) ; ; ++i )
    { 
      // The original implementation
      auto lOrig = flat( &lStateOrig );

      // The clock-cycle accurate implementation
      base_signal::clock();
      auto lNew  = lState.get();

      if( i%1000 == 0 ) std::cout << std::dec << std::setw(16) << i << "\r" << std::flush;
      if( lNew != lOrig )
      {
        std::cout << std::dec << std::setw(16) << i << " " << std::hex << std::setfill('0') << std::hex << std::setw(16) << lOrig << " " <<  std::setw(16) << lNew << std::endl;
        return 1;
      }
    }

    return 0;

}


