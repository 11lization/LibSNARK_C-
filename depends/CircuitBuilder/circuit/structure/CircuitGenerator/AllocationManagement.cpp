
#include <Wire.hpp>
#include <BasicOp.hpp>
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <WireLabelInstruction.hpp>
#include <Gadget.hpp>

#include <logging.hpp>

namespace CircuitBuilder {


	void add_allocation( CircuitGenerator * generator , Object * allocation ){
		generator->add_allocation(allocation);
	}

	void CircuitGenerator::add_allocation(Object * allocation){

        if(allocation->instanceof_Wire()){
           
            WirePtr w = (WirePtr)allocation ;
            AllocatedWires[w] = w ;
        
        }else if ( allocation->instanceof_BasicOp() ){
        
            BasicOp* op = (BasicOp*)allocation ;
            AllocatedOperators[op] = op ;
        
        }else if ( allocation->instanceof_WireLabelInstruction() ){

            WireLabelInstructionPtr wl = (WireLabelInstructionPtr)allocation ;
            AllocatedWireLabelInstructions[wl] = wl ;
        
        } else if ( allocation->instanceof_Gadget() ) {
            
            Gadget* g = (Gadget*) allocation ;
            AllocatedGadgets[g] = g ;
        
        } else if ( allocation->instanceof_WireArray() ){
        
            WireArray* warray = ( WireArray*) allocation ;
            AllocatedWireArrays[warray] = warray ;
        
        }
	
    }

	void CircuitGenerator::deallocate( BasicOp * op ){
        toDeallocate.push_back(op);
		return ;
	}

    void CircuitGenerator::clean_deallocated(){

        const size_t cleaned_op = toDeallocate.size();
        size_t cleaned_w = 0 ;

        for ( BasicOp* op : toDeallocate ){
            
            { 
                const Wires & ws = op->getOutputs();
                for ( WirePtr w : ws ){
                    AllocatedWires.erase(w);
                    delete w ;
                    cleaned_w++ ;
                }
            }

            AllocatedOperators.erase(op);
            delete op ;
        }

        LOGD( "Deallocate %zu Operators , %zu Wires \n" , cleaned_op , cleaned_w );

        toDeallocate.clear();

    }

    void CircuitGenerator::finalize(){
		
		if (circuitEvaluator){ 
			circuitEvaluator->clear();
			delete circuitEvaluator ;
		} 

        inWires.clear();
		outWires.clear() ;
		proverWitnessWires.clear() ;
		knownConstantWires.clear() ;

        primary_inputs.clear();
        primary_array_inputs.clear();


        LOGD( "Deallocate %zu Operators , %zu Wires \n" , AllocatedOperators.size() , AllocatedWires.size());

        for( auto Itr : AllocatedWires ){
            WirePtr w = Itr.second ;
			delete w ;
		}

		for( auto Itr : AllocatedOperators ){
            BasicOp* op = Itr.second ;
			delete op ;
		}

        for ( auto Itr : AllocatedWireLabelInstructions){
            WireLabelInstructionPtr wl = Itr.second;
            delete wl ;
        }

        for ( auto Itr : AllocatedGadgets ){
            Gadget* g = Itr.second ;
            delete g ;
        }

        for ( auto Itr : AllocatedWireArrays ){
            WireArray* warray = Itr.second;
            delete warray ;
        }

        AllocatedOperators.clear() ;
        AllocatedWires.clear() ;
        AllocatedWireLabelInstructions.clear();
        AllocatedWireArrays.clear() ;
        AllocatedGadgets.clear() ;
	}
     
}