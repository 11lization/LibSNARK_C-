
#pragma once

#include <global.hpp>
#include <WireArray.hpp>
 

namespace CircuitBuilder { 
	
	class LinearCombinationBitWire : public BitWire {

	public: 
		
		LinearCombinationBitWire(CircuitGenerator * generator , int wireId ) 
			: BitWire(generator ,  wireId) 
		{
			class_id = class_id | Object::LinearCombinationBitWire_Mask ;
		}
		
		WireArray * getBitWires() {
			return allocate<WireArray>(generator , this );
		}

	};
}

