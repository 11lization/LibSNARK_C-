

#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <ConstMulBasicOp.hpp>

#include <logging.hpp>

namespace CircuitBuilder { 
 

	BigInteger ConstMulBasicOp::assign_const(CircuitGenerator *generator , const BigIntegerPtr __constInteger ){
		bool __inSign = __constInteger->signum() == -1;
		if (!__inSign) {
			BigInteger constbI = __constInteger->mod(generator->config.FIELD_PRIME);
			return constbI ;
		} else {
			BigInteger constbI = __constInteger->negate();
			constbI = constbI.mod(generator->config.FIELD_PRIME);
			return generator->config.FIELD_PRIME.subtract(constbI);
		}
	}


	BigInteger ConstMulBasicOp::assign_const(CircuitGenerator *generator , const BigInteger &__constInteger ){
		return assign_const(generator , (const BigIntegerPtr) & __constInteger ) ;
	}

		
	ConstMulBasicOp::ConstMulBasicOp( 
									CircuitGenerator *generator , 
									WirePtr w, WirePtr out, 
									const BigIntegerPtr __constInteger, 
									const string & desc ) 
								: 	BasicOp( generator , 
											MULCONST_OPCODE ,
											w, 
											out,
											desc ) ,
									inSign(__constInteger->signum() == -1) ,
									constInteger(assign_const(generator , __constInteger ))
	{
		class_id = class_id | Object::ConstMulBasicOp_Mask ;
	}


	ConstMulBasicOp::ConstMulBasicOp( 
		CircuitGenerator *generator , 
		WirePtr w, WirePtr out, 
		const BigInteger &__constInteger, 
		const string & desc ) 
	: 	BasicOp( generator , 
					MULCONST_OPCODE ,
					w,
					out,
					desc ) ,
		inSign(__constInteger.signum() == -1) ,
		constInteger(assign_const(generator , __constInteger ))
	{
		class_id = class_id | Object::ConstMulBasicOp_Mask ;
	}


	string ConstMulBasicOp::getOpcode() const {
		if (!inSign) {
			return "const-mul-" + constInteger.toString(16);
		} else{
			return "const-mul-neg-" + generator->config.FIELD_PRIME.subtract(constInteger).toString(16);
		}
	}

	bool ConstMulBasicOp::get_inSign() const {
		return inSign ;
	}

	
	BigInteger ConstMulBasicOp::getOpCodeConstInteger(){
		if (!inSign) {
			return  constInteger ;
		} else{
			return generator->config.FIELD_PRIME.subtract(constInteger) ;
		}
	}
	
	
	void ConstMulBasicOp::compute(CircuitEvaluator & evaluator) {
		BigInteger result = evaluator.getAssignment(inputs[0]->getWireId()).multiply(constInteger);
		if (result.bitLength() >= generator->config.LOG2_FIELD_PRIME) {
			result = result.mod(generator->config.FIELD_PRIME);
		}
		evaluator.setWireValue( outputs[0] , result) ;
	}
	
	
	bool ConstMulBasicOp::equals(Object *obj) const {
		
		if (this == obj)
			return true;
		if (!(obj->instanceof_ConstMulBasicOp())) {
			return false;
		}

		ConstMulBasicOp *op = (ConstMulBasicOp*) obj;

		return inputs[0]->equals( op->inputs[0] ) && constInteger.equals(op->constInteger);
	}
	
	int ConstMulBasicOp::getNumMulGates() const {
		return 0;
	}
	
}