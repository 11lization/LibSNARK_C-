
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
namespace Gadgets {

	class MiMC7Gadget : public CircuitBuilder::Gadget {

	private:

		Wires outWires;

		/**
	     * MiMC specialized for Fr in ALT-BN128, in which the exponent is 7 and 91
	     * rounds are used.
	     */
    	static BigInteger seed ; 
    	static const int numRounds = 91;
    	static vector<BigInteger> roundConstants;

		WirePtr MiMC_round(WirePtr message, WirePtr key, const BigInteger & rc);
		WirePtr Encrypt(WirePtr message, WirePtr ek) ;
		static BigInteger _keccak256( vector<uint8_t> & inputs ) ;
		static vector<uint8_t> adjustBytes( vector<uint8_t>& input, size_t length) ;
		static BigInteger _updateRoundConstant(BigInteger & rc) ;
		static std::string byteArrayToHexString( vector<uint8_t> & bytes) ;

	public: 
		
		MiMC7Gadget(CircuitGenerator * generator, WirePtr inputLeft , WirePtr inputRight , string desc = "")  ;
		MiMC7Gadget(CircuitGenerator * generator, const Wires & inputs , string desc = "")  ;
		Wires & getOutputWires() ;

		public:  static void init_static_members() ;
	};

}}

