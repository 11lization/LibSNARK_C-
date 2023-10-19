 

#include <BigInteger.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <WireArray.hpp>
#include <BitWire.hpp>
#include <ConstantWire.hpp>
#include <LinearCombinationBitWire.hpp>
#include <LinearCombinationWire.hpp>
#include <VariableBitWire.hpp>
#include <VariableWire.hpp>
#include <WireLabelInstruction.hpp>

#include <BasicOp.hpp>
#include <MulBasicOp.hpp>
#include <AssertBasicOp.hpp>

#include <Exceptions.hpp>
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>



#include <logging.hpp> 

namespace CircuitBuilder { 

	CircuitEvaluator::CircuitEvaluator(CircuitGenerator *__circuitGenerator){
		circuitGenerator = __circuitGenerator;
		valueAssignment = vector<BigInteger>(circuitGenerator->getNumWires() , BigInteger(0l) );
		valueAssignmentFlag = vector<bool>(circuitGenerator->getNumWires() , false );
		valueAssignment[circuitGenerator->oneWire->getWireId()] = BigInteger::ONE() ;
	}


	void CircuitEvaluator::setWireValue(WirePtr wire, const BigInteger &v) {
		if(v.signum() < 0 ){
			throw invalid_argument("Only positive values that are less than the modulus are allowed for this method.");
		}
		valueAssignment[wire->getWireId()] = v;
		valueAssignmentFlag[wire->getWireId()] = true ;
		return ;
	}


	void CircuitEvaluator::setWireValue(WirePtr wire, long v) {
		if(v < 0){
			throw invalid_argument("Only positive values that are less than the modulus are allowed for this method.");
		}
		valueAssignment[wire->getWireId()] = BigInteger( v );
		valueAssignmentFlag[wire->getWireId()] = true ;
		return ;
	}


	void CircuitEvaluator::setWireValue(Wire & wire, const BigInteger &v) {
		if(v.signum() < 0 ){
			throw invalid_argument("Only positive values that are less than the modulus are allowed for this method.");
		}
		valueAssignment[wire.getWireId()] = v;
		valueAssignmentFlag[wire.getWireId()] = true ;
		return ;
	}


	void CircuitEvaluator::setWireValue(Wire &wire, long v) {
		if(v < 0){
			throw invalid_argument("Only positive values that are less than the modulus are allowed for this method.");
		}
		valueAssignment[wire.getWireId()] = BigInteger( v );
		valueAssignmentFlag[wire.getWireId()] = true ;
		return ;
	}

	
	void CircuitEvaluator::setWireValue(vector<WirePtr> &wires , vector<BigInteger> &v) {
		for (size_t i = 0; i < v.size(); i++) {
			setWireValue(wires[i], v[i]);
		}
		for (size_t i = v.size(); i < wires.size(); i++) {
			setWireValue(wires[i] , BigInteger::ZERO());
		}
		return ;
	} 


	void CircuitEvaluator::setWireValue(Wires &wires , vector<BigInteger> &v) {
		for (size_t i = 0; i < v.size(); i++) {
			setWireValue(wires[i], v[i]);
		}
		for (size_t i = v.size(); i < wires.size(); i++) {
			setWireValue(wires[i] , BigInteger::ZERO());
		}
		return ;
	} 


	void CircuitEvaluator::flagAssigned(){
		valueAssignmentFlag = vector<bool>(valueAssignmentFlag.size() , true );
	}
	
	
	void CircuitEvaluator::flagAssigned(wireID_t wireID){
		valueAssignmentFlag[wireID] = true ;
	}


	BigInteger CircuitEvaluator::getWireValue(WirePtr w) {
			
		if ( getAsignFlag(w->getWireId()) ){

			return valueAssignment[w->getWireId()];

		}else{

			BigInteger ret_val(0l) ;
			WireArray * bits = w->getBitWiresIfExistAlready();
			
			if (bits != NULL) {
				BigInteger sum = BigInteger::ZERO() ;
				for (size_t i = 0; i < bits->size(); i++) {
					sum.__add(valueAssignment[bits->get(i)->getWireId()].shiftLeft(i));
				}
				ret_val = sum;
			}
		
			return ret_val ;
		}
		
	}


	vector<BigInteger> CircuitEvaluator::getWiresValues(Wires & w) {
		vector<BigInteger> values ( w.size() ) ; 
		for (size_t i = 0; i < w.size() ; i++) {
			values[i] = getWireValue(w.get(i));
		}
		return values;
	}

	

	void CircuitEvaluator::evaluate() {

		LOGD("Running Circuit Evaluator for < %s > " , circuitGenerator->getName().c_str() );
		
		EvaluationQueue& evalSequence = circuitGenerator->getEvaluationQueue();

		for ( auto e : evalSequence){
			CircuitEvaluator & This = *this ;
			e->evaluate(This);
			e->emit(This);
		}

		 
		// check that each wire has been assigned a value
		wireID_t cc = (wireID_t) valueAssignment.size() ;
		for ( wireID_t i = 0; i < cc ; i++) {
			if ( ! getAsignFlag(i) ) {
				throw  runtime_error(string( "Wire# " + to_string(i) + "is without value" ));
			}
		}
		
		LOGD("Circuit Evaluation Done for < %s >\n\n" , circuitGenerator->getName().c_str() );

		return ;
	}



	void CircuitEvaluator::printInputs( std::ostream & printWriter , string endline ){

		EvaluationQueue& evalSequence = circuitGenerator->getEvaluationQueue();

		// printWriter << "# Input Wire Assignments " << endline ;
		
		for ( auto e : evalSequence){
			
			if ( e->instanceof_WireLabelInstruction()){ 
				
				WireLabelInstruction* wl = (WireLabelInstruction*) e ;
				
				if ( (wl->getType() == LabelType::input) || 
					 (wl->getType() == LabelType::nizkinput)) 
				{
					int id = wl->getWire()->getWireId();
					printWriter << id << " " << valueAssignment[id].toString(16) << endline  ;
				}
			}
		}
			
		return ;
	}

	const BigInteger & CircuitEvaluator::getAssignment(wireID_t wireID) const {
		return valueAssignment[wireID] ;
	}

	bool CircuitEvaluator::getAsignFlag( wireID_t wireID ) const {
		return valueAssignmentFlag[wireID] ;
	}

}

