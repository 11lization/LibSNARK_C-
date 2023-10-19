

#pragma once


#include <global.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
 

namespace CircuitBuilder {

	class VariableWire : public Wire {
	
	public :
	
		VariableWire(CircuitGenerator * generator, int wireId) 
			: Wire( generator , wireId) 
		{
			class_id = class_id | Object::VariableWire_Mask ;
		}
			
		
		VariableWire(CircuitGenerator * generator , WireArray * bits) 
			: Wire (generator , bits ) 
		{
			class_id = class_id | Object::VariableWire_Mask ;
		}


		WireArray * getBitWires() {
			return bitWires;
		}

		void setBits(WireArray * __bitWires) {
			bitWires = __bitWires;
		}

	};
}
