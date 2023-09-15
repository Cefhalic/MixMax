// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <deque>

// ==========================================
class BaseVhdlSignal
{
public:
  static std::deque< BaseVhdlSignal* > mAll;
  static uint8_t mClock;

public:

  static void clock()
  {
    mClock = 1 - mClock;
    for( auto& i : mAll )  i->update();
  }

  virtual void update() = 0;
};

uint8_t BaseVhdlSignal::mClock( 0 );
std::deque< BaseVhdlSignal* > BaseVhdlSignal::mAll;
// ==========================================


// ==========================================
// #define UNARY_OPERATOR( arg1 , arg2 ) \
//   friend inline T arg1 ( const VhdlSignal<T>& a ) { return arg2 *a ; } 

// #define BINARY_OPERATOR( arg1 , arg2 ) \
//   friend inline T arg1 ( const VhdlSignal<T>& a , const VhdlSignal<T>& b ) { return *a arg2 *b ; } \
//   template <typename U> friend inline T arg1 ( const VhdlSignal<T>& a , const        U & b ) { return *a arg2  b ; } \
//   template <typename U> friend inline T arg1 ( const        U & a , const VhdlSignal<T>& b ) { return  a arg2 *b ; }

template <typename T>
class VhdlSignal : public BaseVhdlSignal
{
private:
  T mValue[ 2 ];

public:

  VhdlSignal( const T& aValue = T() ) : BaseVhdlSignal()
  {
    for( auto i(0) ; i!=2 ; ++i ) mValue[ i ] = aValue;
    mAll.push_back( this );
  }

  void update(){ mValue[ 1 - mClock ] = mValue[ mClock ]; }

  const T& operator* () const { return mValue[ mClock ]; }

  void operator= ( const        T&  aValue ) { mValue[ 1 - mClock ] =        aValue;            }
  void operator= ( const VhdlSignal<T>& aOther ) { mValue[ 1 - mClock ] = aOther.mValue[ mClock ]; }

  // friend std::ostream& operator<< ( std::ostream& aStr , const VhdlSignal<T>& a ) { aStr << std::hex << std::setfill('0') << std::setw(16) << a.mValue[ 1 - mClock ]; return aStr; }

  // BINARY_OPERATOR( operator+ , + );
  // BINARY_OPERATOR( operator- , - );
  // BINARY_OPERATOR( operator/ , / );
  // BINARY_OPERATOR( operator* , * );
  // BINARY_OPERATOR( operator% , % );
  // BINARY_OPERATOR( operator& , & );
  // BINARY_OPERATOR( operator| , | );
  // BINARY_OPERATOR( operator^ , ^ );
  // BINARY_OPERATOR( operator<< , << );
  // BINARY_OPERATOR( operator>> , >> );
  // BINARY_OPERATOR( operator|| , || );
  // BINARY_OPERATOR( operator&& , && );

  // UNARY_OPERATOR( operator+ , + );
  // UNARY_OPERATOR( operator- , - );
  // UNARY_OPERATOR( operator! , ! );
  // UNARY_OPERATOR( operator~ , ~ );
};
// ==========================================

