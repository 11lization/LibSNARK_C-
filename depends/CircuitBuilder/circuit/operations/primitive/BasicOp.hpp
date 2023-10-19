

#pragma once

#include <global.hpp>
#include <BigInteger.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <WireArray.hpp>
#include <utilities.hpp>
#include <CircuitEvaluator.hpp>

namespace CircuitBuilder {


    class BasicOp : public Instruction {

    protected :
        OpCode_t OpCode ;
        Wires inputs;
        Wires outputs;
        uint32_t desc_id ;

    public:

        BasicOp( CircuitGenerator *generator , OpCode_t OpCode, 
                 WirePtr input1 , WirePtr output1 , 
                 const string & desc = "" );

        BasicOp( CircuitGenerator *generator , OpCode_t OpCode, 
                 WirePtr input1 , WirePtr input2 , WirePtr output1 , 
                 const string & desc = "" );

        BasicOp( CircuitGenerator *generator , OpCode_t OpCode, 
                 const Wires &inputs, WirePtr output1 , 
                 const string & desc = "");

        BasicOp( CircuitGenerator *generator , OpCode_t OpCode, 
                 const Wires &inputs, const Wires &outputs , 
                 const string & desc = "");
        
        virtual void checkInputs(CircuitEvaluator & evaluator);

        virtual void checkOutputs(CircuitEvaluator & evaluator) ;

        virtual string getOpcode() const = 0;

        OpCode_t op_code();
    
        virtual int getNumMulGates() const = 0;
    
        string toString() const ;
        
        void dump(std::ostream &out) const ;

        const Wires & getInputs() const ;

        const Wires & getOutputs() const ;

        bool doneWithinCircuit() ;
    
        hashCode_t hashCode() const ;
    
        virtual bool equals(Object *obj) const ;

        void check();

        void evaluate(CircuitEvaluator & evaluator) ;

        virtual void compute(CircuitEvaluator & evaluator) = 0;

    };
}
