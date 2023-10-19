
/*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/

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


#include "Tally.hpp"
#include <ModConstantGadget.hpp>
#include <SubsetSumHashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <MiMC7Gadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>


#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace CircuitBuilder {
namespace CRV {
    
    
    Tally::Tally(string circuitName, Config &config ) 
        : CircuitBuilder::CircuitGenerator(circuitName , config)
    {}

    Wires Tally::expwire(WirePtr input) {
        return Wires(input->getBitWires(EXPONENT_BITWIDTH)->asArray());
    }
        
    void Tally::buildCircuit() { 

        Gx = createInputWire("Gx");         Gy = createInputWire("Gy");
        Ux = createInputWire("Ux");         Uy = createInputWire("Uy");
        Vsum_x = createInputWire("Vsum_x"); Vsum_y = createInputWire("Vsum_y"); /*vsum*/
        Wsum_x = createInputWire("Wsum_x"); Wsum_y = createInputWire("Wsum_y"); /*wsum*/
        msgsum = createInputWire("msgsum");

        SK = createProverWitnessWire("sk");

        ECGroupGeneratorGadget *dec1 = new ECGroupGeneratorGadget(this , Gx, Gy ,SK ); 
        ECGroupOperationGadget *dec2 = new ECGroupOperationGadget(this, Vsum_x, Vsum_y, SK, Gx, Gy, msgsum);
        add_allocation(dec1);
        add_allocation(dec2);

        Wires check1 = dec1->getOutputWires();
        Wires check2 = dec2->getOutputWires();

        addEqualityAssertion(check1[0], Ux, "check1");
        addEqualityAssertion(check1[1], Uy, "check1");
        addEqualityAssertion(check2[0], Wsum_x, "check2");
        addEqualityAssertion(check2[1], Wsum_y, "check2");

        return ;
    }

    void Tally::assignInputs(CircuitEvaluator &circuitEvaluator){
        assign_inputs(circuitEvaluator);
        return ;
    }

}}


namespace libsnark {

    CircuitGenerator* create_crv_tally_generator( const CircuitArguments & circuit_arguments , const Config & __config) {
        UNUSEDPARAM(circuit_arguments) ;

        Config config  = __config ;
        
        config.evaluationQueue_size = 31360 ;
        config.inWires_size = 16 ;
        config.outWires_size  = 2 ;
        config.proverWitnessWires_size = 1550 ;

        auto generator = new CircuitBuilder::CRV::Tally( "Tally", config ); 
        generator->generateCircuit();

        return generator ;
    }

}