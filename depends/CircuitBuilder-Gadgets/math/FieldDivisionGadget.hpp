
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <CircuitGenerator.hpp>
#include <AffinePoint.hpp>


namespace CircuitBuilder {
namespace Gadgets {

    
    class FieldDivisionGadget : public Gadget {

	private :
		
		const WirePtr a;
		const WirePtr b;
		WirePtr c;

		Wires outWires ;
		Instruction* evaluator ;
        friend class Evaluator ;



		class Evaluator : public Instruction  {
	        FieldDivisionGadget* parent ;
	    public:
	        Evaluator(CircuitGenerator * generator , FieldDivisionGadget* __parent) 
	        	: Instruction(generator) , parent(__parent) {}

    		void evaluate(CircuitEvaluator & evaluator) {
				BigInteger aValue = evaluator.getWireValue(parent->a);
				BigInteger bValue = evaluator.getWireValue(parent->b);
				BigInteger binvValue = bValue.modInverse(generator->config.FIELD_PRIME);
				BigInteger cValue = aValue.multiply(binvValue).mod(generator->config.FIELD_PRIME);
				
				evaluator.setWireValue(parent->c, cValue);
			}
	        
	    } ;


		void buildCircuit() {

			// This is an example of computing a value outside the circuit and
			// verifying constraints about it in the circuit
			evaluator = new Evaluator (generator, this);
			generator->specifyProverWitnessComputation(evaluator );
			

			/*
			 * Two notes: 1) The order of the above two statements matters (the
			 * specification and the assertion). In the current version, it's not
			 * possible to swap them, as in the evaluation sequence, the assertion
			 * must happen after the value is assigned.
			 * 
			 * 2) The instruction defined above relies on the values of wires (a)
			 * and (b) during runtime. This means that if any point later in the
			 * program, the references a, and b referred to other wires, these wires
			 * are going to be used instead in this instruction. Therefore, it will
			 * be safer to use final references in cases like that to reduce the
			 * probability of errors.
			 */
		}


	public:

	 	FieldDivisionGadget(CircuitGenerator * generator, 
	 						WirePtr __a, WirePtr __b, 
	 						string desc = "") 
	 		: Gadget(generator, desc ) , a(__a) , b(__b) , evaluator(NULL)
	 	{
			
			// if the input values are constant (i.e. known at compilation time), we
			// can save one constraint
			if (a->instanceof_ConstantWire() && b->instanceof_ConstantWire() ) {
				
				BigInteger aVal = ((ConstantWire*) a)->getConstant();
				BigInteger bVal = ((ConstantWire*) b)->getConstant();
				BigInteger binv = bVal.modInverse(generator->config.FIELD_PRIME);
				BigInteger cVal = binv.multiply(aVal).mod(generator->config.FIELD_PRIME);
				
				c = generator->createConstantWire(cVal);
				
			} else {
				c = generator->create_prover_witness_wire("division result");
				buildCircuit();
			}
		}

		~FieldDivisionGadget(){
			if ( evaluator) { delete evaluator ; }
		}
	
	
		Wires & getOutputWires() {
			outWires = WireVector ( c ) ;
	        return outWires ;
		}

	};

}}
