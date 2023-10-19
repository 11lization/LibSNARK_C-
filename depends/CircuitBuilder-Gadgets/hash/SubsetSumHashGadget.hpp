
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
namespace Gadgets {

	class SubsetSumHashGadget : public CircuitBuilder::Gadget {

	private:
		
		typedef struct {
			int INPUT_LENGTH ;
			vector<vector<BigInteger>> COEFFS;
		} Static_t ;

		Wires inputWires;
		Wires outWires;
		bool binaryOutput;
		Static_t* static_data ;
		
		static map< int , Static_t > StaticDataList ;

		void buildCircuit() ;

	public: 
		
		static const int DIMENSION = 3; // set to 4 for higher security 10
		
		SubsetSumHashGadget(CircuitGenerator * generator, Wires &ins, bool binaryOutput, string desc = "")  ;

		Wires & getOutputWires() ;

		~SubsetSumHashGadget(){}
		
		static void init_static_members( const Config & config ) ;

	};

}}