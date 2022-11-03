#include <stdint.h>
#include <iostream>
#include <iomanip>

// #define unlikely(expr) __builtin_expect(!!(expr), 0)

namespace clean
{

  constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );

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
    uint64_t SumOverNew , PartialSumOverOld;

    rng_state_t() : V{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } , counter( 15 ) , SumOverNew( 1 ) , PartialSumOverOld( 0 )
    {}

    // Update per call method
    inline volatile uint64_t get()
    {
      if( counter != 15 )
      {
        counter += 1;
        uint64_t RotatedPreviousPartialSumOverOld( Rotate_61bit( PartialSumOverOld , 36 ) );
        PartialSumOverOld = MOD_MERSENNE( PartialSumOverOld + V[counter] ); 
        V[counter] = MOD_MERSENNE( V[counter-1] + PartialSumOverOld + RotatedPreviousPartialSumOverOld );
      }
      else
      {
        counter = 0;        
        PartialSumOverOld = V[counter]; 
        V[counter] = MOD_MERSENNE( SumOverNew + PartialSumOverOld );
      }

      SumOverNew = MOD_MERSENNE( SumOverNew + V[counter] ); 

      return V[counter];
    }   

    // Batch-update, more like the original
    inline volatile uint64_t get2()
    {
      if( counter != 15 )
      {
        counter += 1;
      }
      else
      {
        PartialSumOverOld = V[0];
        auto lV = V[0] = MOD_MERSENNE( SumOverNew + PartialSumOverOld );
        SumOverNew = MOD_MERSENNE( SumOverNew + lV );

        for( int i(1); i!=16; ++i )
        {
          auto lRotatedPreviousPartialSumOverOld = Rotate_61bit( PartialSumOverOld , 36 );
          PartialSumOverOld = MOD_MERSENNE( PartialSumOverOld + V[i] ); 
          lV = V[i] = MOD_MERSENNE( lV + PartialSumOverOld + lRotatedPreviousPartialSumOverOld );
          SumOverNew = MOD_MERSENNE( SumOverNew + lV );  
        }

        counter = 0;
      }

      return V[counter];
    }   


  };


}