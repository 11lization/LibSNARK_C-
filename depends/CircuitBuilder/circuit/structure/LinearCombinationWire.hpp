
#pragma once

#include <global.hpp>
#include <WireArray.hpp>
 
namespace CircuitBuilder { 

	class LinearCombinationWire : public Wire {

	public :

		LinearCombinationWire( CircuitGenerator* generator , int wireId) 
			: Wire(generator, wireId) 
		{	
			class_id = class_id | Object::LinearCombinationWire_Mask ;
		}
	
		LinearCombinationWire(CircuitGenerator* generator , WireArray * bits) 
			: Wire( generator , bits ) 
		{
			class_id = class_id | Object::LinearCombinationWire_Mask ;
			setBits(bits);
		}
	
		WireArray * getBitWires() {
			return bitWires;
		}

		void setBits(WireArray *  __bitWires) {
			bitWires = __bitWires;
		}

	};
}
