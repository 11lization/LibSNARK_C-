
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <SquareOp.hpp> 

#include <logging.hpp>


namespace CircuitBuilder { 

        
    SquareOp::SquareOp( CircuitGenerator *generator , 
                        WirePtr w1, WirePtr w2, 
                        WirePtr out, 
                        const string & desc ) 
        :   BasicOp( generator , 
                        SQUARE_OPCODE,
                        w1 , w2 ,
                        out , 
                        desc )
    {
        class_id = class_id | Object::SquareOp_Mask ;
    }

    string SquareOp::getOpcode() const {
        return "square";
    }

    void SquareOp::compute(CircuitEvaluator & evaluator) {
        BigInteger result = evaluator.getAssignment (inputs[0]->getWireId()).modPow( evaluator.getAssignment(inputs[1]->getWireId()), generator->config.FIELD_PRIME);
        evaluator.setWireValue(outputs[0] , result);
    }


    bool SquareOp::equals(Object *obj) const {
        //TODO
        if (this == obj){
            return true;
        }

        if (!(obj->instanceof_SquareOp())){
            return false;
        }
        
        SquareOp *op = (SquareOp*) obj ;

        bool check1 = inputs[0]->equals(op->inputs[0])
                        && inputs[1]->equals(op->inputs[1]);
        bool check2 = inputs[1]->equals(op->inputs[0])
                        && inputs[0]->equals(op->inputs[1]);
        
        return check1 || check2;
    }



    int SquareOp::getNumMulGates() const {
        // TODO Auto-generated method stub
        //w.getBitWires(32, desc);
        
        return 1;
    }
    

}
