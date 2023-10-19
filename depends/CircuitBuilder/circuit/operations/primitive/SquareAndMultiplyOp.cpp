
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <SquareAndMultiplyOp.hpp> 

#include <logging.hpp>
 

namespace CircuitBuilder { 

    SquareAndMultiplyOp::SquareAndMultiplyOp(CircuitGenerator *generator , 
                                             WirePtr w1, WirePtr w2, 
                                             WirePtr out ,
                                             const string & desc ) 
        :   BasicOp( generator , 
                    SQUARE_MUL_OPCODE,
                    w1 , w2,
                    out ,
                    desc )
    {
        class_id = class_id | Object::SquareAndMultiplyOp_Mask ;
    }


    string SquareAndMultiplyOp::getOpcode() const {
        return "sm";
    }


    void SquareAndMultiplyOp::compute(CircuitEvaluator & evaluator) {
        BigInteger result = evaluator.getAssignment (inputs[0]->getWireId()).modPow( evaluator.getAssignment(inputs[1]->getWireId()), generator->config.FIELD_PRIME);
        evaluator.setWireValue(outputs[0] , result);
    }
    

    bool SquareAndMultiplyOp::equals(Object *obj) const {
        //TODO
        if (this == obj){
            return true;
        }
        if (!(obj->instanceof_SquareAndMultiplyOp())){
            return false;
        }

        SquareAndMultiplyOp &op = *( (SquareAndMultiplyOp*) obj);

        bool check1 = inputs[0]->equals(op.inputs[0])
                        && inputs[1]->equals(op.inputs[1]);
        bool check2 = inputs[1]->equals(op.inputs[0])
                        && inputs[0]->equals(op.inputs[1]);
        return check1 || check2;
    }
    

    int SquareAndMultiplyOp::getNumMulGates() const {
        // TODO Auto-generated method stub
        return 1;
    }
    
}