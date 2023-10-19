
#pragma once

#include <global.hpp>
#include <BaseClass.hpp>

namespace CircuitBuilder {

	class Instruction : public Object {

	protected :

		CircuitGenerator * generator;

	public : 

		Instruction( CircuitGenerator * __generator ) 
			: Object(), generator( __generator )
		{
			class_id = class_id | Object::Instruction_Mask ;
		}
		
		virtual void evaluate(CircuitEvaluator & evaluator) = 0 ;

		virtual void emit(CircuitEvaluator & evaluator) { UNUSEDPARAM(evaluator) }

		virtual bool doneWithinCircuit() {
			return false;
		}
	
	};

}
