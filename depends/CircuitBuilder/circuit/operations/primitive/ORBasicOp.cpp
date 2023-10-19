

#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <ORBasicOp.hpp> 

#include <logging.hpp>


namespace CircuitBuilder { 


	ORBasicOp::ORBasicOp(CircuitGenerator *generator , 
						 WirePtr w1, WirePtr w2, 
						 WirePtr out, 
						 const string & desc ) 
		:   BasicOp( generator , 
						OR_OPCODE,
						w1 , w2 ,
						out ,
						desc )
	{
		class_id = class_id | Object::ORBasicOp_Mask ;
	}


	string ORBasicOp::getOpcode() const {
		return "or";
	}
	
	
	void ORBasicOp::checkInputs(CircuitEvaluator & evaluator) {
		
		BasicOp::checkInputs(evaluator);

		bool check = evaluator.getAssignment(inputs[0]->getWireId()).isBinary() && 
					 evaluator.getAssignment(inputs[1]->getWireId()).isBinary() ;
		
		if (!check){			
			LOGD("Error - Input(s) to OR are not binary. %s" , this->toString().c_str() );
			throw runtime_error("Error During Evaluation");
		}
	}


	void ORBasicOp::compute(CircuitEvaluator & evaluator) {
		evaluator.setWireValue( outputs[0] , evaluator.getAssignment (inputs[0]->getWireId()).OR(evaluator.getAssignment(inputs[1]->getWireId()))  ) ;
	}

	bool ORBasicOp::equals(Object *obj) const {

		if (this == obj){
			return true;
		}
		if (!(obj->instanceof_ORBasicOp())) {
			return false;
		}

		ORBasicOp *op = (ORBasicOp*) obj;

		bool check1 = inputs[0]->equals(op->inputs[0])
				&& inputs[1]->equals(op->inputs[1]);
		bool check2 = inputs[1]->equals(op->inputs[0])
				&& inputs[0]->equals(op->inputs[1]);
		return check1 || check2;

	}
	

	int ORBasicOp::getNumMulGates() const {
		return 1;
	}
	
}