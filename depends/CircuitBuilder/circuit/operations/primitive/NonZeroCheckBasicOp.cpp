

#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <NonZeroCheckBasicOp.hpp> 

#include <logging.hpp>

 

namespace CircuitBuilder { 

	
	NonZeroCheckBasicOp::NonZeroCheckBasicOp(CircuitGenerator *generator , 
											 WirePtr w ,
											 WirePtr out1, WirePtr out2 , 
											 const string & desc) 
		:   BasicOp( generator , 
					NONZEROCHECK_OPCODE,
					Wires(w) , 
					Wires( out1 , out2 ) , 
					desc )
	{
		class_id = class_id | Object::NonZeroCheckBasicOp_Mask ;
	}

	
	string NonZeroCheckBasicOp::getOpcode() const {
		return "zerop";
	}
	

	void NonZeroCheckBasicOp::compute(CircuitEvaluator & evaluator) {

		if ( evaluator.getAssignment(inputs[0]->getWireId()).signum() == 0) {
			evaluator.setWireValue (outputs[1] , BigInteger::ZERO() );
		} else {
			evaluator.setWireValue (outputs[1] , BigInteger::ONE() );
		}
		evaluator.setWireValue (outputs[0] , BigInteger::ZERO() ); // a dummy value
	}
	

	bool NonZeroCheckBasicOp::equals(Object *obj) const {

		if (this == obj){
			return true;
		}
		if (!(obj->instanceof_NonZeroCheckBasicOp())) {
			return false;
		}

		NonZeroCheckBasicOp *op = (NonZeroCheckBasicOp*) obj;
		return inputs[0]->equals(op->inputs[0]);

	}

	
	int NonZeroCheckBasicOp::getNumMulGates() const {
		return 2;
	}

}
