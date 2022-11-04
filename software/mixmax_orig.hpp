// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

#include <stdint.h>

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
	int i;
	myuint  tempP, tempV;
    Y[0] = ( tempV = sumtotOld);
    myuint sumtot = Y[0], ovflow = 0;
	tempP = 0;            
	for (i=1; i<N; i++){
        myuint tempPO = MULWU(tempP);
        tempP = modadd(tempP,Y[i]);
        tempV = MOD_MERSENNE(tempV + tempP + tempPO);
        Y[i] = tempV;
		sumtot += tempV; if (sumtot < tempV) {ovflow++;}
	}
	myuint x( MOD_MERSENNE(MOD_MERSENNE(sumtot) + (ovflow <<3 )) );
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

