
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>

#include "MerkleTreePathGadget.hpp"
#include "MiMC7Gadget.hpp"

#include <Keccak256.hpp>
#include <misc.hpp>
#include <mimc7_hash.hpp>

namespace CircuitBuilder {
namespace Gadgets {

    vector<BigInteger> MiMC7Gadget::roundConstants;

	MiMC7Gadget::
		MiMC7Gadget(CircuitGenerator * generator, 
					WirePtr inputLeft , 
					WirePtr inputRight ,
					string desc ) 
		: Gadget(generator , desc) , outWires(1)
	{
		outWires[0] = Encrypt(inputLeft, inputRight)->add(inputLeft)->add(inputRight);
	}

	MiMC7Gadget::
		MiMC7Gadget(CircuitGenerator * generator, 
					const Wires & inputs,
					string desc ) 
		: Gadget(generator , desc) , outWires(1)
	{
		
		if( inputs.size() == 1){
         
            outWires[0] = Encrypt(inputs[0], inputs[0])->add(inputs[0])->add(inputs[0]);
        
        }else{
            WirePtr output = inputs[0];
            for(size_t i=1; i<inputs.size() ; i++) {
            	WirePtr mimc7 = Encrypt(output, inputs[i])->add(output)->add(inputs[i]);
                output = mimc7;
            }
            outWires[0] = output ;
        }
	}


	WirePtr MiMC7Gadget::MiMC_round(WirePtr message, WirePtr key, const BigInteger & rc){
        
        WirePtr xored = message->add(key)->add(rc); // mod prime automatically
        
        WirePtr tmp = xored;
        for (int i=0; i<2; i++) {
            tmp = tmp->mul(tmp);
            xored = xored->mul(tmp);
        }
        return xored;
    }


    WirePtr MiMC7Gadget::Encrypt(WirePtr message, WirePtr ek) {

    	WirePtr result = message;
        WirePtr key = ek;
        
        result = MiMC_round(result, key, BigInteger::ZERO());

        for (int i = 1; i < numRounds; i++) {
            result = MiMC_round(result, key, roundConstants[i]);
        }

        return result->add(key);
    }


    BigInteger MiMC7Gadget::_keccak256( vector<uint8_t> & inputs ) { 
        vector<uint8_t> keccak_digest = Hashes::keccak256(inputs);
        string hex_string = byteArrayToHexString(keccak_digest );
        return BigInteger(hex_string, 16);
    }


    vector<uint8_t> MiMC7Gadget::adjustBytes( vector<uint8_t> & input, size_t length) {
        
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


    BigInteger MiMC7Gadget::_updateRoundConstant(BigInteger & rc) {
        vector<uint8_t> byteArray = rc.toByteArray();
        vector<uint8_t> padding_byte = adjustBytes(byteArray, 32);
        return _keccak256(padding_byte);
    }

    string MiMC7Gadget::byteArrayToHexString( vector<uint8_t> & bytes) {
        size_t size = bytes.size() ;
        char buff [ ( size * 2)  + 2 ] ;
        int next_buff_pos = 0 ;
        for ( size_t i = 0 ; i < size ; i++ ) {
            next_buff_pos += sprintf( &(buff[next_buff_pos]) , "%02x" , (bytes[i] & 0xFF) );
        }	
        return string(buff) ;
    }


	Wires & MiMC7Gadget::getOutputWires() {
		return outWires ;
	}


	void MiMC7Gadget::init_static_members() {
	
		vector<uint8_t> seedStrBytes ; 
		seedStrBytes.get_allocator().allocate( Hashes::MiMC7::SeedStr.size());
		for ( auto c : Hashes::MiMC7::SeedStr ){ seedStrBytes.push_back((uint8_t)c) ; }
    	BigInteger seed = _keccak256( seedStrBytes );
    	
    	roundConstants = vector<BigInteger>(numRounds);
        roundConstants[0] = seed;
        for (int i = 1; i < numRounds; i++) {
            roundConstants[i] = _updateRoundConstant(roundConstants[i-1]);
        }
        // LOGD( "roundConstants: %d : %s\n" , numRounds , roundConstants[numRounds-1].toString(16).c_str() ) ;

		return ;
	}	


}}
