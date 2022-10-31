#include <stdint.h>
#include <iostream>
#include <iomanip>

#define N 17
#define SPECIALMUL 36
#define SPECIAL 0

#define BITS  61
typedef uint64_t myuint;
#define M61   2305843009213693951ULL
#define MOD_MERSENNE(k) ((((k)) & M61) + (((k)) >> BITS) )  
#define MULWU(k) (( (k)<<(SPECIALMUL) & M61) | ( (k) >> (BITS-SPECIALMUL))  )
#define modadd(foo, bar) MOD_MERSENNE(foo+bar)

struct rng_state_t
{
    myuint V[N];
    myuint sumtot;
    int counter;
};


myuint iterate_raw_vec(myuint* Y, myuint sumtotOld){
	// operates with a raw vector, uses known sum of elements of Y
	int i;
	myuint  tempP, tempV;
    Y[0] = ( tempV = sumtotOld);
    myuint sumtot = Y[0], ovflow = 0; // will keep a running sum of all new elements
	tempP = 0;              // will keep a partial sum of all old elements
	for (i=1; i<N; i++){
        myuint tempPO = MULWU(tempP);
        tempP = modadd(tempP,Y[i]);
        tempV = MOD_MERSENNE(tempV + tempP + tempPO); // edge cases ?
        Y[i] = tempV;
		sumtot += tempV; if (sumtot < tempV) {ovflow++;}

        // std::cout << " > "<< std::setw(16) << sumtot << " "<< std::setw(16) << tempV << " " << ovflow << std::endl;

	}
	myuint x( MOD_MERSENNE(MOD_MERSENNE(sumtot) + (ovflow <<3 )) );

    std::cout << "-----------------------------------" << std::endl;
    for( int i(0) ; i!=9 ; ++i ) std::cout << std::setw(16) << Y[i] << ' ';
    std::cout << std::endl;
    for( int i(9) ; i!=17 ; ++i ) std::cout << std::setw(16) << Y[i] << ' ';
    std::cout << "| " << std::setw(16) << x;    
    std::cout << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    return x;
}


myuint flat(rng_state_t* X) {
    int i;
    i=X->counter;
    
    if (i<=(N-1) ){
        X->counter++;
        return X->V[i];
    }else{
        X->sumtot = iterate_raw_vec(X->V, X->sumtot);
        X->counter=2;
        return X->V[1];
    }
}


int main()
{
    std::cout << std::hex << std::setfill('0');
    
    constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

    rng_state_t lState{ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} , 1 , 2 };
    lState.sumtot = iterate_raw_vec( lState.V , lState.sumtot );

    for( int i(0) ; i!=40 ; ++i ) std::cout << std::setw(16) << flat( &lState ) << std::endl;

    return 0;
}

