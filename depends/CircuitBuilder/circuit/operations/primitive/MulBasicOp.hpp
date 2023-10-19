
#pragma once


#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 


namespace CircuitBuilder { 

	class MulBasicOp : public BasicOp {

	public:

		MulBasicOp( CircuitGenerator *generator , 
					WirePtr w1, WirePtr w2, 
					WirePtr output, 
					const string & desc = "") ;
	
		string getOpcode() const ;
		
		void compute(CircuitEvaluator & evaluator) ;

		bool equals(Object *obj) const ;
		
		int getNumMulGates() const ;

	};
}