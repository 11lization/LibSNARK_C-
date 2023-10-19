
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


/**
 * A Merkle tree authentication gadget using the subsetsum hash function
 * 
 */

namespace CircuitBuilder {
namespace Gadgets {

	class MerkleTreePathGadget_MiMC7 : public CircuitBuilder::Gadget {

	private:

	  	int treeHeight;
		WirePtr directionSelectorWire;
		Wires directionSelectorBits;
		bool invertDirectionSelectorWireBits ;
		Wires leafWires;
		Wires intermediateHashWires;
		Wires outRoot;

		void buildCircuit() ;

	public: 
		
		MerkleTreePathGadget_MiMC7(CircuitGenerator * generator, 
								   WirePtr directionSelectorWire, 
								   Wires & leafWires, 
								   Wires & intermediateHasheWires,
								   int treeHeight, 
								   bool invertDirectionSelectorWireBits = false ,
								   string desc = "")  ;

		~MerkleTreePathGadget_MiMC7(){}
		Wires & getOutputWires() ;

		private: static bool static_members_initialized  ;
		public:  static void init_static_members(Config &config) ;
	
	};

}}