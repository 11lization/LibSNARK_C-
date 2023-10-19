
/*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
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

#include "Register.hpp"
#include <SubsetSumHashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <MiMC7Gadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>

#include <logging.hpp>

namespace CircuitBuilder {
namespace CRV {
    
    Register::Register(string circuitName, Config &config) 
        : CircuitBuilder::CircuitGenerator(circuitName , config)
    {}     

    void Register::buildCircuit() { 

        HashOut = createInputWire("hashin");
        SK_id = createProverWitnessWire("sk_id"); // voter private key

        MiMC7 = allocate<MiMC7Gadget>( this , Wires(SK_id)) ;
        WirePtr PK_id = MiMC7->getOutputWires()[0];

        addEqualityAssertion(PK_id, HashOut);

        return ;
    }
 
    void Register::assignInputs(CircuitEvaluator &circuitEvaluator){
        assign_inputs(circuitEvaluator);
        return ;
    }

    }
}

namespace libsnark {
    
    CircuitGenerator* create_crv_registor_generator( const CircuitArguments & circuit_arguments , const Config & __config ) {

        UNUSEDPARAM(circuit_arguments) ;

        Config config = __config ;

        config.evaluationQueue_size = 650 ;
        config.inWires_size = 4 ;
        config.outWires_size  = 4 ;
        config.proverWitnessWires_size = 4 ;

        auto generator = new CircuitBuilder::CRV::Register( "Register", config ); 
        generator->generateCircuit();

        return generator ;
    }

}