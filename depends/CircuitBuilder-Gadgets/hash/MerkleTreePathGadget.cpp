
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>

#include "MerkleTreePathGadget.hpp"
#include "HashGadget.hpp"


namespace CircuitBuilder {
namespace Gadgets {


	MerkleTreePathGadget::
		MerkleTreePathGadget(CircuitGenerator * generator, 
								   WirePtr __directionSelectorWire, 
								   Wires & __leafWires, 
								   Wires & __intermediateHasheWires,
								   int __treeHeight, 
								   bool __invertDirectionSelectorWireBits , 
								   string desc)
		: Gadget(generator , desc),
		  treeHeight(__treeHeight),
		  directionSelectorWire(__directionSelectorWire), 
		  invertDirectionSelectorWireBits (__invertDirectionSelectorWireBits ) ,
		  leafWires(__leafWires),
		  intermediateHashWires(__intermediateHasheWires)
	{
		buildCircuit();
	}


	void MerkleTreePathGadget::buildCircuit() {

		if ( invertDirectionSelectorWireBits )  { directionSelectorBits = directionSelectorWire->invBits(treeHeight)->getBitWires(treeHeight)->asArray(); } 
		else 									{ directionSelectorBits = directionSelectorWire->getBitWires(treeHeight)->asArray(); }
		

		// Apply CRH to leaf data
		HashGadget* hashGadget;
		WirePtr currentHash = leafWires[0];


		// Apply CRH across tree path guided by the direction bits
		WirePtr temp;
		WirePtr temp2;
		Wires inHash = (2);
		
		for (int i = 0; i < treeHeight; i++) {
            temp = currentHash->sub(intermediateHashWires[i]);
            temp2 = directionSelectorBits[i]->mul(temp);
            inHash[0] = intermediateHashWires[i]->add(temp2);
            temp = currentHash->add(intermediateHashWires[i]);
            inHash[1] = temp->sub(inHash[0]);

			// in0 = (c-inter)*d + inter
			// in1 = (c+inter) - in0			
			// d:0 > inter / c
			// d:1 > c / inter            
            hashGadget = allocate<HashGadget>( generator, inHash);
			currentHash = hashGadget->getOutputWires()[0];
		}
		outRoot = Wires(currentHash);

	}

	Wires & MerkleTreePathGadget::getOutputWires() {
		return outRoot;	
	}	
 
}}


