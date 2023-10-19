
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <AddBasicOp.hpp>

#include <logging.hpp>

namespace CircuitBuilder {

	 
	AddBasicOp::AddBasicOp( 
		CircuitGenerator *generator , 
		WireVector &ws ,
		WirePtr out, 
		const string & desc ) 
	:   BasicOp( generator , 
					ADD_OPCODE ,
					ws ,
					out,
					desc )
	{
		class_id = class_id | Object::AddBasicOp_Mask ;
	}


	string AddBasicOp::getOpcode() const {
		return "add";
	}


	void AddBasicOp::compute( CircuitEvaluator & evaluator ) {
		BigInteger s = BigInteger::ZERO() ;
		for (WirePtr w : inputs ) {
			s.__add( evaluator.getAssignment( w->getWireId()));
		}
		evaluator.setWireValue ( outputs[0] , s.mod(generator->config.FIELD_PRIME) ) ;
	}


	bool AddBasicOp::equals(Object *obj) const {

		if (this == obj)
			return true;
		if (!(obj->instanceof_AddBasicOp())) {
			return false;
		}

		AddBasicOp *op = (AddBasicOp*) obj;
		
		if( op->inputs.size() != inputs.size() ){
			return false;
		}
		
		if(inputs.size() == 2){
			bool check1 = inputs[0]->equals(op->inputs[0])
					&& inputs[1]->equals(op->inputs[1]);
			bool check2 = inputs[1]->equals(op->inputs[0])
					&& inputs[0]->equals(op->inputs[1]);
			return check1 || check2;
		} else {
			bool check = true;
			for(size_t i = 0; i < inputs.size(); i++){
				check = check && inputs[i]->equals(op->inputs[i]);
			}
			return check;
		}
		return true ;
	}


	int AddBasicOp::getNumMulGates() const {
		return 0;
	}

}