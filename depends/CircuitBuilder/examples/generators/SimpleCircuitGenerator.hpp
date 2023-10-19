
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>


namespace CircuitBuilder {

	class SimpleCircuitGenerator : public CircuitGenerator {

	private :
		Wires inputs ;
		
	protected :

		void buildCircuit() ;

	public:
	
		SimpleCircuitGenerator(string circuitName , Config config ) ;
		void generateSampleInput(CircuitEvaluator & circuitEvaluator);

	};
}
