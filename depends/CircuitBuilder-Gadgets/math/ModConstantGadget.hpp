
/**
 * This gadget provides the remainder of a % c, where c is a circuit constant.
 *
 *
 */

#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <CircuitGenerator.hpp>
#include <AffinePoint.hpp>


namespace CircuitBuilder {
namespace Gadgets {


	class ModConstantGadget : public Gadget {

	private :
		
		const WirePtr a;
		const WirePtr b;
		const BigInteger c;
		WirePtr r;
		WirePtr q;


		Wires outWires ;
		Instruction* evaluator ;
        friend class Evaluator ;


		class Evaluator : public Instruction  {
	        ModConstantGadget* parent ;
	        WirePtr ab ;
	    public:
	        Evaluator(CircuitGenerator * generator , ModConstantGadget* __parent , WirePtr __ab ) 
	        	: Instruction(generator) , parent(__parent), ab(__ab) {}

	        void evaluate(CircuitEvaluator & evaluator) {
				BigInteger mulValue = evaluator.getWireValue(ab);
			
				BigInteger rValue = mulValue.mod(parent->c);
				evaluator.setWireValue(parent->r, rValue);
				BigInteger qValue = mulValue.fdivide(parent->c);  // round with ceil or floor ? .
				evaluator.setWireValue(parent->q, qValue);
			}
	        
	    } ;
		

		void buildCircuit() {
			WirePtr ab = a->mul(b);
			r = generator->create_prover_witness_wire();
			q = generator->create_prover_witness_wire();

			// notes about how to use this code block can be found in FieldDivisionGadget
			evaluator = new Evaluator (generator, this, ab );
			generator->specifyProverWitnessComputation( evaluator);
		}



	public: 


		ModConstantGadget(CircuitGenerator * generator, 
						  WirePtr __a, BigInteger & __c, 
						  string desc="") 
			: Gadget(generator, desc ) , 
			  a(__a) , 
			  b(generator->oneWire ), 
			  c(__c) ,
			  evaluator(NULL)
		{
			if(c.signum() != 1){
				throw invalid_argument("c must be a positive constant. Signed operations not supported yet.");
			}

			// TODO: add further checks.
			
			buildCircuit();
		}


		ModConstantGadget(CircuitGenerator * generator, 
						  WirePtr __a, WirePtr __b, BigInteger &__c, 
						  string desc ="") 
			: Gadget(generator, desc ) , 
			  a(__a) , b(__b ), c(__c) , evaluator(NULL)
		{
			if(c.signum() != 1){
				throw invalid_argument ("c must be a positive constant. Signed operations not supported yet.");
			}

			// TODO: add further checks.
			
			buildCircuit();
		}


		~ModConstantGadget(){
			if (evaluator){ delete evaluator ; }
		}


		Wires & getOutputWires() {
			outWires = Wires ( r , q  ) ;
	        return outWires ;
		}

	};

}}
