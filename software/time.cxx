#include "mixmax_orig_clean.hpp"
#include "mixmax_orig.hpp"
#include <chrono>
using namespace std::chrono;

#include <iostream>
#include <iomanip>


void TimeOriginal( const uint64_t& aIterations )
{
  rng_state_t lStateOrig{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
  lStateOrig.sumtot = iterate_raw_vec( lStateOrig.V , lStateOrig.sumtot );

  auto start = high_resolution_clock::now();
  for( uint64_t i(0) ; i != aIterations ; ++i ) flat( &lStateOrig );
  auto stop = high_resolution_clock::now(); 
  auto duration = duration_cast<nanoseconds>(stop - start).count();
  std::cout << "Original  | " << aIterations << " iterations | " << duration << "ns | " << double(duration)/aIterations << "ns/it" << std::endl;
}

void TimeCleaned( const uint64_t& aIterations )
{
  clean::rng_state_t lStateClean;

  auto start = high_resolution_clock::now();
  for( uint64_t i(0) ; i != aIterations ; ++i ) lStateClean.get();
  auto stop = high_resolution_clock::now(); 
  auto duration = duration_cast<nanoseconds>(stop - start).count();
  std::cout << "Cleaned 1 | " << aIterations << " iterations | " << duration << "ns | " << double(duration)/aIterations << "ns/it" << std::endl;
}

void TimeCleaned2( const uint64_t& aIterations )
{
  clean::rng_state_t lStateClean;

  auto start = high_resolution_clock::now();
  for( uint64_t i(0) ; i != aIterations ; ++i ) lStateClean.get2();
  auto stop = high_resolution_clock::now(); 
  auto duration = duration_cast<nanoseconds>(stop - start).count();
  std::cout << "Cleaned 2 | " << aIterations << " iterations | " << duration << "ns | " << double(duration)/aIterations << "ns/it" << std::endl;
}

int main()
{
  std::cout << "Timing original and cleaned implementations" << std::endl;

  uint64_t i( 1e10 );
  TimeOriginal( i );
  TimeCleaned( i );
  TimeCleaned2( i );

  return 0;
}


