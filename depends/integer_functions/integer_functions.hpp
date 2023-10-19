 


#pragma once
 
#include <BigInteger.hpp> 

namespace IntegerFunctions {

	static inline int jacobi( const BigInteger & A , const BigInteger & B){
		return mpz_jacobi(A.get_mpz() , B.get_mpz() ) ;
	}
		
	BigInteger ressol(const BigInteger &a, const BigInteger &p) ;

}









