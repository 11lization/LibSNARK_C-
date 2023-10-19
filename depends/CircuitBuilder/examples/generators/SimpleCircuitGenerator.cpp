
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

#include <examples/generators/SimpleCircuitGenerator.hpp>

#include <logging.hpp>

namespace CircuitBuilder {


	SimpleCircuitGenerator::SimpleCircuitGenerator(string circuitName , Config config ) 
		: CircuitGenerator(circuitName , config)
	{}


	void SimpleCircuitGenerator::buildCircuit() {

		// declare input array of length 4.
		inputs = createInputWireArray(4);

		// r1 = in0 * in1
		WirePtr r1 = inputs[0]->mul(inputs[1]);

		// r2 = in2 + in3
		WirePtr r2 = inputs[2]->add(inputs[3]);

		// result = (r1+5)*(6*r2)
		WirePtr result = r1->add(5)->mul(r2->mul(6));

		// mark the wire as output
		makeOutput(result);

	}

	void SimpleCircuitGenerator::generateSampleInput(CircuitEvaluator & circuitEvaluator) {

		for (int i = 0; i < 4; i++) {
			LOGD ( "Input Wire assignment : %s <== %d \n" , inputs[i]->toString().c_str() , i+1  )
			circuitEvaluator.setWireValue(inputs[i] , i + 1);
		}
	}

}


