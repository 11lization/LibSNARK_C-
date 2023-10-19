 

#pragma once

#include <global.hpp> 
#include <utilities.hpp>
#include <BasicOp.hpp> 


namespace CircuitBuilder { 

    class ConstantSquareOp : public BasicOp {
    
    private :
        
        bool inSign;
        BigInteger constInteger;
        
        BigInteger assign_const(CircuitGenerator *generator , BigIntegerPtr __constInteger );

    public :

        ConstantSquareOp(CircuitGenerator *generator , 
                        WirePtr w ,
                        BigIntegerPtr __constInteger, 
                        WirePtr out ,
                        const string & desc = "" ) ;

        string getOpcode() const ;

        void compute(CircuitEvaluator & evaluator) ;
    
    	bool equals(Object *obj) const ;
        
        int getNumMulGates() const ;

    };

}