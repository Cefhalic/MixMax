#include <stdint.h>
#include <iostream>
#include <iomanip>

constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

inline constexpr uint64_t Rotate_61bit( const uint64_t& aVal , const std::size_t& aSize )
{
  return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
}

inline constexpr uint64_t MOD_MERSENNE( const uint64_t& aVal )
{
 return (aVal & M61) + (aVal >> 61); 
}






struct rng_state_t
{
  uint64_t V[ 16 ];
  int counter;
  uint64_t PartialSumOverOld , SumOverNew; 
  uint64_t RotatedPreviousPartialSumOverOld , PreSum;
  

  rng_state_t() : V{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ,
  counter( -2 ) , PartialSumOverOld( 0 ) , SumOverNew( M64 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 )
  {}


  friend std::ostream& operator<< ( std::ostream& aStr , const rng_state_t& x )
  {
    aStr << std::setfill('0') << std::setw(5) << x.counter << " " << std::hex << std::setw(16) << x.PartialSumOverOld << " " << std::setw(16) << x.SumOverNew << " " << std::setw(16) << x.RotatedPreviousPartialSumOverOld << " " << std::setw(16) << x.PreSum << std::dec;
  }

  uint64_t get()
  {
    // ===================================================================================
    // Current clock
    auto Temp = SumOverNew + PreSum;

    if( counter == 0 )
    {
      V[counter] = MOD_MERSENNE( Temp );
      SumOverNew = MOD_MERSENNE( SumOverNew + Temp );
    }
    else if( counter > 0 )
    { 
      V[counter] = MOD_MERSENNE( V[counter-1] + PreSum );
      SumOverNew = MOD_MERSENNE( V[counter-1] + Temp );
    }
    // ===================================================================================
    
    // ===================================================================================
    // One clock-cycle ahead
    auto nextcounter = (counter+1) % 16;
    PreSum = MOD_MERSENNE( PartialSumOverOld + RotatedPreviousPartialSumOverOld );
    // ===================================================================================

    // ===================================================================================
    // Two clock-cycles ahead
    nextcounter = (counter+2) % 16;
    if( nextcounter == 0 )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = V[nextcounter];
    }
    else if( nextcounter > 0 )
    {
      RotatedPreviousPartialSumOverOld = Rotate_61bit( PartialSumOverOld , 36 );
      PartialSumOverOld = MOD_MERSENNE( PartialSumOverOld + V[nextcounter] ); 
    } 
    // ===================================================================================

    uint64_t RetVal(0xDEADBEEF);
    if( counter >= 0 ) RetVal = V[counter];
    counter = (counter+1) % 16;
    return RetVal;
  }
      
};





int main()
{
    std::cout << std::setfill('0');

    rng_state_t lState;
    for( int i(0) ; i!=43 ; ++i ) std::cout << std::hex << std::setw(16) << lState.get() << std::endl;

    return 0;
}



// #include <stdint.h>
// #include <iostream>
// #include <iomanip>
// #include <fstream>

// constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
// constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

// inline constexpr uint64_t Rotate_61bit( const uint64_t& aVal , const std::size_t& aSize )
// {
//   return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
// }

// inline uint64_t MOD_MERSENNE( const uint64_t& aVal )
// {
//  //return (aVal & M61) + (aVal >> 61);
//   if( aVal > M61 ) return aVal - M61;
//   return aVal;
// }






// struct rng_state_t
// {
//   uint64_t V[ 16 ];
//   int counter;
//   uint64_t PartialSumOverOld , SumOverNew; 
//   uint64_t RotatedPreviousPartialSumOverOld , PreSum , PreSum2 , PreSumX;
//   std::ofstream File;


//   rng_state_t() : V{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ,
//   counter( -3 ) , PartialSumOverOld( 0 ) , SumOverNew( M64 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 ) , PreSum2( 0 ) , PreSumX( 0 ),
//   File( "mixmax.txt" )
//   {}


//   friend std::ostream& operator<< ( std::ostream& aStr , const rng_state_t& x )
//   {
//     aStr << std::setfill('0') << std::setw(5) << x.counter << " " << std::hex << std::setw(16) << x.PartialSumOverOld << " " << std::setw(16) << x.SumOverNew << " " << std::setw(16) << x.RotatedPreviousPartialSumOverOld << " " << std::setw(16) << x.PreSum << std::dec;
//   }

//   uint64_t get()
//   {
//     File << std::setfill('0');
//     File << std::dec << std::setw(2) << counter << std::hex << " | " << std::setw(16) << SumOverNew << " | " << std::setw(16) << PreSumX << " " << std::setw(16) << PreSum2 << " |";
//     for( auto i(0); i!=16; ++i ) File << " " << std::setw(16) << V[i];
//     File << std::endl;


//     auto V_counter =

//     // ===================================================================================
//     // Current clock
//     if( counter == 0 )
//     {
//       V[counter] = MOD_MERSENNE(  SumOverNew     + PreSum2 );
//       SumOverNew = MOD_MERSENNE( (SumOverNew<<1) + PreSum2 );
//     }
//     else if( counter > 0 )
//     {
//       V[counter] = MOD_MERSENNE(              PreSumX );
//       SumOverNew = MOD_MERSENNE( SumOverNew + PreSumX );      
//     }
//     // ===================================================================================

//     // ===================================================================================
//     // One clock-cycle ahead
//     //auto nextcounter = (counter+1) % 16;
//     if( counter >= 0 ) PreSumX = PreSum + V[counter];
//     PreSum2 = PreSum;
//     // ===================================================================================

//     // ===================================================================================
//     // Two clock-cycle ahead
//     auto nextcounter = (counter+2) % 16;
//     PreSum = PartialSumOverOld + RotatedPreviousPartialSumOverOld;
//     // ===================================================================================

//     // ===================================================================================
//     // Three clock-cycles ahead
//     nextcounter = (counter+3) % 16;
//     if( nextcounter == 0 )
//     {
//       RotatedPreviousPartialSumOverOld = 0;
//       PartialSumOverOld = V[nextcounter];
//     }
//     else if( nextcounter > 0 )
//     {
//       RotatedPreviousPartialSumOverOld = Rotate_61bit( PartialSumOverOld , 36 );
//       PartialSumOverOld = PartialSumOverOld + V[nextcounter]; 
//     } 
//     // ===================================================================================

//     uint64_t RetVal(0xDEADBEEF);
//     if( counter >= 0 ) RetVal = V[counter];
//     counter = (counter+1) % 16;
//     return RetVal;
//   }
      
// };





// int main()
// {
//     std::cout << std::setfill('0');

//     rng_state_t lState;
//     for( int i(0) ; i!=10 ; ++i ) std::cout << std::hex << std::setw(16) << lState.get() << std::endl;

//     return 0;
// }
