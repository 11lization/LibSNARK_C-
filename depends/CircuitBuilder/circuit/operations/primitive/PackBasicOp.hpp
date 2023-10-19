 

#pragma once

#include <global.hpp>
#include <utilities.hpp>
#include <BasicOp.hpp> 


namespace CircuitBuilder { 

	class PackBasicOp : public BasicOp {

	public:

        PackBasicOp(CircuitGenerator *generator , 
					const Wires &inBits,
					WirePtr out, 
					const string & desc = "" ) ;

		string getOpcode() const ;
		
		void checkInputs(CircuitEvaluator & evaluator) ;
 
		void compute(CircuitEvaluator & evaluator) ;

		bool equals(Object *obj) const ;
		
		int getNumMulGates() const ;

	};
}
