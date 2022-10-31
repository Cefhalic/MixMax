#include "mti.h"
#include <stdio.h>
#include "mixmax2.hpp"

// create one struct that contains all vhdl signals that need to be passed to the function
typedef struct
{
    mtiSignalIdT clk;
    mtiDriverIdT lo , hi;
} port_t;


// the process function that will be called on each event (in this case only clk)
static void counter( void *param )
{   
    port_t * ip = (port_t *) param;           // connect function argument to counter struct
    mtiInt32T clk = mti_GetSignalValue ( ip->clk ); // get current values from the vhdl world

    static tRngState lState;
    if( clk )
    {
        base_signal::clock();
        uint64_t lVal = lState.get();
        mti_ScheduleDriver( ip->lo , ( lVal >>  0 ) & 0xFFFFFFFF , 0 , MTI_INERTIAL );
        mti_ScheduleDriver( ip->hi , ( lVal >> 32 ) & 0xFFFFFFFF , 0 , MTI_INERTIAL );
    }


}


extern "C" {  // only need to export C interface if used by C++ source code
    void MixMaxFli(
        mtiRegionIdT region, // location in the design
        char *parameters, // from vhdl world (not used)
        mtiInterfaceListT *generics, // from vhdl world (not used)
        mtiInterfaceListT *ports // linked list of ports
    )
    {
        // create a struct to store a link for each vhdl signal
        port_t *ip = (port_t *) mti_Malloc( sizeof(port_t) );
        // map input signals (from vhdl world) to struct
        ip->clk  = mti_FindPort( ports , (char*)("clk") );

        // map "cnt" output signal (to vhdl world) to struct
        ip->lo  = mti_CreateDriver( mti_FindPort( ports , (char*)("lo") ) );
        ip->hi  = mti_CreateDriver( mti_FindPort( ports , (char*)("hi") ) );

        // create "counter" process with a link to all vhdl signals where the links to the vhdl signals are in the struct
        mtiProcessIdT process_id = mti_CreateProcess( (char*)("counter_p") , counter , ip );
        // trigger “counter” process when event on vhdl signal clk
        mti_Sensitize( process_id , ip->clk , MTI_EVENT );
    }
}
