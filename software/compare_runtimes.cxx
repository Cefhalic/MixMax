// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

#include "mixmax_clean.hpp"
#include "mixmax_orig.hpp" // Must come last because of all the preprocessor directives

#include <iostream>
#include <iomanip>
#include <chrono>
using namespace std::chrono;

uint64_t TimeOriginal( const uint64_t& aIterations )
{
  rng_state_t lStateOrig{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
  lStateOrig.sumtot = iterate_raw_vec( lStateOrig.V , lStateOrig.sumtot );

  auto start = steady_clock::now();
  for( uint64_t i(0) ; i != aIterations ; ++i ) flat( &lStateOrig );
  auto stop = steady_clock::now(); 
  auto duration = duration_cast<nanoseconds>(stop - start).count();
  std::cout << "Original  | " << aIterations << " iterations | " << duration << "ns | " << double(duration)/aIterations << "ns/it" << std::endl;
  return flat( &lStateOrig );
}

uint64_t TimeCleaned( const uint64_t& aIterations )
{
  clean::rng_state_t lStateClean;

  auto start = steady_clock::now();
  for( uint64_t i(0) ; i != aIterations ; ++i ) lStateClean.get();
  auto stop = steady_clock::now(); 
  auto duration = duration_cast<nanoseconds>(stop - start).count();
  std::cout << "Cleaned 1 | " << aIterations << " iterations | " << duration << "ns | " << double(duration)/aIterations << "ns/it" << std::endl;
  return lStateClean.get();
}

uint64_t TimeCleaned2( const uint64_t& aIterations )
{
  clean::rng_state_t lStateClean;

  auto start = steady_clock::now();
  for( uint64_t i(0) ; i != aIterations ; ++i ) lStateClean.get2();
  auto stop = steady_clock::now(); 
  auto duration = duration_cast<nanoseconds>(stop - start).count();
  std::cout << "Cleaned 2 | " << aIterations << " iterations | " << duration << "ns | " << double(duration)/aIterations << "ns/it" << std::endl;
  return lStateClean.get2();
}

int main()
{
  std::cout << "Timing original and cleaned implementations" << std::endl;

  constexpr uint64_t i( 1e10 );
  uint64_t x(0);
  x ^= TimeOriginal( i );
  x ^= TimeCleaned( i );
  x ^= TimeCleaned2( i );

  return 0;
}


