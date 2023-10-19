

#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 
 

namespace CircuitBuilder { 

	class SplitBasicOp : public BasicOp {

	public:
		
        SplitBasicOp( CircuitGenerator *generator , 
					  WirePtr w,
					  const Wires &outs, 
					  const string & desc = "" ) ;
	
		string getOpcode() const ;
		
		void checkInputs(CircuitEvaluator & evaluator) ;

		protected :void compute(CircuitEvaluator & evaluator) ;

		bool equals(Object *obj) const ;

		public : int getNumMulGates() const ;

	};
}
