

#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 


namespace CircuitBuilder { 

	class NonZeroCheckBasicOp : public BasicOp {

	public:

		NonZeroCheckBasicOp(CircuitGenerator *generator , 
							WirePtr w ,
							WirePtr out1, WirePtr out2 , 
							const string & desc = "") ;

		string getOpcode() const ;
		
		void compute(CircuitEvaluator & evaluator) ;

		bool equals(Object *obj) const ;
		
		int getNumMulGates() const ;

	};
}
