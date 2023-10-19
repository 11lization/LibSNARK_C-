

#pragma once 

#include <global.hpp>

namespace CircuitBuilder {


    class Object {

        protected: uint32_t class_id ;
        
        #define AddClassType(ClassName,IdLsh) \
            public : static const uint32_t ClassName ## _Mask  = (0x00000001 << IdLsh) ; \
            public : bool instanceof_ ## ClassName () const  { return (class_id & (0x00000001 << IdLsh)) ; }


        AddClassType(Instruction,2) 
        AddClassType(BasicOp,3) 
        AddClassType(AddBasicOp,4)
        AddClassType(AssertBasicOp,5)
        AddClassType(ConstantSquareOp,6)
        AddClassType(ConstMulBasicOp,7)
        AddClassType(MulBasicOp,8)
        AddClassType(NonZeroCheckBasicOp,9)
        AddClassType(ORBasicOp,10)
        AddClassType(PackBasicOp,11)
        AddClassType(SplitBasicOp,12)
        AddClassType(SquareAndMultiplyOp,13)
        AddClassType(SquareOp,14)
        AddClassType(XorBasicOp,15)

        AddClassType(Wire,20) 
        AddClassType(BitWire,21)
        AddClassType(ConstantWire,22) 
        AddClassType(VariableWire,23) 
        AddClassType(VariableBitWire,24)
        AddClassType(LinearCombinationWire,25)
        AddClassType(LinearCombinationBitWire,26)
        
        AddClassType(WireLabelInstruction,27)
        AddClassType(WireArray,28)
        AddClassType(Gadget,29)

    public :
        
        Object() : class_id(0x00000000) {}
        
        virtual ~Object(){}

        virtual string toString() const {
            return "" ;
        }

        uint32_t classMask(){ return class_id ; }

        virtual hashCode_t hashCode()  const  {
            hashCode_t h = (hashCode_t) this ;
            return h ;
        }
    
    };


    void add_allocation( CircuitGenerator*  , Object* );

    template<class T >
    T* allocate ( CircuitGenerator* generator ){
        T* new_allocation = new T(generator );
        add_allocation( generator , new_allocation );
        return new_allocation ;
    }

    template<class T , typename Arg1 >
    T* allocate ( CircuitGenerator * generator , Arg1 arg1  ){
        T* new_allocation = new T(generator , arg1 );
        add_allocation( generator , new_allocation );
        return new_allocation ;
    }

    template<class T , typename Arg1 , typename Arg2 >
    T* allocate ( CircuitGenerator * generator , Arg1 arg1 , Arg2 arg2 ){
        T* new_allocation = new T(generator , arg1 , arg2 );
        add_allocation( generator , new_allocation );
        return new_allocation ;
    }

    template<class T , typename Arg1 , typename Arg2 , typename Arg3 >
    T* allocate ( CircuitGenerator * generator , Arg1 arg1 , Arg2 arg2 , Arg3 arg3 ){
        T* new_allocation = new T(generator , arg1 , arg2 , arg3 );
        add_allocation( generator , new_allocation );
        return new_allocation ;
    }

    template<class T , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    T* allocate ( CircuitGenerator * generator , Arg1 arg1 , Arg2 arg2 , Arg3 arg3 , Arg4 arg4  ){
        T* new_allocation = new T(generator , arg1 , arg2 , arg3 , arg4 );
        add_allocation( generator , new_allocation );
        return new_allocation ;
    }

    template<class T ,  
             typename Arg1 , typename Arg2 , 
             typename Arg3 , typename Arg4 , 
             typename Arg5 >
    T* allocate ( CircuitGenerator * generator , 
                  Arg1 arg1 , Arg2 arg2 , 
                  Arg3 arg3 , Arg4 arg4 , 
                  Arg5 arg5 )
    {
        T* new_allocation = new T(generator , arg1 , arg2 , arg3 , arg4 , arg5 );
        add_allocation( generator , new_allocation );
        return new_allocation ;
    }

}




