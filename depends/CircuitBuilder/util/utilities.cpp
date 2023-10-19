

#include <openssl/evp.h> 
#include <openssl/sha.h> 
 
#include <utilities.hpp>
#include <WireArray.hpp>
#include <Wire.hpp>
#include <CircuitGenerator.hpp>
#include <mimc7_hash.hpp>
#include <BigInteger.hpp>

namespace CircuitBuilder {

	Wires Util::concat(const Wires &a1, const Wires &a2) {
		Wires all(a1.size() + a2.size()) ;  
		for (size_t i = 0; i < all.size(); i++) {
			WirePtr iw = i < a1.size() ? a1[i] : a2[(i - a1.size())] ;
			all.set(i  , iw );
		}
		return all;
	}


	Wires Util::concat(WirePtr w, WirePtr a) {
		Wires all(2) ;
		for (size_t i = 0; i < all.size(); i++) {
			WirePtr iw = i < 1 ? w : a ;
			all.set(i  , iw );
		}
		return all ;
	}

    Wires Util::reverseBytes(Wires inBitWires) {
		Wires outs = inBitWires.copyOfRange(0, inBitWires.size());
		int numBytes = inBitWires.size() / 8;
		for (int i = 0; i < numBytes / 2; i++) {
			int other = numBytes - i - 1;
			for (int j = 0; j < 8; j++) {
				WirePtr temp = outs[i * 8 + j];
				outs[i * 8 + j] = outs[other * 8 + j];
				outs[other * 8 + j] = temp;
			}
		}
		return outs;
	}

	
	Wires Util::split(CircuitGenerator * generator , WirePtr x, int totalbitlength, int numchunks, int chunksize){
		
		Wires & temp_xbit = x->getBitWires(totalbitlength)->asArray();
		int diff = numchunks * chunksize - totalbitlength;
		// xbit = new WireArray(xbit).shiftRight(numchunks * chunksize, diff).asArray();
		WireArray * w_array = allocate<WireArray>(generator , temp_xbit ) ;
		WireArray * w_array2 = w_array->shiftRight(numchunks * chunksize, diff) ;
		Wires & xbit = w_array2->asArray();
		
		// Wire[][] xbitsplit = new Wire[numchunks][chunksize];
		vector< Wires > xbitsplit (numchunks , Wires(chunksize) ) ;
		for(int i = 0 ; i < numchunks ; i++){
			for(int j = 0 ; j < chunksize ; j++){
				xbitsplit[i][j] = xbit[i * numchunks + j];
			}
		}

		Wires output(numchunks) ;//= new Wire[numchunks];
		for(int i = 0 ; i < numchunks ; i++){
			// new WireArray(xbitsplit[i]).packAsBits(chunksize);
			WireArray* w_array = allocate<WireArray>(generator , xbitsplit[i] );
			WirePtr w = w_array->packAsBits(chunksize) ;
			output.set(i , w ) ; 
		}
		return output;
	}

	
	Wires Util::split(CircuitGenerator * generator , Wire & x, int totalbitlength, int numchunks, int chunksize){
		return split(generator , & x , totalbitlength, numchunks , chunksize ) ;
	}


    vector<BigInteger> Util::split(BigInteger x, int numchunks, int chunksize) {
        vector<BigInteger> chunks(numchunks);
        BigInteger mask = BigInteger(2l).pow(chunksize).subtract(1l);
        for (int i=0; i< numchunks; i++) {
            chunks[i] = x.AND(mask);
            x = x.shiftRight(chunksize);
        }
        return chunks;
    }

    vector<BigInteger> Util::split(BigInteger x, int chunksize) {
        int numchunks = (int)ceil((x.bitLength()*1.0 / chunksize));
        
        return split(x, numchunks, chunksize);
    }

    vector<BigInteger> Util::zeropadBigIntegers(vector<BigInteger> a, int totallength_) {
        size_t totallength = (size_t)totallength_ ;
        if (a.size() >= totallength) {
            return a;
        }
        vector<BigInteger> output(a);

        for (size_t i=0; i < (totallength - a.size()) ; i++) {
            output.push_back(BigInteger::ZERO());
        }

        return output;
    }


	BigInteger Util::nextRandomBigInteger(BigInteger::RandState & rand , const BigInteger & n) {

		BigInteger result = BigInteger(n.bitLength(), rand);
		while (result.compareTo(n) >= 0) {
			result = BigInteger(n.bitLength(), rand);
		}
		return result;
	}


	BigInteger Util::nextRandomBigInteger(BigInteger::RandState & rand , int numBits) {
		return BigInteger(numBits, rand) ; 
	}


}
