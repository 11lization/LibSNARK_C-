
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
namespace Gadgets {

	class DecryptionGadget : public CircuitBuilder::Gadget {

	private:

        Wires outWires;

        WirePtr plain;
        WirePtr key;
        Wires cipher;
		
		void buildCircuit() ;

	public: 
		
		DecryptionGadget(CircuitGenerator * generator, Wires& cipher, WirePtr key, string desc = "")  ;
		Wires & getOutputWires() ;
	
	};

}}