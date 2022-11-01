#include "mixmax_orig.hpp"
#include <iostream>
#include <iomanip>

int main()
{
    std::cout << std::hex << std::setfill('0');

    rng_state_t lState{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
    lState.sumtot = iterate_raw_vec( lState.V , lState.sumtot );

    for( int i(0) ; i!=70 ; ++i ) std::cout << std::setw(16) << flat( &lState ) << std::endl;

    return 0;
}

