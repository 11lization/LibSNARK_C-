

#pragma once

#include <string>
#include <vector>
#include <BigInteger.hpp>




namespace Hashes {

	class MerkleTreePath {
    
    public :

        static BigInteger makeRoot( BigInteger directionSelector, 
                                    BigInteger leafWires, 
                                    const vector<BigInteger> & intermediateHasheWires, 
                                    int treeHeight ,
                                    const BigInteger & FIELD_PRIME );

    };
}