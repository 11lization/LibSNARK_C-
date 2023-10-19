
#pragma once 

#include <global.hpp>
 
namespace CircuitBuilder {

	class Config {

	public :
		
		int EC_Selection ;
		BigInteger FIELD_PRIME ;
		size_t LOG2_FIELD_PRIME ;
		bool outputVerbose ;
		bool debugVerbose ;
		bool hexOutputEnabled ;
		BigInteger CURVE_ORDER ;
		BigInteger SUBGROUP_ORDER ;
		string hashType;

		Config() : FIELD_PRIME(0l) , CURVE_ORDER(0l) {
			
			hexOutputEnabled = true ;
			outputVerbose = true ;
			debugVerbose = true ;

			evaluationQueue_size = 0 ;
			inWires_size = 0 ;
			outWires_size  = 0 ;
			proverWitnessWires_size = 0 ;
		
		}


		// estimated final counts for following arrays
		size_t evaluationQueue_size ;
		size_t inWires_size ;
		size_t outWires_size ;
		size_t proverWitnessWires_size ;

	};
}
