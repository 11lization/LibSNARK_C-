

#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 


#include <logging.hpp>



namespace CircuitBuilder {

	class AssertBasicOp : public BasicOp {

	public:

		AssertBasicOp( 
            CircuitGenerator *generator , 
            WirePtr w1, WirePtr w2, 
            WirePtr out, 
            const string & desc = "" ) ;

		void compute( CircuitEvaluator & evaluator) ;

		void checkOutputs( CircuitEvaluator & evaluator ) ;
		
		string getOpcode() const ;
		
		bool equals(Object *obj) const ;
		
		int getNumMulGates() const ;

	};
}

