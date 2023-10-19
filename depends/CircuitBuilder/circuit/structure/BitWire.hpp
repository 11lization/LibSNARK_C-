 


#pragma once

#include <global.hpp>

#include <Wire.hpp>

namespace CircuitBuilder { 

	class BitWire : public Wire {

	public:
		
		BitWire(CircuitGenerator * generator, int wireId);

		WirePtr mul( const WirePtr w, const string & desc  ) ;

		WirePtr mul( const BigInteger &b, const string & desc ) ;

		WirePtr invAsBit( const string & desc  ) ;
		
		WirePtr OR ( const WirePtr w, const string & desc  ) ;
		
		WirePtr XOR ( const WirePtr w, const string & desc  ) ; 
		
		WireArray * getBits( const WirePtr w, int bitwidth, const string & desc );
		
	};
}
