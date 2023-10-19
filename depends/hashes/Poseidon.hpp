

#pragma once

#include <string>
#include <vector>
#include <BigInteger.hpp>




namespace Hashes {

	class Poseidon {
    
    private :
        
        static const size_t numRounds ;
        static vector<BigInteger> roundConstants;
        static BigInteger FIELD_PRIME ;

        static int NUM_ROUNDS_F ;
        static vector<uint8_t> NUM_ROUNDS_P ;
        static vector<vector<vector<BigInteger>>> M ;
        static vector<vector<vector<BigInteger>>> P ;
        static vector<vector<BigInteger>> C ;
        static vector<vector<BigInteger>> S ;
        static bool constants_loaded ;

        

	public :
        
		static BigInteger hash(const vector<BigInteger> & inputs , BigInteger __FIELD_PRIME ) ;

	private :

        static BigInteger _poseidon(const vector<BigInteger> & inputs) ;
        static BigInteger exp5(BigInteger & a) ;
        static void exp5state(vector<BigInteger> & state) ;
        static void ark(vector<BigInteger> & state, vector<BigInteger> & c, int it) ;
        static void mix(vector<BigInteger> & state, vector<vector<BigInteger>> & m) ;
        static void load_opt_constants();

    };


}