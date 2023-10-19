
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <MulBasicOp.hpp> 

#include <logging.hpp>


namespace CircuitBuilder { 


	MulBasicOp::MulBasicOp( CircuitGenerator *generator , 
							WirePtr w1, WirePtr w2, 
							WirePtr output, 
							const string & desc ) 
		:   BasicOp( generator , 
					MUL_OPCODE ,
					w1 , w2 ,
					output , 
					desc )
	{
		class_id = class_id | Object::MulBasicOp_Mask ;
	}


	string MulBasicOp::getOpcode() const {
		return "mul";
	}
	

	void MulBasicOp::compute(CircuitEvaluator & evaluator) {
		BigInteger result = evaluator.getAssignment(inputs[0]->getWireId()). multiply(evaluator.getAssignment (inputs[1]->getWireId()));
		
		if (result.compareTo(generator->config.FIELD_PRIME) > 0) {
			result = result.mod(generator->config.FIELD_PRIME);
		}
		evaluator.setWireValue(outputs[0] , result );
	}


	bool MulBasicOp::equals(Object *obj) const {

		if (this == obj){
			return true;
		}
		
		if (!(obj->instanceof_MulBasicOp())) {
			return false;
		}

		MulBasicOp *op = (MulBasicOp*) obj ;

		bool check1 = inputs[0]->equals(op->inputs[0])
				&& inputs[1]->equals(op->inputs[1]);
		bool check2 = inputs[1]->equals(op->inputs[0])
				&& inputs[0]->equals(op->inputs[1]);
		
		return check1 || check2;

	}
	

	int MulBasicOp::getNumMulGates() const {
		return 1;
	}

}