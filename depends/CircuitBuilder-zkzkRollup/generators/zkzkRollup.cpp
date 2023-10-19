
/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk@zkrypto.com>
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

#include "zkzkRollup.hpp"
#include <HashGadget.hpp>
#include <MerkleTreeGadget.hpp>
#include <MerkleTreePathGadget.hpp>


#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;
using namespace CircuitBuilder::zkzkRollup;

namespace CircuitBuilder {
    namespace zkzkRollup {


        zkzkRollup::zkzkRollup(string circuitName, int __treeHeight , Config &config)
            : CircuitBuilder::CircuitGenerator(circuitName, config) , treeHeight(__treeHeight) 
        {}


        void zkzkRollup::buildCircuit() {
            /* statements */
            leafNodes = createInputWireArray(2048,"leafNodes");
            targetNode = createInputWire("targetNode");
            rt = createInputWire("rt");

            /* witnesses */
            directionSelector = createProverWitnessWire("direction");
            intermediateHashWires = createProverWitnessWireArray(treeHeight, "intermediateHashes");

            // make target Node
            MerkleTreeGadget *treeGadget = allocate<MerkleTreeGadget>(this, *leafNodes);
            addEqualityAssertion(treeGadget->getRootNode(), targetNode, "invalid targetNode");

            // target node membership test
            Wires targetNodes = { targetNode };
            MerkleTreePathGadget *merkleTreeGadget
                = allocate<MerkleTreePathGadget>(this, directionSelector, targetNodes, *intermediateHashWires, treeHeight);
            addEqualityAssertion(merkleTreeGadget->getOutputWires()[0], rt, "membership failed invalid rt");

            return;
        }

        void zkzkRollup::assignInputs(CircuitEvaluator &circuitEvaluator) {
            assign_inputs(circuitEvaluator);
            return;
        }

    }
}


namespace libsnark {

    CircuitGenerator * create_crv_zkzkrollup_generator(const CircuitArguments &circuit_arguments , const Config & __config ) {

        string hashType = "SHA256" ;
        const string treeHeightKey = "treeHeight";
        int treeHeight = 0 ;
        const string hashTypeKey = "hashType";


        if(circuit_arguments.find(treeHeightKey) != circuit_arguments.end() ){
            treeHeight = atoi ( circuit_arguments.at(treeHeightKey).c_str());
        }

        if(circuit_arguments.find(hashTypeKey) != circuit_arguments.end() ){
            hashType = circuit_arguments.at(hashTypeKey) ;
        }

        if (treeHeight != 21) {
            LOGD("\n\n" );
            LOGD("CreateGenerator     :\n" );
            LOGD("Invalid tree height : %d\n", treeHeight);
            LOGD("Only support 21 \n");
            return NULL ;
        }

        if (hashType.compare("MiMC7") != 0 && hashType.compare("SHA256") != 0 && hashType.compare("Poseidon") != 0) {
            LOGD("\n\n" );
            LOGD("CreateGenerator   :\n" );
            LOGD("Invalid hash type : %s\n", hashType.c_str());
            LOGD("Only support MiMC7, SHA256, Poseidon \n");
            return NULL ;
        }

        Config config = __config ;

        config.hashType = hashType;

        auto generator = new CircuitBuilder::zkzkRollup::zkzkRollup("zkzkRollup", treeHeight , config);
        generator->generateCircuit();

        return generator;
    }

}