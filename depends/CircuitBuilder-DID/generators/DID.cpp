/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk2889@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/

#include <limits> 

#include <global.hpp>
#include <BigInteger.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <WireArray.hpp>
#include <BitWire.hpp>
#include <ConstantWire.hpp>
#include <LinearCombinationBitWire.hpp>
#include <LinearCombinationWire.hpp>
#include <VariableBitWire.hpp>
#include <VariableWire.hpp>
#include <WireLabelInstruction.hpp>

#include <BasicOp.hpp>
#include <MulBasicOp.hpp>
#include <AssertBasicOp.hpp>

#include <Exceptions.hpp>
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>

#include "DID.hpp"
#include <SubsetSumHashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <MiMC7Gadget.hpp>
#include <MerkleTreePathGadget_MiMC7.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>

#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace CircuitBuilder {
namespace DID {

    DID::DID(string circuitName, Config &config)
        : CircuitBuilder::CircuitGenerator(circuitName , config)
    {}

    void DID::buildCircuit() { 

		// /********************* INPUT ***************************/
		// WirePtr iPk, bHash, sName, sYear, sUniv, sDegree, sMajor, sSN;

		// /********************* Witness ***************************/
		// WirePtr wName, wYear, wUniv, wDegree, wMajor, wSN;

        iPk = createInputWire("iPk");
        bHash = createInputWire("bHash");
        sName = createInputWire("sName");
        sYear = createInputWire("sYear");
        sUniv = createInputWire("sUniv");
        sDegree = createInputWire("sDegree");
        sMajor = createInputWire("sMajor");
        sSN = createInputWire("sSN");

        wName = createProverWitnessWire("wName");
        wYear = createProverWitnessWire("wYear");
        wUniv = createProverWitnessWire("wUniv");
        wDegree = createProverWitnessWire("wDegree");
        wMajor = createProverWitnessWire("wMajor");
        wSN = createProverWitnessWire("wSN");

        vector<WirePtr> hash_wires = {wName->add(sName), sYear->add(wYear), sUniv->add(wUniv), sDegree->add(wDegree), sMajor->add(wMajor), sSN->add(wSN)} ;
        MiMC7Gadget *hash = allocate<MiMC7Gadget>(this , hash_wires);
        WirePtr hash_out = hash->getOutputWires()[0];

        vector<WirePtr> block_hash_wires = {iPk, hash_out};
        MiMC7Gadget *block_hash = allocate<MiMC7Gadget>(this, block_hash_wires);
        WirePtr block_hash_out = block_hash->getOutputWires()[0];

        addEqualityAssertion(block_hash_out, bHash);

        return ;
    }

    void DID::finalize(){
		CircuitGenerator::finalize();
	}

    void DID::assignInputs(CircuitEvaluator &circuitEvaluator){
        assign_inputs(circuitEvaluator);
        return ;
    }
}}

namespace libsnark {

    CircuitGenerator* create_crv_did_generator( const CircuitArguments & circuit_arguments , const Config & __config ) {
        UNUSEDPARAM(circuit_arguments) ;

        Config config  = __config ;

        config.evaluationQueue_size = 53560;
        config.inWires_size = 20 ;
        config.outWires_size  = 0 ;
        config.proverWitnessWires_size = 2080 ;

        CircuitBuilder::DID::DID * generator = new CircuitBuilder::DID::DID( "DID", config );
        generator->generateCircuit();

        return generator ;
    }

}