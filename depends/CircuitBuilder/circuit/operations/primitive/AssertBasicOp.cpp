

#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <AssertBasicOp.hpp> 

#include <logging.hpp>


namespace CircuitBuilder {


	AssertBasicOp::AssertBasicOp( 
		CircuitGenerator *generator , 
		WirePtr w1, WirePtr w2, 
		WirePtr out, 
		const string & desc ) 
	:   BasicOp( generator , 
					CONSTRAINT_OPCODE,
					w1, w2 , 
					out ,
					desc )
	{
		class_id = class_id | Object::AssertBasicOp_Mask ;
	}


	void AssertBasicOp::compute( CircuitEvaluator & evaluator ) {
		
		BigInteger leftSide = 	evaluator.getAssignment(inputs[0]->getWireId()).multiply(
								evaluator.getAssignment(inputs[1]->getWireId())).mod(
								generator->config.FIELD_PRIME);

		const BigInteger &rightSide = evaluator.getAssignment(outputs[0]->getWireId());
		
		bool check = leftSide.equals(rightSide);
		
		if (!check) {
			LOGD(" *** Assertion Failed %s ***\n[%s * %s != %s] \n" , 
					this->toString().c_str() ,
					evaluator.getAssignment(inputs[0]->getWireId()).toString(16).c_str() ,
					evaluator.getAssignment(inputs[1]->getWireId()).toString(16).c_str() ,
					evaluator.getAssignment(outputs[0]->getWireId()).toString(16).c_str() );
			
			if( ! generator->ignore_failed_assertion){
				throw runtime_error("Error During Evaluation");
			}
		}
	}


	void AssertBasicOp::checkOutputs(CircuitEvaluator & evaluator) {
		// do nothing
		UNUSEDPARAM(evaluator)
	}
	

	string AssertBasicOp::getOpcode() const {
		return "assert";
	}
	

	bool AssertBasicOp::equals(Object *obj) const {

		if (this == obj)
			return true;
		if (!(obj->instanceof_AssertBasicOp())) {
			return false;
		}
		
		AssertBasicOp *op = (AssertBasicOp*) obj ;
		
		bool check1 = inputs[0]->equals(op->inputs[0])
				&& inputs[1]->equals(op->inputs[1]);
		bool check2 = inputs[1]->equals(op->inputs[0])
				&& inputs[0]->equals(op->inputs[1]);
		
		return (check1 || check2) && outputs[0]->equals(op->outputs[0]);

	}
	
	
	int AssertBasicOp::getNumMulGates() const {
		return 1;
	}
}

