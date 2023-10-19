
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
namespace RealEstate {

	class BondDecryptionGadget : public CircuitBuilder::Gadget {

	private:

        Wires outWires;

        Wires plain;
        WirePtr key;

		/*********** Ciphertext ***********/
        Wires CT_userKey, CT_bankKey, CT_bond, CT_bond_balance;
		
		void buildCircuit() ;

	public: 
		
		BondDecryptionGadget(CircuitGenerator * generator, Wires& cipher, WirePtr key, string desc = "")  ;
		Wires & getOutputWires() ;
	
	};

}}