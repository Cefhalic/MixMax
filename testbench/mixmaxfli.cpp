// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

#include "mti.h"
#include "mixmax2.hpp"

// create one struct that contains all vhdl signals that need to be passed to the function
typedef struct
{
    mtiSignalIdT clk;
    mtiDriverIdT lo , hi;
} port_t;

// the process function that will be called on each event (in this case only clk)
static void MixmaxProcess( void *param )
{   
    port_t * ip = (port_t *) param;                 // connect function argument to port struct
    mtiInt32T clk = mti_GetSignalValue ( ip->clk ); // get current values from the vhdl world

    static tRngState<false> lState;
    if( clk )
    {
        BaseVhdlSignal::clock();
        uint64_t lVal = lState.get();
        mti_ScheduleDriver( ip->lo , ( lVal >>  0 ) & 0x7FFFFFFF , 0 , MTI_INERTIAL ); // slightly perverse, but it appears that the modelsim FLI cannot cope
        mti_ScheduleDriver( ip->hi , ( lVal >> 31 ) & 0x3FFFFFFF , 0 , MTI_INERTIAL ); // with integers that use the MSB of a word
    }
}


extern "C" {  // only need to export C interface if used by C++ source code
    void MixMaxFli( mtiRegionIdT region, // location in the design
                    char *parameters, // from vhdl world (not used)
                    mtiInterfaceListT *generics, // from vhdl world (not used)
                    mtiInterfaceListT *ports ) // linked list of ports
    {
        // create a struct to store a link for each vhdl signal
        port_t *ip = (port_t *) mti_Malloc( sizeof(port_t) );
        // map input signals (from vhdl world) to struct
        ip->clk  = mti_FindPort( ports , (char*)("clk") );

        // map "cnt" output signal (to vhdl world) to struct
        ip->lo  = mti_CreateDriver( mti_FindPort( ports , (char*)("lo") ) );
        ip->hi  = mti_CreateDriver( mti_FindPort( ports , (char*)("hi") ) );

        // create "MixmaxProcess" process with a link to all vhdl signals where the links to the vhdl signals are in the struct
        mtiProcessIdT process_id = mti_CreateProcess( (char*)("MixmaxProcess") , MixmaxProcess , ip );
        // trigger “MixmaxProcess” process when event on vhdl signal clk
        mti_Sensitize( process_id , ip->clk , MTI_EVENT );
    }
}
