



#include <cstdint>
#include <cstring>
#include <vector>
#include <string>


#include <mimc7_hash.hpp>
#include <misc.hpp>
#include <merkle_tree_path.hpp>



namespace Hashes {
 


    BigInteger MerkleTreePath::makeRoot(BigInteger directionSelector , 
                                        BigInteger leafWires , 
                                        const vector<BigInteger> & intermediateHashes , 
                                        int treeHeight,
                                        const BigInteger & FIELD_PRIME ) 
    {
        
        vector<size_t> directionSelectorBits = MISC::zeroPad( MISC::split( directionSelector ) , treeHeight ); 
        
        BigInteger currentHash = leafWires;

        for (int i = 0; i < treeHeight; i++) {
            
            if( directionSelectorBits[i] == 0) {
            // if(directionSelectorBits[i].toByteArray()[0] == 0) {
                currentHash = MiMC7::hash( intermediateHashes[i], currentHash , FIELD_PRIME );
            }else{
                currentHash = MiMC7::hash( currentHash, intermediateHashes[i] , FIELD_PRIME );
            }
        }
        
        return currentHash ;
    }

}



#ifdef __cplusplus
extern "C" {
#endif

    void merkle_tree_path_make_root( mpz_ptr dst , 
                                     mpz_srcptr directionSelector , 
                                     mpz_srcptr leafWires , 
                                     const MP_INT* intermediateHashes ,
                                     int intermediateHashes_count ,
                                     int treeHeight , 
                                     mpz_srcptr fieldPrime )
    {
    
        BigInteger _directionSelector = BigInteger(directionSelector) ;
        BigInteger _leafWires = BigInteger(leafWires) ;
        BigInteger _fieldPrime = BigInteger(fieldPrime) ;

        vector<BigInteger> _intermediateHashes( intermediateHashes_count , BigInteger::ZERO() ) ;
        
        for ( int i = 0 ; i < intermediateHashes_count ; i ++ ){
            _intermediateHashes[i] = BigInteger( &intermediateHashes [i] ) ;
        }

        BigInteger _dst = Hashes::MerkleTreePath::makeRoot ( _directionSelector , _leafWires , _intermediateHashes , treeHeight,  _fieldPrime ) ;
        
        BigInteger::set_mpz( dst , _dst ) ;

    }

#ifdef __cplusplus
}
#endif  