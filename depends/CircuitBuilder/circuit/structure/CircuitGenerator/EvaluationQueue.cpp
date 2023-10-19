

 
#include <Instruction.hpp>
#include <Wire.hpp>
#include <BasicOp.hpp>
#include <CircuitGenerator.hpp>

#include <logging.hpp>

namespace CircuitBuilder {




	EvaluationQueue::EvaluationQueue( size_t estimated_queue_size ){
		put_order = 0 ;
		vector_alloc_size = estimated_queue_size + 1000 ;
		base_insertion_order = vector<InstructionPtr>(vector_alloc_size);
	}

 
	void EvaluationQueue::put(InstructionPtr instr  ){
		
		if ( put_order >= vector_alloc_size ){
			vector_alloc_size += 1000 ;
			base_insertion_order.resize(vector_alloc_size) ;
		}

		base_insertion_order[put_order] = instr  ;

		hashCode_t hs = instr ->hashCode() ;

		auto node = base_hash_map.find(hs) ; 
		
		if( node == base_hash_map.end() ){
			base_hash_map[hs] = vector<size_t>(1 , put_order ) ;
		}else{
			node->second.push_back(put_order) ;
		}

		put_order++ ; 
	}


	InstructionPtr EvaluationQueue::find_Instruction_by_Inputs ( InstructionPtr instr ){
		
		hashCode_t hs = instr->hashCode() ;

		auto node = base_hash_map.find(hs) ; 

		if( node == base_hash_map.end() ){
			return NULL ;
		}
			
		// compare instructions
		for ( size_t index : node->second ){
			
			InstructionPtr i_instr = base_insertion_order[index] ;

			if ( ! (instr->instanceof_BasicOp() && i_instr->instanceof_BasicOp()) ) {
				continue ;
			}
				
			BasicOp * instr_op = (BasicOp*) instr ;
			BasicOp * i_instr_op = (BasicOp*) i_instr ;

			if (instr_op->op_code() != i_instr_op->op_code() ){
				continue ;
			}

			// compare inputs
			if ( instr_op->getInputs().size() != i_instr_op->getInputs().size() ){
				continue ;
			}

			// compare inputs wire ids
			bool matched = true ;
			for ( size_t i_input = 0 ; i_input < instr_op->getInputs().size() ; i_input ++ ){
				if( instr_op->getInputs()[i_input]->getWireId() != i_instr_op->getInputs()[i_input]->getWireId() ){
					matched = false ;
					break ;
				}
			}
			
			if( ! matched){
				continue ;
			}
			
			// matched 
			return i_instr ;
			
		}

		// inputs did not match
		return NULL ;
	}


}