
#pragma once

#include <global.hpp> 
#include <BitWire.hpp>


namespace CircuitBuilder {

	class VariableBitWire : public BitWire {

	public: 
		
		VariableBitWire(CircuitGenerator * generator, int wireId) 
			: BitWire( generator, wireId) 
		{
			class_id = class_id | Object::VariableBitWire_Mask ;
		}


		WireArray * getBitWires() {
			return allocate<WireArray>(generator ,  this , 1 ) ;
		}

	};	

}