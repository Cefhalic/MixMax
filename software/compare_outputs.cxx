// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

#include "mixmax2.hpp"
#include "mixmax_clean.hpp"
#include "mixmax_orig.hpp" // Must come last because of all the preprocessor directives

#include <iostream>
#include <iomanip>

int main( int argc , char** argv )
{
    std::cout << "Comparing original implementation, both cleaned implementations and the cycle-accurate implementations \n                 iterations\r" << std::flush;

    rng_state_t lStateOrig{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
    lStateOrig.sumtot = iterate_raw_vec( lStateOrig.V , lStateOrig.sumtot );

    clean::rng_state_t lStateClean;
    clean::rng_state_t lStateClean2;

    tRngState lCycleState;

    for( int i(0) ; i!=6 ; ++i )
    { 
      // The clock-cycle accurate implementation
      base_signal::clock();
      auto lNew  = lCycleState.get();
    }

    for( int i(0) ; i!=1 ; ++i )
    { 
      auto lClean  = lStateClean.get();
      auto lClean2 = lStateClean2.get2();
    }

    for( uint64_t i(0) ; ; ++i )
    { 
      // The original implementation
      auto lOrig = flat( &lStateOrig );

      auto lClean = lStateClean.get();
      auto lClean2 = lStateClean2.get2();      

      // The clock-cycle accurate implementation
      base_signal::clock();
      auto lNew  = lCycleState.get();

      if( ! (i & 0x1FFFF) ) std::cout << std::dec << std::setw(16) << i << "\r" << std::flush;
      if( ( lClean != lOrig ) || ( lClean2 != lOrig ) || ( lNew != lOrig ) )
      {
        std::cout << std::dec << std::setw(16) << i << " " << std::hex << std::setfill('0') << "Original = " << std::setw(16) << lOrig << " | Clean = " << std::setw(16) << lClean << " | Clean 2 = " << std::setw(16) << lClean2 << " | New = " << std::setw(16) << lNew << std::endl;
        return 1;
      }
    }

    return 0;

}


