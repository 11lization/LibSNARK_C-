
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>

#include "DecryptionGadget.hpp"
#include "hash/HashGadget.hpp"

namespace CircuitBuilder {
namespace Gadgets {

	void DecryptionGadget::buildCircuit() {
        vector<WirePtr> inputWires = {key, cipher[0]};

        HashGadget *hashGadget = allocate<HashGadget>(generator, inputWires);
        plain = cipher[1]->sub(hashGadget->getOutputWires()[0]);
        outWires[0] = plain;
	}

	
	DecryptionGadget::
		DecryptionGadget(CircuitGenerator * generator, 
							Wires &_cipher,
                            WirePtr _key, 
							string desc ) 
		: Gadget(generator , desc) , outWires(1)
	{
        cipher = _cipher;
        key = _key;
		buildCircuit();
	}

	Wires & DecryptionGadget::getOutputWires() {
		return outWires;	
	}

}}