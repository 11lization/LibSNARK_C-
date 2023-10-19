
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <ConstantSquareOp.hpp> 

#include <logging.hpp>


namespace CircuitBuilder { 


    BigInteger ConstantSquareOp::assign_const(CircuitGenerator *generator , BigIntegerPtr __constInteger ){
    
        bool __inSign = __constInteger->signum() == -1;
        
        if (!__inSign) {
            BigInteger constbI = __constInteger->mod(generator->config.FIELD_PRIME);
            return constbI ;
        } else {
            BigInteger constbI = __constInteger->negate();
            constbI = constbI.mod(generator->config.FIELD_PRIME);
            return generator->config.FIELD_PRIME.subtract(constbI);
        }
    }


    ConstantSquareOp::ConstantSquareOp( CircuitGenerator *generator , 
                                        WirePtr w ,
                                        BigIntegerPtr __constInteger, 
                                        WirePtr out ,
                                        const string & desc) 
        :   BasicOp( generator , 
                    CONST_SQUARE_OPCODE ,
                    w ,
                    out ,
                    desc ) ,
            inSign(__constInteger->signum() == -1) ,
            constInteger(assign_const(generator , __constInteger ))
    {
        class_id = class_id | Object::ConstantSquareOp_Mask ; 
    }

    
    string ConstantSquareOp::getOpcode() const {
        if (!inSign) {
            return "const-square-" + constInteger.toString(16);
        } else{
            return "const-square-neg-" + generator->config.FIELD_PRIME.subtract(constInteger).toString(16);
        }
    }


    void ConstantSquareOp::compute(CircuitEvaluator & evaluator) {
        BigInteger result = evaluator.getAssignment(inputs[0]->getWireId()).modPow(constInteger, generator->config.FIELD_PRIME);
        result = result.mod(generator->config.FIELD_PRIME);
        evaluator.setWireValue ( outputs[0] , result );
    }


    bool ConstantSquareOp::equals(Object *obj) const {
        
        //TODO
        if (this == obj){
            return true;
        }

        if (!(obj->instanceof_ConstantSquareOp())){
            return false;
        }
        
        ConstantSquareOp *op = (ConstantSquareOp*) obj ;

        bool check1 = inputs[0]->equals(op->inputs[0])
                        && inputs[1]->equals(op->inputs[1]);
        bool check2 = inputs[1]->equals(op->inputs[0])
                        && inputs[0]->equals(op->inputs[1]);
        
        return check1 || check2;
    }
    

    int ConstantSquareOp::getNumMulGates() const {
        // TODO Auto-generated method stub
        return 1;
    }
    
}