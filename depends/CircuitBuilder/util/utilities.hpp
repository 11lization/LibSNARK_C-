 


#pragma once
 
#include <global.hpp>
#include <libff/algebra/fields/bigint.hpp>


namespace CircuitBuilder {

	template<typename FieldT> FieldT convert2FieldT( const BigInteger &BI ) {
		libff::bigint<FieldT::num_limbs> bi ( BI.get_mpz()) ;
		FieldT fp (bi);
		return fp ;
	}


	class Util {

	public:

		static Wires concat(const Wires &a1, const Wires &a2) ;

		static Wires concat(WirePtr w, WirePtr a) ;

		static Wires reverseBytes(Wires inBitWires) ;

		static Wires split(CircuitGenerator * generator , WirePtr x, int totalbitlength, int numchunks, int chunksize) ;

		static Wires split(CircuitGenerator * generator , Wire & x, int totalbitlength, int numchunks, int chunksize) ;

		static vector<BigInteger> split(BigInteger x, int numchunks, int chunksize);
		
		static vector<BigInteger> split(BigInteger x, int chunksize);

		static vector<BigInteger> zeropadBigIntegers(vector<BigInteger>, int totallenght);

		static BigInteger nextRandomBigInteger( BigInteger::RandState & rand, const BigInteger &n) ;

		static BigInteger nextRandomBigInteger( BigInteger::RandState & rand , int numBits) ;

	};

}









