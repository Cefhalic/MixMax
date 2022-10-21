#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <deque>
#include <stdexcept>
// #include <fstream>

constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

inline constexpr uint64_t Rotate_61bit( const uint64_t& aVal , const std::size_t& aSize )
{
  return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
}

inline uint64_t MOD_MERSENNE( const uint64_t& aVal )
{
  // if ( aVal >= M61 ) return aVal - M61;
  return aVal % M61; 
 //return (aVal & M61) + ((aVal >> 61)&0x7); 
}



// ==========================================
class base_signal
{
public:
  static std::deque< base_signal* > mAll;
  static const uint32_t mClockCnt;  
  static uint32_t mClockRd , mClockWr;

public:

  static void clock()
  {
    mClockRd = mClockWr;
    mClockWr = ( mClockWr + 1 ) % mClockCnt;
    for( auto& i : mAll )  i->update();
  }

  virtual void update() = 0;
};

uint32_t base_signal::mClockRd( 0 );
uint32_t base_signal::mClockWr( 1 );
const uint32_t base_signal::mClockCnt( 16 );

std::deque< base_signal* > base_signal::mAll;
// ==========================================




// ==========================================
template <typename T>
class signal : public base_signal
{
public:
  T mValue[ mClockCnt ];

public:

  signal( const T& aValue = T() ) : base_signal()
  {
    for( auto i(0) ; i!=mClockCnt ; ++i ) mValue[ i ] = aValue;
    mAll.push_back( this );
  }

  signal( const std::initializer_list<T>& aInitializerList ) : base_signal()
  {
    T* lPtr = mValue;
    if( aInitializerList.size() > mClockCnt ) throw std::out_of_range( "Signal initializer list exceeds array size" );
    for( auto lIt = aInitializerList.begin() ; lIt != aInitializerList.end() ; ++lIt , ++lPtr ) *lPtr = *lIt;
    mAll.push_back( this );
  }

  void update(){ mValue[ mClockWr ] = mValue[ mClockRd ]; }

  const T& operator* () const { return mValue[ mClockRd ]; }

  const T& operator[] ( const int32_t& i ) const { 
    if( i == -1 )
    {
      std::cout << "Warning: looking at write value" << std::endl;  
      return mValue[ mClockWr ];
    }

    if( i < 0 or i >= mClockCnt-1) throw std::out_of_range( "Signal index out of range" );
    return mValue[ ( mClockRd - i ) % mClockCnt ];
  }

  void operator= ( const        T&  aValue ) { mValue[ mClockWr ] =        aValue;            }
  void operator= ( const signal<T>& aOther ) { mValue[ mClockWr ] = aOther.mValue[ mClockRd ]; }

  friend std::ostream& operator<< ( std::ostream& aStr , const signal<T>& a ) { aStr << std::hex << std::setfill('0') << std::setw(16) << a.mValue[ mClockWr ]; return aStr; }

  // friend T operator+ ( const signal<T>& a , const signal<T>& b ) { return *a + *b ; }
  // friend T operator+ ( const signal<T>& a , const         T & b ) { return *a +  b ; }
  // friend T operator+ ( const         T & a , const signal<T>& b ) { return  a + *b ; }

  // friend T operator- ( const signal<T>& a , const signal<T>& b ) { return *a - *b ; }
  // friend T operator- ( const signal<T>& a , const         T & b ) { return *a -  b ; }
  // friend T operator- ( const         T & a , const signal<T>& b ) { return  a - *b ; }

  // template < typename U >
  // friend auto operator<< ( const signal<T>& a , const U& b ) -> decltype( *a << b ) { return *a << b ; }
};
// ==========================================


static const int offset = 2;


struct rng_state_t
{
  signal< uint64_t> W;
  // signal< uint64_t> X[2];
  int counter;
  signal< uint64_t> PartialSumOverOld , SumOverNew; 
  signal< uint64_t> RotatedPreviousPartialSumOverOld , PreSum; // , PreSum2 , PreSum2clk;
  //bool FirstPass;
  // std::ofstream File;

