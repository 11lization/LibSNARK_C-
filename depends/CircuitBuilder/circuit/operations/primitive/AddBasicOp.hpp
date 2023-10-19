

#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp>
#include <WireArray.hpp>


namespace CircuitBuilder {

	class AddBasicOp : public BasicOp {

	public : 

		AddBasicOp( CircuitGenerator *generator , 
					WireVector &ws ,
					WirePtr out, 
					const string & desc = "" ) ;

		string getOpcode() const ;

		void compute( CircuitEvaluator & evaluator ) ;

		bool equals(Object *obj) const ;

		int getNumMulGates() const ;

	};

}