

#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>

 

namespace CircuitBuilder { 

	class ConstMulBasicOp : public BasicOp {

	private : 
		
		bool inSign;
		BigInteger constInteger;
		
		BigInteger assign_const(CircuitGenerator *generator , const BigIntegerPtr __constInteger );

		BigInteger assign_const(CircuitGenerator *generator , const BigInteger &__constInteger );

	public: 

		ConstMulBasicOp(CircuitGenerator *generator , 
						WirePtr w, WirePtr out, 
						const BigIntegerPtr __constInteger, 
						const string & desc = "" ) ;
		
		ConstMulBasicOp(CircuitGenerator *generator , 
						WirePtr w, WirePtr out, 
						const BigInteger &__constInteger, 
						const string & desc = "" ) ;
		
		string getOpcode() const ;

		bool get_inSign() const ;

		BigInteger getOpCodeConstInteger();
		
		void compute(CircuitEvaluator & evaluator) ;
		
		bool equals(Object *obj) const ;
		
		int getNumMulGates() const ;
		
	};
}