#include "mixmax2.hpp"
#include "mixmax_orig_clean.hpp"
#include "mixmax_orig.hpp"

#include <iostream>
#include <iomanip>

int main()
{
    std::cout << "Comparing original, cleaned and cycle-accurate implementations \n                 iterations\r" << std::flush;

    rng_state_t lStateOrig{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
    lStateOrig.sumtot = iterate_raw_vec( lStateOrig.V , lStateOrig.sumtot );

    clean::rng_state_t lStateClean;

    tRngState lState;

    for( int i(0) ; i!=4 ; ++i )
    { 
      // The clock-cycle accurate implementation
      base_signal::clock();
      auto lNew  = lState.get();
    }

    for( int i(0) ; i!=1 ; ++i ) auto lClean = lStateClean.get();

    for( uint64_t i(0) ; ; ++i )
    { 
      // The original implementation
      auto lOrig = flat( &lStateOrig );

      auto lClean = lStateClean.get();

      // The clock-cycle accurate implementation
      base_signal::clock();
      auto lNew  = lState.get();

      if( ! (i & 0x1FFFF) ) std::cout << std::dec << std::setw(16) << i << "\r" << std::flush;
      if( lNew != lOrig )
      {
        std::cout << std::dec << std::setw(16) << i << " " << std::hex << std::setfill('0') << "Original = " << std::hex << std::setw(16) << lOrig << " | Clean = " << std::setw(16) << lClean  << " | New = " << std::setw(16) << lNew << std::endl;
        return 1;
      }
    }

    return 0;

}


