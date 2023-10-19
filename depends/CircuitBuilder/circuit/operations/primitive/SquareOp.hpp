 
#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 


namespace CircuitBuilder { 

    class SquareOp : public BasicOp {

    public :
        
        SquareOp(CircuitGenerator *generator , 
                 WirePtr w1, WirePtr w2, 
                 WirePtr out, 
                 const string & desc = "" ) ;

        string getOpcode() const ;
    
        void compute(CircuitEvaluator & evaluator) ;

        bool equals(Object *obj) const ;

        int getNumMulGates() const ;
        
    };

}
