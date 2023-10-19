
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>

#include "HashGadget.hpp"
#include "SHA256Gadget.hpp"
#include "MiMC7Gadget.hpp"
#include "PoseidonGadget.hpp"

namespace CircuitBuilder {
namespace Gadgets {

    HashGadget::
		HashGadget(CircuitGenerator * generator, 
                            const Wires& ins, 
							string desc ) 
		: Gadget(generator , desc)
	{

        if (generator->config.hashType.compare("SHA256") == 0) {
            SHA256Gadget* sha256 = allocate<SHA256Gadget>(generator, ins, 256, 32*ins.size(), false, true);
            Wires hashOutput = sha256->getOutputWires();
            
            outWires = allocate<WireArray>(generator, hashOutput)->reverse()->packWordsIntoLargerWords(32, 8);
        }
        else if (generator->config.hashType.compare("MiMC7") == 0) {
            MiMC7Gadget* mimc7 = allocate<MiMC7Gadget>(generator, ins);
		    outWires = mimc7->getOutputWires();
        }
        else if (generator->config.hashType.compare("Poseidon") == 0) {
            PoseidonGadget* poseidon = allocate<PoseidonGadget>(generator, ins);
		    outWires = poseidon->getOutputWires();
        }
	}

	Wires & HashGadget::getOutputWires() {
		return outWires;	
	}

}}