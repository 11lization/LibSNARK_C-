
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>
#include <HashGadget.hpp>
#include <DecryptionGadget.hpp>

#include "BondDecryptionGadget.hpp"

namespace CircuitBuilder {
namespace RealEstate {

	void BondDecryptionGadget::buildCircuit() {
        // step1. CT_user_k (PKE)
		// 1-1. k <= c1 / c0^sk
		// k = ECGroupOp(c1, -c0^sk)
		// msg = [ c3 - H(k) ].

        // step2. CT_bank_k

        // step3. CT_p_bond (SE)
		// DecryptionGadget()

        // step4. CT_pp_bond
	}

	
	BondDecryptionGadget::
		BondDecryptionGadget(CircuitGenerator * generator, 
							Wires &_cipher,
                            WirePtr _key, 
							string desc ) 
		: Gadget(generator , desc) , outWires(1)
	{
        cipher = _cipher;
        key = _key;
		buildCircuit();
	}

	Wires & BondDecryptionGadget::getOutputWires() {
		return outWires;	
	}

}}