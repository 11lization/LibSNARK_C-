

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <OpCode.hpp>
#include <BigInteger.hpp>

using namespace std ;

namespace CircuitBuilder {

	class Instruction ;
	
	class BasicOp ;
	class AddBasicOp ;
	class AssertBasicOp ;
	class ConstantSquareOp ;
	class ConstMulBasicOp ;
	class MulBasicOp ;
	class NonZeroCheckBasicOp ;
	class ORBasicOp ;
	class PackBasicOp ;
	class SplitBasicOp ;
	class SquareAndMultiplyOp ;
	class SquareOp ;
	class XorBasicOp ;
 
	class Wire ;
	class Wires ;
	class WireArray ;
	class BitWire ;
	class ConstantWire ;
	class LinearCombinationBitWire ;
	class LinearCombinationWire ;
	class VariableBitWire ;
	class VariableWire ;
	class WireLabelInstruction ;

 	class Gadget ;

	class CircuitGenerator ;
	class CircuitEvaluator ;
	class Config ;
	

	typedef Instruction* InstructionPtr ;
	typedef vector<InstructionPtr> Instructions ;

	
	typedef vector<BigInteger> Assignments ;

	typedef Wire* WirePtr ;
	typedef Wire* wire ;

	typedef Wires* WiresPtr ;
	
	typedef VariableWire * VariableWirePtr ;
	typedef ConstantWire * ConstantWirePtr ;
	typedef WireLabelInstruction * WireLabelInstructionPtr ;

	typedef std::string WireDesc ;

	typedef unsigned long hashCode_t ;
	typedef uint16_t OpCode_t ;
	typedef int32_t wireID_t ;
}

using namespace CircuitBuilder ;


#ifndef UNUSEDPARAM
#define UNUSEDPARAM(x) (void)(x);
#endif
