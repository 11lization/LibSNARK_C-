 
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <PackBasicOp.hpp> 

#include <logging.hpp>
 

namespace CircuitBuilder { 

	PackBasicOp::PackBasicOp(CircuitGenerator *generator , 
							 const Wires &inBits,
							 WirePtr out, 
							 const string & desc) 
		:   BasicOp( generator , 
						PACK_OPCODE,
						inBits , 
						Wires (out) , 
						desc )
	{
		class_id = class_id | Object::PackBasicOp_Mask ;
	}


	string PackBasicOp::getOpcode() const {
		return "pack";
	}
	
	
	void PackBasicOp::checkInputs(CircuitEvaluator & evaluator) {
		
		BasicOp::checkInputs(evaluator);
		
		bool check = true;
		
		for (size_t i = 0; i < inputs.size(); i++) {
			check &= evaluator.getAssignment(inputs[i]->getWireId()).isBinary() ;
		}

		if (!check) {
			LOGD("Error - Input(s) to Pack are not binary. %s\n" , this->toString().c_str() );
			throw runtime_error ("Error During Evaluation");
		}
	}


	void PackBasicOp::compute(CircuitEvaluator & evaluator) {
		
		BigInteger sum = BigInteger::ZERO();
		
		for (size_t i = 0; i < inputs.size(); i++) {
			BigInteger bI("2");
			bI.__pow(i);
			sum.__add( evaluator.getAssignment(inputs[i]->getWireId()).multiply(bI));
		}
		
		evaluator.setWireValue (outputs[0] , sum.mod(generator->config.FIELD_PRIME) );
	}


	bool PackBasicOp::equals(Object *obj) const {

		if (this == obj){
			return true;
		}
		if (!(obj->instanceof_PackBasicOp())) {
			return false;
		}

		PackBasicOp *op = (PackBasicOp*) obj ;
		
		if (op->inputs.size() != inputs.size()){
			return false;
		}

		bool check = true;
		for (size_t i = 0; i < inputs.size(); i++) {
			check = check && inputs[i]->equals(op->inputs[i]);
		}

		return check;
	}
	

	int PackBasicOp::getNumMulGates() const {
		return 0;
	}

}
