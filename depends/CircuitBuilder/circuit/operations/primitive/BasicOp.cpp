
#include <CircuitEvaluator.hpp>
#include <CircuitGenerator.hpp>
#include <BasicOp.hpp>

#include <logging.hpp>

#include <Exceptions.hpp>

namespace CircuitBuilder {

    BasicOp::BasicOp( CircuitGenerator *generator , 
                      OpCode_t __OpCode, 
                      WirePtr input1 , 
                      WirePtr output1 , 
                      const string & desc)
        : Instruction(generator) , OpCode(__OpCode), 
          inputs(input1) , 
          outputs(output1),
          desc_id(generator->set_desc(desc))
    {
        class_id = class_id | Object::BasicOp_Mask ;
        check();
    }


    BasicOp::BasicOp( CircuitGenerator *generator , 
                      OpCode_t __OpCode, 
                      WirePtr input1 , WirePtr input2 , 
                      WirePtr output1 , 
                      const string &desc)
        : Instruction(generator) , 
          OpCode(__OpCode), 
          inputs(input1 , input2) , 
          outputs(output1),
          desc_id(generator->set_desc(desc))
    {
        class_id = class_id | Object::BasicOp_Mask ;
        check();
    }


    BasicOp::BasicOp( CircuitGenerator *generator , 
                      OpCode_t __OpCode, 
                      const Wires &__inputs, 
                      WirePtr output1 , 
                      const string & desc)
        : Instruction(generator) , 
          OpCode(__OpCode), 
          inputs(__inputs) , 
          outputs(output1), 
          desc_id(generator->set_desc(desc))
    {
        class_id = class_id | Object::BasicOp_Mask ;
        check();
    }

    
    BasicOp::BasicOp( CircuitGenerator *generator , 
                      OpCode_t __OpCode, 
                      const Wires &__inputs, 
                      const Wires &__outputs , 
                      const string & desc)
        : Instruction(generator) , 
          OpCode(__OpCode), 
          inputs(__inputs) , 
          outputs(__outputs), 
          desc_id(generator->set_desc(desc))
    {
        class_id = class_id | Object::BasicOp_Mask ;
        check();
    }

    
    void BasicOp::check(){

        for (WirePtr w : inputs.asArray() ) {
            if (w == nullptr) {
                LOGD("One of the input wires is null: %p \n" , this);
                throw NullPointerException("A null wire");
            } else if (w->getWireId() == -1) {
                LOGD("One of the input wires is not packed: %p \n" , this);
                throw invalid_argument("A wire with a negative id");
            }
        }

        for (WirePtr w : outputs.asArray() ) {
            if (w == nullptr) {
                throw NullPointerException("A null wire");
            }
        }
        
        return;
    }

    OpCode_t BasicOp::op_code(){ return OpCode ; }

    void BasicOp::evaluate(CircuitEvaluator &evaluator) {
        checkInputs(evaluator);
        compute(evaluator);
        checkOutputs(evaluator);
    }


    void BasicOp::checkInputs(CircuitEvaluator & evaluator) {
        for (WirePtr w : inputs ) {   
            if ( ! evaluator.getAsignFlag( w->getWireId()) ) {
                LOGD("Error - The inWire [%d] has not been assigned. Op : [ %s ] \n"  , w->getWireId() , toString().c_str() );
                throw runtime_error("Error During Evaluation");
            }
        }
    }


    void BasicOp::checkOutputs(CircuitEvaluator & evaluator) {
        for (WirePtr w : outputs ) {            
            if ( ! evaluator.getAsignFlag( w->getWireId()) ) {
                LOGD("Error - The outWire [%d] has not been assigned. Op : [ %s ] \n"  , w->getWireId() , toString().c_str() );
                throw runtime_error("Error During Evaluation");
            }
        }
    }


    void BasicOp::dump(std::ostream &out) const {
        
        string desc = generator->get_desc(desc_id);

        out << (getOpcode()) 
            << " in " << inputs.size() << " <" << inputs << ">" 
            << " out " << outputs.size() << " <" << outputs << ">" ;
        
        if ( desc.size() > 0) {
            out << " \t\t# " << desc ;
        }

        return ;
    }


    std::ostream& operator<<(std::ostream &out, const BasicOp & op ){
        op.dump(out);
        return out ;
    }


    string BasicOp::toString() const {
        stringstream rtn ;
        dump(rtn);  
        return rtn.str() ;
    }


    const Wires & BasicOp::getInputs() const {
        return inputs;
    }

    const Wires & BasicOp::getOutputs() const {
        return outputs;
    }


    bool BasicOp::doneWithinCircuit() {
        return true;
    }
    
    
    hashCode_t BasicOp::hashCode() const {

        hashCode_t h ;
        h = std::hash<std::string>()(getOpcode());
        
        for(WirePtr in : inputs.asArray() ){
            h += (hashCode_t)in->hashCode();
        }
        
        return h;
    }
    
    
    bool BasicOp::equals(Object* obj) const {
        return (this == obj) ;
        // logic moved to subclasses
    }

}
