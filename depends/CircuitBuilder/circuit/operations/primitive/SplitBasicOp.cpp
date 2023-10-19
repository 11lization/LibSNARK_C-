
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <SplitBasicOp.hpp> 

#include <logging.hpp>

 

namespace CircuitBuilder { 

	
	SplitBasicOp::SplitBasicOp( CircuitGenerator *generator , 
								WirePtr w,
								const Wires &outs, 
								const string & desc ) 
		:   BasicOp( generator , 
						SPLIT_OPCODE,
						Wires(w) ,
						outs , 
						desc )
	{
		class_id = class_id | Object::SplitBasicOp_Mask ;
	}


	string SplitBasicOp::getOpcode() const {
		return "split";
	}
	
	void SplitBasicOp::checkInputs(CircuitEvaluator & evaluator) {
		
		BasicOp::checkInputs(evaluator);
		
		auto bitLength = evaluator.getAssignment(inputs[0]->getWireId()).bitLength() ;
		
		if ( outputs.size() < bitLength ) {
			LOGD( "test:: %zu :: %zu " , outputs.size() , bitLength  );
			
			LOGD("Error in Split --- The number of bits does not fit -- Input: %s \n\t%s" ,
					evaluator.getAssignment(inputs[0]->getWireId()).toString(16).c_str() , this->toString().c_str() );
			
			throw runtime_error ("Error During Evaluation -- " + this->toString() );
		}
	}


	void SplitBasicOp::compute(CircuitEvaluator & evaluator) {

		BigInteger inVal = evaluator.getAssignment (inputs[0]->getWireId());

		if (inVal.compareTo(generator->config.FIELD_PRIME) > 0) {
			evaluator.setWireValue ( inputs[0] , inVal.mod(generator->config.FIELD_PRIME) ) ;
		}

		for (size_t i = 0; i < outputs.size(); i++) {
			evaluator.setWireValue ( outputs[i] , inVal.testBit(i) ? BigInteger::ONE() : BigInteger::ZERO() ) ;
		}
	}


	bool SplitBasicOp::equals(Object *obj) const {

		if (this == obj){
			return true;
		}
		if (!(obj->instanceof_SplitBasicOp())) {
			return false;
		}

		SplitBasicOp *op =  (SplitBasicOp*) obj ;
		return inputs[0]->equals(op->inputs[0]) && outputs.size() == op->outputs.size();

	}
	

	int SplitBasicOp::getNumMulGates() const {
		return outputs.size() + 1;
	}

}
