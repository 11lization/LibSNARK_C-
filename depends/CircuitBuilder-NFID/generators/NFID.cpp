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


#include "NFID.hpp"
#include <HashGadget.hpp>
#include <MerkleTreePathGadget.hpp>

#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace CircuitBuilder {
namespace NFID {
    

    NFID::NFID(string circuitName, int __treeHeight , Config &config) 
        : CircuitBuilder::CircuitGenerator(circuitName , config) , treeHeight(__treeHeight)
    {}


    void NFID::buildCircuit() { 

        /* statements */
        op = createInputWire("op");
        ref_val = createInputWire("ref_val");
        rt = createInputWire("rt");
        
        // cm = createProverWitnessWire("cm");   //c
        issuer_pk = createInputWire("issuer_pk");
        issuer_addr = createProverWitnessWire("issuer_addr");
        holder_addr = createProverWitnessWire("holder_addr");
        attr_key = createProverWitnessWire("attr_key");
        attr_type = createProverWitnessWire("attr_type");
        attr_value = createProverWitnessWire("attr_value");
        r = createProverWitnessWire("r");

        // Preimage of cm such as age, r, addr, ...

        /* witnesses */
        directionSelector = createProverWitnessWire("direction");                               // Path(cm)
        intermediateHashWires = createProverWitnessWireArray(treeHeight, "intermediateHashes"); //

        // commitmennt check
        vector<WirePtr> nextInputWires;
        HashGadget* hashGadget;

        hashGadget = allocate<HashGadget>(this, issuer_pk);
        addOneAssertion((hashGadget->getOutputWires()[0])->isEqualTo(issuer_addr), "issuer pk-addr hash error");

        nextInputWires = {holder_addr, attr_key, attr_type, attr_value, r};
        hashGadget = allocate<HashGadget>(this, nextInputWires);
        WirePtr h = hashGadget->getOutputWires()[0];
        
        nextInputWires = {issuer_addr, h};
        hashGadget = allocate<HashGadget>(this, nextInputWires);
        WirePtr c = hashGadget->getOutputWires()[0];


        // membership check
        Wires leafWires = { c };
        MerkleTreePathGadget *merkleTreeGadget = allocate<MerkleTreePathGadget>(this, directionSelector, leafWires, *intermediateHashWires, treeHeight);
        addOneAssertion((rt->isEqualTo(merkleTreeGadget->getOutputWires()[0])->checkNonZero()),
                "membership failed");
        
        // TODO
        /*
        age > 20
        op : compare
        1 : ==  addOneAssertion(op->isEqualTo(1)->mul(attr_value->isEqualTo(ref_val)), "inappropriate attribute");
        2 : >   addOneAssertion(op->isEqualTo(2)->mul(attr_value->isGreaterThan(ref_val, config.LOG2_FIELD_PRIME - 1)), "inappropriate attribute");
        3 : >=  addOneAssertion(op->isEqualTo(3)->mul(attr_value->isGreaterThanOrEqual(ref_val, config.LOG2_FIELD_PRIME - 1)), "inappropriate attribute");
        4 : <   addOneAssertion(op->isEqualTo(4)->mul(attr_value->isLessThan(ref_val, config.LOG2_FIELD_PRIME - 1)), "inappropriate attribute");
        5 : <=  addOneAssertion(op->isEqualTo(5)->mul(attr_value->isLessThanOrEqual(ref_val, config.LOG2_FIELD_PRIME - 1)), "inappropriate attribute");
        */

        addOneAssertion((op->isEqualTo(3))->mul((attr_value->isGreaterThanOrEqual(ref_val, config.LOG2_FIELD_PRIME - 1))), "inappropriate attribute");
        

        return ;
    }

    void NFID::finalize(){
        // if(cin){ delete cin ; }
        // if(cout){ delete cout ; }
        // if(CT) { delete CT ; }
        if(intermediateHashWires) { delete intermediateHashWires ; }
        CircuitGenerator::finalize();
    }
 
    void NFID::assignInputs(CircuitEvaluator &circuitEvaluator){
        assign_inputs(circuitEvaluator);
        return ;
    }

}}



namespace libsnark {

    CircuitGenerator* create_crv_nfid_generator(const CircuitArguments & circuit_arguments , const Config & __config ) {
        
        string hashType = "SHA256" ;
        const string treeHeightKey = "treeHeight";
        int treeHeight = 0;
        const string hashTypeKey = "hashType";


        if(circuit_arguments.find(treeHeightKey) != circuit_arguments.end() ){
            treeHeight = atoi ( circuit_arguments.at(treeHeightKey).c_str());
        }

        if(circuit_arguments.find(hashTypeKey) != circuit_arguments.end() ){
            hashType = circuit_arguments.at(hashTypeKey) ;
        }

        if (treeHeight != 8 && treeHeight != 16 && treeHeight != 32 && treeHeight != 64 ) {
            LOGD("\n\n" );
            LOGD("CreateGenerator     :\n" );
            LOGD("Invalid tree height : %d\n", treeHeight);
            LOGD("Only support 8, 16, 32, 64 \n");
            return NULL ;
        }
        
        if (hashType.compare("MiMC7") != 0 && hashType.compare("SHA256") != 0 && hashType.compare("Poseidon") != 0) {
            LOGD("\n\n" );
            LOGD("CreateGenerator   :\n" );
            LOGD("Invalid hash type : %s\n", hashType.c_str());
            LOGD("Only support MiMC7, SHA256, Poseidon \n");
            return NULL ;
        }
        


        Config config  = __config ;
        
        config.evaluationQueue_size = 53560;
        config.inWires_size = 5 ;
        config.outWires_size  = 0 ;
        config.proverWitnessWires_size = 2080 ;
        config.hashType = hashType ;

        auto generator = new CircuitBuilder::NFID::NFID( "NFID", treeHeight , config);
        generator->generateCircuit();

        return generator ;
    }
    
}