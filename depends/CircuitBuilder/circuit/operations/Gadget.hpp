 
#pragma once

#include <global.hpp>
#include <CircuitGenerator.hpp>


namespace CircuitBuilder { 

	class Gadget : public Object {

	protected :

		CircuitGenerator *generator;
		string description;

	public :

		Gadget(CircuitGenerator * __generator , string desc = "") 
			: generator(__generator) , description (desc) 
		{
			class_id = class_id | Object::Gadget_Mask ;
		}

		virtual ~Gadget(){}

		virtual Wires & getOutputWires() = 0 ;
	
		virtual string toString() const {
			return "CircuitBuilder.Gadget " + description ;
		}
		
		string debugStr(string s) {
			return toString() + ":" + s;
		}
	};
}
