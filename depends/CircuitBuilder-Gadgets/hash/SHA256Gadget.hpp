
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
namespace Gadgets {

	class SHA256Gadget : public Gadget {

	private:
        Wires unpaddedInputs;

	    int bitwidthPerInputElement;
	    int totalLengthInBytes;

	    int numBlocks;
	    bool binaryOutput;
	    bool paddingRequired;

	    Wires preparedInputBits;
	    Wires outWires;

        static vector<u_long> H;
        static vector<u_long> K;

        void prepare();
        WirePtr computeCh(WirePtr a, WirePtr b, WirePtr c, int numBits);
        WirePtr computeMaj(WirePtr a, WirePtr b, WirePtr c, int numBits);

	protected:
        void buildCircuit() ;

	public: 
		
		SHA256Gadget(CircuitGenerator * generator, 
					const Wires & inputs , 
					int bitWidthPerInputElement, 
					int totalLengthInBytes, 
					bool binaryOutput, 
					bool paddingRequired, 
					string desc = "") ;

		Wires & getOutputWires() ;

		static void init_static_members() ;
        
	};

}}

