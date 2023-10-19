
#pragma once


#include <global.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <CircuitEvaluator.hpp>

#include <logging.hpp>

namespace CircuitBuilder {

	enum LabelType {
		input = 1, 
		output = 2,
		nizkinput = 3,
		debug = 4
	} ;


	class WireLabelInstruction : public  Instruction {

	private : 

		LabelType type ;
		WirePtr w;
		uint32_t desc_id ;
		
	public : 

		WireLabelInstruction(CircuitGenerator * generator ,
							 LabelType __type, WirePtr __w) 
			: Instruction(generator) , 
			  type(__type) , w(__w) , desc_id( generator->set_desc("not set"))
		{
			class_id = class_id | Object::WireLabelInstruction_Mask ;
		}

		WireLabelInstruction(CircuitGenerator * generator ,
							 LabelType __type, WirePtr __w, 
							 const string &desc ) 
			: Instruction(generator) , 
			  type(__type) , w(__w), desc_id( generator->set_desc(desc) ) 
		{
			class_id = class_id | Object::WireLabelInstruction_Mask ;
		}

		WirePtr getWire() {
			return w;
		}

		string type_str() const {
			switch(type){
				case input :
					return "input" ;
					break ;
				case output :
					return "output" ;
					break ;
				case nizkinput :
					return "nizkinput" ;
					break ;
				case debug :
					return "debug" ;
					break ;
			}
		}

		string toString() const {
			string desc = generator->get_desc(desc_id);
			return type_str() + " " + w->toString() + (desc.size() == 0 ? "" : "\t\t\t # " + desc );
		}

		void evaluate(CircuitEvaluator &evaluator) {
			UNUSEDPARAM(evaluator)
			// nothing to do.
		}

		
		void emit(CircuitEvaluator & evaluator) {

			string desc = generator->get_desc(desc_id);	

			if (((type == LabelType::output) && (generator->config.outputVerbose)) || 
				((type == LabelType::debug) && (generator->config.debugVerbose)))
			{
				stringstream ss ;
				
				ss << "\t[" << type << "] Value of Wire # " << w->toString() << (desc.length() > 0 ? " (" + desc + ")" : "") << " :: "
						<< evaluator.getWireValue(w).toString(generator->config.hexOutputEnabled ? 16 : 10) ;

				LOGD("%s\n" , ss.str().c_str());
			}
		}

		LabelType getType() {
			return type;
		}

		bool doneWithinCircuit() {
			return type != LabelType::debug;
		}

	};
}
