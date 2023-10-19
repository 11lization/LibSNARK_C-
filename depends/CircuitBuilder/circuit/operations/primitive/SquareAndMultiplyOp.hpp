
#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 


namespace CircuitBuilder { 

    class SquareAndMultiplyOp : public BasicOp {

    public:

        SquareAndMultiplyOp(CircuitGenerator *generator , 
                            WirePtr w1, WirePtr w2, 
                            WirePtr out ,
                            const string & desc = "" ) ;
                            
        string getOpcode() const ;

        void compute(CircuitEvaluator & evaluator) ;
        
    	bool equals(Object *obj) const ;
        
        int getNumMulGates() const ;

    };
}