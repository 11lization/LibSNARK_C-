
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <XorBasicOp.hpp> 

#include <logging.hpp>

namespace CircuitBuilder { 

		
	XorBasicOp::XorBasicOp( CircuitGenerator *generator , 
							WirePtr w1, WirePtr w2, 
							WirePtr out, 
							const string & desc  ) 
		:   BasicOp( generator , 
		 			 XOR_OPCODE,
					 w1 , w2 ,
					 out , 
					 desc )
	{
		class_id = class_id | Object::XorBasicOp_Mask ;
	}
	

	string XorBasicOp::getOpcode() const {
		return "xor";
	}

	
	void XorBasicOp::checkInputs(CircuitEvaluator & evaluator) {
		
		BasicOp::checkInputs(evaluator);
		
		bool check = evaluator.getAssignment(inputs[0]->getWireId()).isBinary() && 
					 evaluator.getAssignment(inputs[1]->getWireId()).isBinary() ;
		
		if (!check){
			LOGD("Error - Input(s) to XOR are not binary. %s \n" ,this->toString().c_str() );
			throw runtime_error("Error During Evaluation");
		}
	}

	
	void XorBasicOp::compute(CircuitEvaluator & evaluator) {
		evaluator.setWireValue( outputs[0] , evaluator.getAssignment (inputs[0]->getWireId()).XOR(evaluator.getAssignment(inputs[1]->getWireId()))  ) ;
	}

	
	bool XorBasicOp::equals(Object *obj) const {

		if (this == obj){
			return true;
		}

		if (!(obj->instanceof_XorBasicOp())) {
			return false;
		}

		XorBasicOp *op = (XorBasicOp*) obj ;

		bool check1 = inputs[0]->equals(op->inputs[0])
				&& inputs[1]->equals(op->inputs[1]);
		bool check2 = inputs[1]->equals(op->inputs[0])
				&& inputs[0]->equals(op->inputs[1]);
		
		return check1 || check2;

	}
	
	int XorBasicOp::getNumMulGates() const {
		return 1;
	}
	

}