

#pragma once

#include <string>
#include <vector>
#include <BigInteger.hpp>


namespace Hashes {

	class MiMC7 {
    
    private :
        
        static const size_t numRounds ;

        static vector<BigInteger> roundConstants;

        static void make_roundConstants() ;
        static BigInteger _keccak256( vector<uint8_t> & inputs ) ;
        static BigInteger _updateRoundConstant(BigInteger & rc) ;
        
        static BigInteger MiMC_( const BigInteger & inputLeft, const BigInteger & inputRight , const BigInteger & FIELD_PRIME) ;
        static BigInteger Encrypt(const BigInteger & message , const BigInteger & key , const BigInteger & FIELD_PRIME ) ;
        static BigInteger MiMC_round(const BigInteger &message, const BigInteger &key, const BigInteger &rc , const BigInteger &FIELD_PRIME) ;
        static vector<uint8_t> adjustBytes( vector<uint8_t> & input, size_t length) ;


	public :
        
        static const std::string SeedStr ;

		static BigInteger hash( const BigInteger & input , const BigInteger & FIELD_PRIME ) ;

        static BigInteger hash( const BigInteger & input1 , const BigInteger & input2 , const BigInteger & FIELD_PRIME ) ;
        
        static BigInteger hash( const vector<BigInteger> & inputs , const BigInteger & FIELD_PRIME ) ;

    };

}