

#pragma once

#include <global.hpp>
#include <BaseClass.hpp>

namespace CircuitBuilder { 
	
	class CircuitEvaluator {

	private : 
	
		CircuitGenerator *circuitGenerator;
		vector<BigInteger> valueAssignment;
		vector<bool> valueAssignmentFlag;

		friend class CircuitGenerator ;
	 		
	public : 

		CircuitEvaluator(CircuitGenerator *circuitGenerator);  
		~CircuitEvaluator(){ valueAssignment.clear() ; }

		void setWireValue(WirePtr w, const BigInteger &v) ;
		void setWireValue(WirePtr w, long v) ;
		void setWireValue(Wire &w, const BigInteger &v) ;
		void setWireValue(Wire & w, long v) ;

		void setWireValue(vector<WirePtr> &wires , vector<BigInteger> &v) ;
		void setWireValue(Wires &wires , vector<BigInteger> &v) ;

		void flagAssigned();
		void flagAssigned(wireID_t wireID);
	 
	 	size_t size(){ return valueAssignment.size() ; }

		BigInteger getWireValue(WirePtr w) ;

		vector<BigInteger> getWiresValues(Wires & w) ;

		void evaluate() ;

		void printInputs( std::ostream & printWriter , string endline = "\n") ;

		const BigInteger & getAssignment( wireID_t wireID ) const ;

		bool getAsignFlag( wireID_t wireID ) const ;

		void clear(){ valueAssignment.clear() ; }

	};
}
