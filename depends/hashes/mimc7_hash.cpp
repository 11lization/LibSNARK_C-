


#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

#include <Keccak256.hpp>
#include <misc.hpp>
#include <mimc7_hash.hpp>


using namespace std;


namespace Hashes {

    const std::string MiMC7::SeedStr = "mimc7_seed" ;
    
    const size_t MiMC7::numRounds = 91 ;

    vector<BigInteger> MiMC7::roundConstants ;



    BigInteger MiMC7::hash( const BigInteger & input , const BigInteger & FIELD_PRIME ){
        
        make_roundConstants() ;
    
        return MiMC_( input , input , FIELD_PRIME ) ;
         
    }


    BigInteger MiMC7::hash( const BigInteger & input1 , const BigInteger & input2 , const BigInteger & FIELD_PRIME ){
        
        make_roundConstants() ;

        return MiMC_( input1 , input2 , FIELD_PRIME ) ;
         
    }


    BigInteger MiMC7::hash( const vector<BigInteger> & inputs , const BigInteger & FIELD_PRIME ){
        
        make_roundConstants() ;

        BigInteger output = inputs[0];
            
        for(size_t i = 1; i < inputs.size() ; i++) {
            BigInteger mimc7 = MiMC_(output, inputs[i], FIELD_PRIME );
            output = mimc7;
        }
        
        return output ;
    }


    void MiMC7::make_roundConstants() {

        if ( ! roundConstants.size() ) {

            vector<uint8_t> seedStrBytes ; 
            seedStrBytes.get_allocator().allocate(Hashes::MiMC7::SeedStr.size());
            
            for ( auto c : SeedStr ){ seedStrBytes.push_back((uint8_t)c) ; }
            
            BigInteger seed = _keccak256( seedStrBytes );

            roundConstants.resize(numRounds);
            roundConstants[0] = seed;
            for (size_t i = 1; i < numRounds; i++) {
                roundConstants[i] = _updateRoundConstant(roundConstants[i-1]);
            }
        
        }

    }


    BigInteger MiMC7::_keccak256( vector<uint8_t> & inputs ) { 
        vector<uint8_t> keccak_digest = keccak256(inputs);
        string hex_string = MISC::byteArrayToHexString( keccak_digest );
        return BigInteger(hex_string, 16);
    }


    BigInteger MiMC7::_updateRoundConstant(BigInteger & rc) {
        vector<uint8_t> byteArray = rc.toByteArray();
        vector<uint8_t> padding_byte = adjustBytes(byteArray, 32);
        return _keccak256(padding_byte);
    }


    vector<uint8_t> MiMC7::adjustBytes( vector<uint8_t> & input, size_t length) {
    
        if (input.size() >= length) { // restrict byte length
            vector<uint8_t> restrictedByte(length);
            MISC::array_copy(input, input.size() - length, restrictedByte, 0, length);
            return restrictedByte;
        }
        
        // zero padding
        vector<uint8_t> res (32);
        vector<uint8_t> pad (32 - input.size() , (uint8_t) 0 );

        MISC::array_copy(pad, 0, res, 0, pad.size());
        MISC::array_copy(input, 0, res, pad.size() , input.size());

        return res;
    }

    
    BigInteger MiMC7::MiMC_( const BigInteger & inputLeft, const BigInteger & inputRight , const BigInteger & FIELD_PRIME ) {
        return Encrypt(inputLeft, inputRight , FIELD_PRIME ).add(inputLeft).add(inputRight).mod(FIELD_PRIME);
    }


    BigInteger MiMC7::Encrypt( const BigInteger & message , const BigInteger & key , const BigInteger & FIELD_PRIME ) {
        
        BigInteger result = message;

        result = MiMC_round(result, key, BigInteger::ZERO() , FIELD_PRIME );
        for (size_t i = 1; i < numRounds; i++) {
            result = MiMC_round(result, key, roundConstants[i] , FIELD_PRIME );
        }

        return result.add(key).mod(FIELD_PRIME);
    }


    BigInteger MiMC7::MiMC_round( const BigInteger &message, const BigInteger &key, const BigInteger &rc , const BigInteger &FIELD_PRIME ) {
        
        BigInteger xored = message.add(key).add(rc).mod(FIELD_PRIME); // mod prime automatically
        BigInteger tmp = xored;

        for (size_t i=0; i<2; i++) {
            tmp = tmp.multiply(tmp).mod(FIELD_PRIME);
            xored = xored.multiply(tmp).mod(FIELD_PRIME);
        }
        
        return xored;
    }

}




#ifdef __cplusplus
extern "C" {
#endif

    void mimc7_hash( mpz_ptr dst , mpz_srcptr input , mpz_srcptr fieldPrime ){

        BigInteger _fieldPrime = BigInteger(fieldPrime);
        BigInteger _input = BigInteger(input) ;
        
        BigInteger _dst = Hashes::MiMC7::hash( _input , _fieldPrime ) ;

        BigInteger::set_mpz( dst , _dst ) ;

    }
    

    void mimc7_hash_from_array_inputs( mpz_ptr dst , const MP_INT*  inputs , int inputs_count , mpz_srcptr fieldPrime ){
    
        BigInteger _fieldPrime = BigInteger(fieldPrime) ;

        vector<BigInteger> _inputs( inputs_count , BigInteger::ZERO() ) ;
        
        for ( int i = 0 ; i < inputs_count ; i ++ ){
            _inputs[i] = BigInteger( &inputs [i] ) ;
        }

        BigInteger _dst = Hashes::MiMC7::hash( _inputs , _fieldPrime  ) ;
        
        BigInteger::set_mpz( dst , _dst ) ;

    }


#ifdef __cplusplus
}
#endif   