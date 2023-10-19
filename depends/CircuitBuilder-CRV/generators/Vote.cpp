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


#include "Vote.hpp"
#include <SubsetSumHashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <MiMC7Gadget.hpp>
#include <MerkleTreePathGadget_MiMC7.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>


#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace CircuitBuilder {
namespace CRV {
    
    Vote::Vote(string circuitName, 
               Config &config , 
               int __treeHeight) 
        : CircuitBuilder::CircuitGenerator(circuitName , config)
    {
        treeHeight = __treeHeight;
    }


    void Vote::buildCircuit() { 

        Gx = createInputWire("Gx"); Gy = createInputWire("Gy");
        Ux = createInputWire("Ux"); Uy = createInputWire("Uy");
        Vx_in = createInputWire("Vx_in");   Vy_in = createInputWire("Vy_in");
        Wx_in = createInputWire("Wx_in");   Wy_in = createInputWire("Wy_in");
        E_id = createInputWire("E_id");

        sn_in = createInputWire("sn_in");
        root_in = createInputWire("root_in");
        
        sk_id = createProverWitnessWire("sk_id");
        Sx = createProverWitnessWire("Sx"); Sy = createProverWitnessWire("Sy");
        Tx = createProverWitnessWire("Tx"); Ty = createProverWitnessWire("Ty");
        randomizedEnc = createProverWitnessWire("randomizedEnc");
        msg = createProverWitnessWire("msg");


        directionSelector = createProverWitnessWire("Direction selector");
        intermediateHashWires = createProverWitnessWireArray(treeHeight, "Intermediate Hashes");

        vector<WirePtr> sn_hash_wires = {Sx, Tx, sk_id, E_id} ;
        MiMC7Gadget *sn_hash = allocate<MiMC7Gadget>(this , sn_hash_wires);
        WirePtr sn_out = sn_hash->getOutputWires()[0];

        ECGroupOperationGadget *encV = new ECGroupOperationGadget(this , Gx, Gy, randomizedEnc, Sx, Sy, msg); //하나에 120ms 정도
        ECGroupOperationGadget *encW = new ECGroupOperationGadget(this , Ux, Uy, randomizedEnc, Tx, Ty, msg);
        add_allocation(encV);
        add_allocation(encW);

        MiMC7Gadget * pk_hash = allocate<MiMC7Gadget>(this , Wires(sk_id)) ;
        WirePtr pk_out = pk_hash->getOutputWires()[0];
        
        Wires V_out = encV->getOutputWires();
        Wires W_out = encW->getOutputWires();
        Wires ekpk ( {Sx, Tx, pk_out} ) ;
        MerkleTreePathGadget_MiMC7 *merkleTreeGadget = allocate<MerkleTreePathGadget_MiMC7>(this, directionSelector, ekpk, *intermediateHashWires, treeHeight , false );
        root_out = merkleTreeGadget->getOutputWires()[0];
        
        //addEqualityAssertion(pk_out, pk_in);
        addEqualityAssertion(sn_out, sn_in);
        addEqualityAssertion(V_out[0], Vx_in);
        addEqualityAssertion(V_out[1], Vy_in);
        addEqualityAssertion(W_out[0], Wx_in);
        addEqualityAssertion(W_out[1], Wy_in);
        addEqualityAssertion(root_out, root_in);

        return ;
    }

    void Vote::finalize(){
		delete intermediateHashWires ;
		CircuitGenerator::finalize();
	}

    void Vote::assignInputs(CircuitEvaluator &circuitEvaluator){
        assign_inputs(circuitEvaluator);
        return ;
    }

}}


namespace libsnark {

    CircuitGenerator* create_crv_vote_generator( const CircuitArguments & circuit_arguments , const Config & __config ) {
        UNUSEDPARAM(circuit_arguments) ;

        Config config  = __config ;
        
        config.evaluationQueue_size = 53560;
        config.inWires_size = 40 ;
        config.outWires_size  = 4 ;
        config.proverWitnessWires_size = 2080 ;

        CircuitBuilder::CRV::Vote * generator = new CircuitBuilder::CRV::Vote( "Vote", config , 16);
        generator->generateCircuit();

        return generator ;
    }

}