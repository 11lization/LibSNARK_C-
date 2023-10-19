 

#include <BitWire.hpp>
#include <WireArray.hpp> 
#include <ConstantWire.hpp> 
#include <LinearCombinationWire.hpp> 
#include <VariableBitWire.hpp> 
#include <VariableWire.hpp> 
#include <LinearCombinationBitWire.hpp> 

#include <XorBasicOp.hpp> 
#include <ConstMulBasicOp.hpp> 
#include <AddBasicOp.hpp> 
#include <ORBasicOp.hpp> 
#include <MulBasicOp.hpp>

#include <CircuitGenerator.hpp>


namespace CircuitBuilder { 
	
	BitWire::BitWire(CircuitGenerator * generator, int wireId) 
		: Wire( generator , wireId) 
	{
		class_id = class_id | Object::BitWire_Mask ;
	}


	WirePtr BitWire::mul( const WirePtr w, const string & desc) {
		if (w->instanceof_ConstantWire()) {
			ConstantWirePtr cw = (ConstantWirePtr)w ;
			return mul(cw->getConstant(), desc);
		} else {
			
			WirePtr output;
			
			if (w->instanceof_BitWire()){
				output = allocate<VariableBitWire>(generator , generator->currentWireId++);
			}else{
				output = allocate<VariableWire>(generator , generator->currentWireId++);
			}

			BasicOp* op = allocate<MulBasicOp>( generator , this, w, output, desc);
			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return output;
			}else{
				generator->currentWireId--;
				generator->deallocate( op );
				return cachedOutputs->get(0);
			}
		}
	}

	WirePtr BitWire::mul(const BigInteger &b, const string & desc) {
		WirePtr out;
		
		if(b.equals(BigInteger::ZERO())){
			return generator->zeroWire;
		} else if(b.equals(BigInteger::ONE())){
			return this;
		} else{
			out = allocate<LinearCombinationWire>(generator , generator->currentWireId++);
			BasicOp* op = allocate<ConstMulBasicOp>(generator, this, out, b, desc);

			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return out;
			}
			else{
				generator->currentWireId--;
				generator->deallocate( op );
				return cachedOutputs->get(0);
			}	
		}
	}


	WirePtr BitWire::invAsBit(const string & desc) {

		WirePtr neg = Wire::mul(-1l, desc);
		WirePtr out = allocate<LinearCombinationBitWire>(generator , generator->currentWireId++);
		Wires in ( generator->oneWire, neg ) ;
		BasicOp* op = allocate<AddBasicOp>( generator , in , out, desc);

		Wires * cachedOutputs = generator->addToEvaluationQueue(op);
		if(cachedOutputs == NULL){
			return out;
		}
		else{
			generator->currentWireId--;
			generator->deallocate( op );
			return cachedOutputs->get(0);
		}		
	}


	WirePtr BitWire::OR( const WirePtr w, const string & desc) {
		if (w->instanceof_ConstantWire()) {
			ConstantWirePtr cw = (ConstantWirePtr) w ;
			return cw->OR( this, desc);
		} else {
			WirePtr out;
			if (w->instanceof_BitWire()) {
				out = allocate<VariableBitWire>(generator , generator->currentWireId++);
				BasicOp* op = allocate<ORBasicOp>(generator , this, w, out, desc);
				Wires * cachedOutputs = generator->addToEvaluationQueue(op);
				if(cachedOutputs == NULL){
					return out;
				}
				else{
					generator->currentWireId--;
					generator->deallocate( op );
					return cachedOutputs->get(0);
				}
			} else {
				return Wire::OR(w, desc);
			}	
		}
	}
	
	
	WirePtr BitWire::XOR(const WirePtr w, const string & desc) {
		 if (w->instanceof_ConstantWire()) {
		 	ConstantWirePtr cw = (ConstantWirePtr) w ;
			return cw->XOR( this, desc);
		} else {
			WirePtr out;
			if (w->instanceof_BitWire()) {
				out = allocate<VariableBitWire>(generator , generator->currentWireId++);
				BasicOp* op = allocate<XorBasicOp>(generator , this, w, out, desc);
				Wires * cachedOutputs = generator->addToEvaluationQueue(op);
				if(cachedOutputs == NULL){
					return out;
				}
				else{
					generator->currentWireId--;
					generator->deallocate( op );
					return cachedOutputs->get(0);
				}
			} else {
				return Wire::XOR(w, desc);
			}	
		}
	}
	
	WireArray * BitWire::getBits( const WirePtr w, int bitwidth, const string & desc) {
		UNUSEDPARAM(w)
		UNUSEDPARAM(desc)
		WireArray * ws = allocate<WireArray>( generator , this ) ;
		return ws->adjustLength(bitwidth);
	}
		
	
}