  rng_state_t() : W(1) , // X{ 1 , 1 } ,
  counter( -offset ) , PartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 ) //, PreSum2( 0 ),
  //FirstPass( true )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    

    // ===================================================================================
    // Three clock-cycles ahead
    auto nextcounter = (counter+offset) % 16;
    if( nextcounter == 0 )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = W[15-offset];
    }
    else if( nextcounter > 0 )
    {
      RotatedPreviousPartialSumOverOld = Rotate_61bit( *PartialSumOverOld , 36 );
      PartialSumOverOld = *PartialSumOverOld + W[15-offset]; 
    } 
    // ===================================================================================

    // ===================================================================================
    // Two clock-cycles ahead
    PreSum = MOD_MERSENNE( *PartialSumOverOld + *RotatedPreviousPartialSumOverOld );
    // ===================================================================================

    // ===================================================================================
    // One clock-cycle ahead
    // PreSum2 = MOD_MERSENNE( *PreSum );
    // ===================================================================================

    // PreSum2clk = PreSum2;
    // PreSum2clk2 = PreSum2clk;

    auto Temp = *SumOverNew + *PreSum;

    // ===================================================================================
    // Current clock
    if( counter == 0 )
    {
      // X[0] = RetVal = MOD_MERSENNE( *SumOverNew + *PreSum2 );
      // X[1] = MOD_MERSENNE( *SumOverNew );

      W = RetVal = MOD_MERSENNE( Temp );
      SumOverNew = MOD_MERSENNE( *SumOverNew + Temp );
    }
    else if( counter > 0 )
    { 
      // X[0] = RetVal = MOD_MERSENNE( *X[1] + *PreSum2 + *PreSum2clk );
      // X[1] = X[0];

      W = RetVal = MOD_MERSENNE( *W + *PreSum );
      SumOverNew = MOD_MERSENNE( *W + Temp );
    }
    // ===================================================================================

    // std::cout << counter << " | " << W[-1] << " | " << X[0][-1] << " | " << X[1][-1] << std::endl;


    counter = (counter+1) % 16;
    return RetVal;
  }


//   // friend std::ostream& operator<< ( std::ostream& aStr , const rng_state_t& x )
//   // {
//   //   aStr << std::setfill('0') << std::setw(5) << x.counter << " " << std::hex << std::setw(16) << x.PartialSumOverOld << " " << std::setw(16) << x.SumOverNew << " " << std::setw(16) << x.RotatedPreviousPartialSumOverOld << " " << std::setw(16) << x.PreSum << std::dec;
//   // }


//   uint64_t get()
//   {
//     // ===================================================================================
//     // Current clock


//     // std::cout << SumOverNew.mValue[0] << " | " << SumOverNew.mValue[1] << std::endl;
//     // std::cout << PreSumClk.mValue[0] << " | " << PreSumClk.mValue[1] << std::endl;
//     // std::cout << W[0].mValue[0] << " | " << W[0].mValue[1] << std::endl;
//     // std::cout << "---------------------------------------" << std::endl;


//     if( counter == 0 )
//     {
//       W[ 0] = (*SumOverNew) + (*PreSumClk) ;
//       W[15] = SumOverNew;      
//       SumOverNew = MOD_MERSENNE( ((*SumOverNew)<<1) + (*PreSumClk) );
//     }
//     else if( counter > 0 )
//     {
//       W[counter] =                                 PreSumX ;
//       SumOverNew = MOD_MERSENNE( (*SumOverNew) + (*PreSumX) );
//     }

//     uint64_t RetVal(0xDEADBEEF);
//     if( counter >= 0 )
//     {
//       auto& x = W[counter].mValue[ !lClock ];
//       V[counter] = MOD_MERSENNE( x );
//       RetVal = x;
//     }    
//     // ===================================================================================

//     PreSumX = (*W[(counter+15)%16]) + (*PreSum) + (*PreSumClk);
//     PreSumClk = PreSum;

//     // ===================================================================================
//     // Two clock-cycle ahead
//     PreSum = (*PartialSumOverOld) + (*RotatedPreviousPartialSumOverOld);
//     // ===================================================================================

//     // ===================================================================================
//     // Three clock-cycles ahead
//     auto nextcounter = (counter+3) % 16;
//     if( nextcounter == 0 )
//     {
//       RotatedPreviousPartialSumOverOld = 0;
//       PartialSumOverOld = *(V[nextcounter]);
//     }
//     else if( nextcounter > 0 )
//     {
//       RotatedPreviousPartialSumOverOld = Rotate_61bit( *PartialSumOverOld , 36 );
//       PartialSumOverOld = (*PartialSumOverOld) + *(V[nextcounter]); 
//     } 
//     // ===================================================================================

//     counter = (counter+1) % 16;
//     return RetVal;
//   }
      
};





int main()
{
    std::cout << std::setfill('0');

    rng_state_t lState;
    for( int i(0) ; i!=41+offset ; ++i )
    {
      base_signal::clock();
      lState.get();
      // std::cout << std::hex << std::setw(16) << lState.get() << std::endl;

      std::cout << "W=" << lState.W
                << " | PartialSumOverOld=" << lState.PartialSumOverOld 
                << " | RotatedPreviousPartialSumOverOld=" << lState.RotatedPreviousPartialSumOverOld 
                << " | PreSum=" << lState.PreSum
                << " | SumOverNew=" << lState.SumOverNew 
                << std::endl; 
    }

    return 0;
}
