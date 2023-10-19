
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
namespace Gadgets {

	class HashGadget : public CircuitBuilder::Gadget {

	private:
		Wires outWires;

	public: 
		
		HashGadget(CircuitGenerator * generator, const Wires & inputs, string desc = "")  ;
		~HashGadget(){}
		Wires & getOutputWires() ;

	};

}}

