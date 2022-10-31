#include <stdint.h>
#include <deque>

// ==========================================
class base_signal
{
public:
  static std::deque< base_signal* > mAll;
  static uint8_t mClock;

public:

  static void clock()
  {
    mClock = 1 - mClock;
    for( auto& i : mAll )  i->update();
  }

  virtual void update() = 0;
};

uint8_t base_signal::mClock( 0 );
std::deque< base_signal* > base_signal::mAll;
// ==========================================


// ==========================================
// #define UNARY_OPERATOR( arg1 , arg2 ) \
//   friend inline T arg1 ( const signal<T>& a ) { return arg2 *a ; } 

// #define BINARY_OPERATOR( arg1 , arg2 ) \
//   friend inline T arg1 ( const signal<T>& a , const signal<T>& b ) { return *a arg2 *b ; } \
//   template <typename U> friend inline T arg1 ( const signal<T>& a , const        U & b ) { return *a arg2  b ; } \
//   template <typename U> friend inline T arg1 ( const        U & a , const signal<T>& b ) { return  a arg2 *b ; }

template <typename T>
class signal : public base_signal
{
private:
  T mValue[ 2 ];

public:

  signal( const T& aValue = T() ) : base_signal()
  {
    for( auto i(0) ; i!=2 ; ++i ) mValue[ i ] = aValue;
    mAll.push_back( this );
  }

  void update(){ mValue[ 1 - mClock ] = mValue[ mClock ]; }

  const T& operator* () const { return mValue[ mClock ]; }

  void operator= ( const        T&  aValue ) { mValue[ 1 - mClock ] =        aValue;            }
  void operator= ( const signal<T>& aOther ) { mValue[ 1 - mClock ] = aOther.mValue[ mClock ]; }

  // friend std::ostream& operator<< ( std::ostream& aStr , const signal<T>& a ) { aStr << std::hex << std::setfill('0') << std::setw(16) << a.mValue[ 1 - mClock ]; return aStr; }

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

