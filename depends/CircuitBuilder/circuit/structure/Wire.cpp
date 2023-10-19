

#include <BigInteger.hpp>

#include <Wire.hpp>
#include <WireArray.hpp>
#include <VariableWire.hpp>
#include <ConstantWire.hpp>
#include <LinearCombinationWire.hpp>
#include <VariableBitWire.hpp>

#include <AddBasicOp.hpp>
#include <AssertBasicOp.hpp>
#include <ConstantSquareOp.hpp>
#include <ConstMulBasicOp.hpp>
#include <MulBasicOp.hpp>
#include <NonZeroCheckBasicOp.hpp>
#include <ORBasicOp.hpp>
#include <PackBasicOp.hpp>
#include <SplitBasicOp.hpp>
#include <SquareAndMultiplyOp.hpp>
#include <SquareOp.hpp>
#include <XorBasicOp.hpp>

#include <CircuitGenerator.hpp>
#include <Exceptions.hpp>


#include <logging.hpp>

namespace CircuitBuilder { 


	Wire::Wire(CircuitGenerator * __generator , int __wireId) 
		: Object(),
		  wireId(__wireId) , 
		  generator(__generator)
	{	
		class_id = class_id | Object::Wire_Mask ;
		bitWires = NULL ;
		  
		if (wireId < 0) {
			throw invalid_argument("wire id cannot be negative");
		}
	}


	Wire::Wire(CircuitGenerator * __generator , WireArray * bits , int __wireId) 
		: Object(),
		  wireId(__wireId) , 
		  generator(__generator)
	{	
		class_id = class_id | Object::Wire_Mask ;
		setBits(bits) ;
	}
	

	string Wire::toString() const {
		return std::to_string(wireId)  ;
	}

	wireID_t Wire::getWireId() const {
		return wireId;
	}

	WireArray * Wire::getBitWires() {
		return NULL ;
	}

	WireArray * Wire::getBitWires(size_t bitwidth, const string & desc ) {
		WireArray * bitWires = getBitWires();
		if (bitWires == NULL) {
			bitWires = forceSplit(bitwidth, desc);
			setBits(bitWires);
			return bitWires;
		} else {
			return bitWires->adjustLength(bitwidth);
		}
	}
		
	WireArray *Wire::getBitWiresIfExistAlready(){
		return getBitWires();
	}


	void Wire::setBits(WireArray * bits) {
		UNUSEDPARAM(bits)
		bitWires = NULL ;
		// LOGD("Warning --  you are trying to set bits for either a constant or a bit wire. -- Action Ignored\n");
	}

		 

	WirePtr Wire::power( const WirePtr exp, const string & desc) {
		
		packIfNeeded(desc);
		
		exp->packIfNeeded(desc);
		
		WirePtr out = allocate<LinearCombinationWire>(generator , generator->currentWireId++);
		BasicOp* op = allocate<SquareOp>(generator, this, exp, out , desc);
		
		Wires * cachedOutputs = generator->addToEvaluationQueue(op);
		if(cachedOutputs == NULL){
			return out;
		}
		else{
			generator->currentWireId--;
			generator->deallocate(op);
			return cachedOutputs->get(0);
		}
	}


	WirePtr Wire::mul(long l, const string & desc) {
		BigInteger bI = BigInteger(l) ;
		return mul( bI , desc);
	}
	
	WirePtr Wire::mul(long base, int exp, const string & desc) {
		BigInteger b = BigInteger(base ) ;
		b.__pow(exp);
		return mul( b , desc);
	}

	WirePtr Wire::mul(const BigInteger &b, const string & desc) {
		
		packIfNeeded(desc);
		
		if (b.equals(BigInteger::ONE())){
			return this;
		}
		
		if (b.equals(BigInteger::ZERO())){
			return generator->zeroWire;
		}
		
		WirePtr out = allocate<LinearCombinationWire>(generator , generator->currentWireId++);
		BasicOp* op = allocate<ConstMulBasicOp>(generator, this, out, b , desc);

		Wires * cachedOutputs = generator->addToEvaluationQueue(op);
		if(cachedOutputs == NULL){
			return out;
		}else{
			generator->currentWireId--;
			generator->deallocate(op );
			return cachedOutputs->get(0);
		}
	}

	WirePtr Wire::mul(const WirePtr w, const string & desc) {
		if ( w->instanceof_ConstantWire() ) {
			ConstantWirePtr cw = (ConstantWirePtr) w ;
			BigInteger bI = BigInteger(cw->getConstant()) ; 
			return mul( bI  , desc);
		} else {
			packIfNeeded(desc);
			w->packIfNeeded(desc);
			WirePtr output = allocate<VariableWire>(generator , generator->currentWireId++);
			BasicOp* op = allocate<MulBasicOp>(generator, this , w, output, desc );
			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return output;
			}else{
				generator->currentWireId--;
				generator->deallocate(op ) ;
				return cachedOutputs->get(0);
			}
		}
	}



	WirePtr Wire::add(long v, const string & desc) {
		return add( generator->createConstantWire(v, desc) , desc);
	}

	WirePtr Wire::add(const BigInteger &b, const string & desc) {
		return add( generator->createConstantWire( b , desc) , desc);
	}

	WirePtr Wire::add( const WirePtr w, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		
		if(w->wireId < 0){
			LOGD ("w wireId: %d\n" , w->wireId);
		}

		if(wireId < 0){
			LOGD ("wireId: %d\n" , wireId);
		}

		WireArray * w_array = allocate<WireArray>( generator , this , w );
		return w_array->sumAllElements(desc); 
	}

	

	WirePtr Wire::sub(long v, const string & desc) {
		return sub( generator->createConstantWire(v, desc) , desc);
	}

	WirePtr Wire::sub(const BigInteger &b, const string & desc) {
		return sub( generator->createConstantWire(b, desc) , desc);
	}

	WirePtr Wire::sub( const WirePtr w, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		WirePtr neg = w->mul(-1l, desc); 
		return add(neg, desc);
	}

	

	WirePtr Wire::checkNonZero(const string & desc) {
		packIfNeeded(desc);
		/**
		 * this wire is not currently used for anything - It's for compatibility
		 * with earlier experimental versions when the target was Pinocchio
		 **/
		
		WirePtr out1 = allocate<Wire>(generator , generator->currentWireId++);
		WirePtr out2 = allocate<VariableBitWire>(generator , generator->currentWireId++);

		BasicOp* op = allocate<NonZeroCheckBasicOp>(generator, this , out1, out2, desc );

		Wires * cachedOutputs = generator->addToEvaluationQueue(op);
		if(cachedOutputs == NULL ){
			return out2;
		}
		else{
			generator->currentWireId-=2;
			generator->deallocate(op );
			return cachedOutputs->get(1);
		}		
	}


	WirePtr Wire::invAsBit(const string & desc) {
		packIfNeeded(desc); // just a precaution .. should not be really needed
		WirePtr w1 = mul(-1l, desc);
		WirePtr out = generator->oneWire->add(w1, desc);
		return out;
	}

	

	WirePtr Wire::OR( const WirePtr w, const string & desc) {
		if (w->instanceof_ConstantWire()) {
			return w->OR(this, desc);
		} else {
			packIfNeeded(desc); 
			WirePtr out = allocate<VariableWire>(generator , generator->currentWireId++);
			BasicOp* op = allocate<ORBasicOp>(generator , this, w, out, desc);
			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return out;
			}
			else{
				generator->currentWireId--;
				generator->deallocate( op ) ;
				return cachedOutputs->get(0);
			}
		}
	}

	
	/**
	 * Assumes a binary wire
	 * 
	 * @param w
	 * @param desc
	 * @return
	 */
	WirePtr Wire::XOR( const WirePtr w, const string & desc) {
		if (w->instanceof_ConstantWire()) {
			return w->XOR(this, desc);
		} else {
			packIfNeeded(desc); 
			WirePtr out = allocate<VariableWire>(generator, generator->currentWireId++);
			BasicOp* op = allocate<XorBasicOp>(generator , this, w, out, desc);
			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return out;
			}
			else{
				generator->currentWireId--;
				generator->deallocate( op ) ;
				return cachedOutputs->get(0);
			}
		}
	}

	
	
	WirePtr Wire::AND( const WirePtr w, const string & desc) {
		return mul(w, desc);
	}



	WireArray * Wire::forceSplit(size_t bitwidth, const string & desc ) {

		Wires ws(bitwidth) ;
		
		for (size_t i = 0; i < bitwidth; i++) {
			ws.set(i , allocate<VariableBitWire>(generator , generator->currentWireId++) ) ;
		}
		
		BasicOp* op = allocate<SplitBasicOp>(generator , this, ws , desc);
		Wires * cachedOutputs = generator->addToEvaluationQueue(op);
		
		if(cachedOutputs == NULL ){
			return allocate<WireArray>(generator , ws ) ;
		}
		else{
			generator->currentWireId-=bitwidth;
			generator->deallocate( op ) ;
			return allocate<WireArray>(generator , *cachedOutputs)->adjustLength(bitwidth) ;
		}		

	}

	void Wire::restrictBitLength(size_t bitWidth, const string & desc) {
		WireArray * bitWires = getBitWires();
		if (bitWires == NULL) {
			getBitWires(bitWidth, desc);
		} else {
			if (bitWires->size() > bitWidth) {
				bitWires = forceSplit(bitWidth, desc);
				setBits(bitWires);
			} else {
				// nothing to be done.
			}
		}
	}



	WirePtr Wire::xorBitwise(long v, size_t numBits, const string & desc) {
		return xorBitwise( generator->createConstantWire(v, desc), numBits, desc);
	}

	WirePtr Wire::xorBitwise( const BigInteger &b, size_t numBits, const string & desc) {
		return xorBitwise(generator->createConstantWire(b, desc), numBits, desc);
	}

	WirePtr Wire::xorBitwise( const WirePtr w, size_t numBits, const string & desc) {
		WireArray * bits1 = getBitWires(numBits, desc);
		WireArray * bits2 = w->getBitWires(numBits, desc);
		WireArray * result = bits1->xorWireArray(bits2, numBits, desc);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}

	
	
	WirePtr Wire::andBitwise(long v, size_t numBits, const string & desc) {
		return andBitwise(generator->createConstantWire(v, desc), numBits, desc);
	}

	WirePtr Wire::andBitwise( const BigInteger &b, size_t numBits, const string & desc) {
		return andBitwise(generator->createConstantWire(b, desc), numBits, desc);
	}

	WirePtr Wire::andBitwise( const WirePtr w, size_t numBits, const string & desc) {
		WireArray * bits1 = getBitWires(numBits, desc);
		WireArray * bits2 = w->getBitWires(numBits, desc);
		WireArray * result = bits1->andWireArray(bits2, numBits, desc);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}

	

	
	WirePtr Wire::orBitwise(long v, size_t numBits, const string & desc) {
		return orBitwise( generator->createConstantWire(v, desc), numBits, desc);
	}

	WirePtr Wire::orBitwise( const BigInteger &b, size_t numBits, const string & desc) {
		return orBitwise( generator->createConstantWire(b, desc), numBits, desc);
	}

	WirePtr Wire::orBitwise( const WirePtr w, size_t numBits, const string & desc) {
		WireArray * bits1 = getBitWires(numBits, desc);
		WireArray * bits2 = w->getBitWires(numBits, desc);
		WireArray * result = bits1->orWireArray(bits2, numBits, desc);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}

	

	WirePtr Wire::isEqualTo( const BigInteger &b, const string & desc) {
		return isEqualTo( generator->createConstantWire(b, desc));
	}

	WirePtr Wire::isEqualTo(long v, const string & desc) {
		return isEqualTo( generator->createConstantWire(v, desc));
	}

	WirePtr Wire::isEqualTo( const WirePtr w, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		WirePtr s = sub(w, desc);
		return s->checkNonZero(desc)->invAsBit(desc);
	}



	WirePtr Wire::isLessThanOrEqual(long v, size_t bitwidth, const string & desc) {
		return isLessThanOrEqual( generator->createConstantWire(v, desc), bitwidth, desc);
	}

	WirePtr Wire::isLessThanOrEqual( const BigInteger &b, size_t bitwidth, const string & desc) {
		return isLessThanOrEqual( generator->createConstantWire(b, desc), bitwidth, desc);
	}

	WirePtr Wire::isLessThanOrEqual( const WirePtr w, size_t bitwidth, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		BigInteger p = BigInteger(2l);
		p.__pow(bitwidth);
		WirePtr pWire = generator->createConstantWire(p, desc);
		WirePtr sum = pWire->AND(w, desc)->sub(this, desc);
		WireArray * bitWires = sum->getBitWires(bitwidth + 1, desc);
		return bitWires->get(bitwidth);
	}

	

	WirePtr Wire::isLessThan(long v, size_t bitwidth, const string & desc) {
		return isLessThan( generator->createConstantWire(v, desc) , bitwidth, desc);
	}

	WirePtr Wire::isLessThan( const BigInteger &b, size_t bitwidth, const string & desc) {
		return isLessThan( generator->createConstantWire(b, desc), bitwidth, desc);
	}

	WirePtr Wire::isLessThan( const WirePtr w, size_t bitwidth, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		BigInteger p = BigInteger(2l);
		p.__pow(bitwidth);
		WirePtr pWire = generator->createConstantWire(p, desc);
		WirePtr sum = pWire->AND(this, desc)->sub(w, desc);
		WireArray * bitWires = sum->getBitWires(bitwidth + 1, desc);
		return bitWires->get(bitwidth)->invAsBit(desc);
	}

	

	WirePtr Wire::isGreaterThanOrEqual(long v, size_t bitwidth, const string & desc) {
		return isGreaterThanOrEqual( generator->createConstantWire(v, desc), bitwidth, desc);
	}

	WirePtr Wire::isGreaterThanOrEqual( const BigInteger &b, size_t bitwidth, const string & desc) {
		return isGreaterThanOrEqual( generator->createConstantWire(b, desc), bitwidth, desc);
	}

	WirePtr Wire::isGreaterThanOrEqual( const WirePtr w, size_t bitwidth, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		BigInteger p = BigInteger(2l);
		p.__pow(bitwidth);
		WirePtr pWire = generator->createConstantWire(p, desc);
		WirePtr sum = pWire->add(this, desc)->sub(w, desc);
		WireArray * bitWires = sum->getBitWires(bitwidth + 1, desc);
		return bitWires->get(bitwidth);
	}

	
	WirePtr Wire::isGreaterThan(long v, size_t bitwidth, const string & desc) {
		return isGreaterThan( generator->createConstantWire(v, desc), bitwidth, desc);
	}

	WirePtr Wire::isGreaterThan( const BigInteger &b, size_t bitwidth, const string & desc) {
		return isGreaterThan( generator->createConstantWire(b, desc), bitwidth, desc);
	}

	WirePtr Wire::isGreaterThan( const WirePtr w, size_t bitwidth, const string & desc) {
		packIfNeeded(desc);
		w->packIfNeeded(desc);
		BigInteger p = BigInteger("2");
		p.__pow(bitwidth);
		WirePtr pWire = generator->createConstantWire(p, desc);
		WirePtr sum = pWire->AND(w, desc)->sub(this, desc);
		WireArray * bitWires = sum->getBitWires(bitwidth + 1, desc);
		return bitWires->get(bitwidth)->invAsBit(desc);
	}

	

	WirePtr Wire::rotateLeft(size_t numBits, size_t s, const string & desc) {
		WireArray * bits = getBitWires(numBits, desc);
		Wires rotatedBits(numBits);
		for (size_t i = 0; i < numBits; i++) {
			if (i < s)
				rotatedBits[i] = bits->get(i + (numBits - s));
			else
				rotatedBits[i] = bits->get(i - s);
		}
		WireArray * result = allocate<WireArray>(generator , rotatedBits);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}


	WirePtr Wire::rotateRight(size_t numBits, size_t s, const string & desc) {
		WireArray * bits = getBitWires(numBits, desc);
		Wires rotatedBits(numBits);
		for (size_t i = 0; i < numBits; i++) {
			if (i >= numBits - s)
				rotatedBits[i] = bits->get(i - (numBits - s));
			else
				rotatedBits[i] = bits->get(i + s);
		}
		WireArray * result = allocate<WireArray>( generator, rotatedBits);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}


	WirePtr Wire::shiftLeft(size_t numBits, size_t s, const string & desc) {
		WireArray * bits = getBitWires(numBits, desc);
		Wires shiftedBits(numBits);
		for (size_t i = 0; i < numBits; i++) {
			if (i < s)
				shiftedBits[i] = generator->zeroWire;
			else
				shiftedBits[i] = bits->get(i - s);
		}
		WireArray * result = allocate<WireArray>(generator, shiftedBits);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}

	WirePtr Wire::shiftRight(size_t numBits, size_t s, const string & desc) {
		WireArray * bits = getBitWires(numBits, desc);
		Wires shiftedBits(numBits);
		for (size_t i = 0; i < numBits; i++) {
			if (i >= numBits - s)
				shiftedBits[i] = generator->zeroWire;
			else
				shiftedBits[i] = bits->get(i + s);
		}
		WireArray * result = allocate<WireArray>(generator , shiftedBits);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}


	WirePtr Wire::invBits(size_t bitwidth, const string & desc) {
		Wires & bits = getBitWires(bitwidth, desc)->asArray();
		Wires resultBits(bits.size());
		for (size_t i = 0; i < resultBits.size(); i++) {
			resultBits[i] = bits[i]->invAsBit(desc);
		}
		return allocate<LinearCombinationWire>( generator, allocate<WireArray>( generator , resultBits));
	}


	WirePtr Wire::trimBits(size_t currentNumOfBits, size_t desiredNumofBits, const string & desc) {
		WireArray * bitWires = getBitWires(currentNumOfBits, desc);
		WireArray * result = bitWires->adjustLength(desiredNumofBits);
		auto check = result->checkIfConstantBits(desc);
		if ( ! check.check ) {
			return allocate<LinearCombinationWire>(generator , result );
		} else {
			return generator->createConstantWire( check.BInt );
		}
	}


	void Wire::packIfNeeded(const string & desc) const {
		UNUSEDPARAM(desc)
		if (wireId == -1) {
			WirePtr w = (WirePtr)this;
			w->pack();
		}
	}


	void Wire::pack(const string & desc) {
		if (wireId == -1) {
			WireArray * bits = getBitWires();
			if (bits == NULL) {
				throw runtime_error("A Pack operation is tried on a wire that has no bits.");
			}
			wireId = generator->currentWireId++;
			
			BasicOp* op = allocate<PackBasicOp>(generator, bits->asArray() , this,  desc);
			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			
			if(cachedOutputs != NULL){
				generator->currentWireId--;
				wireId = cachedOutputs->get(0)->getWireId();
			}		

		}
	}
		
	
	hashCode_t Wire::hashCode() const {
		return wireId;
	}


	bool Wire::equals( const Object * obj) const {
		
		if(this == obj){
		
			return true;
		
		}else if( ! ( obj->instanceof_Wire() )){
		
			return false;
		
		}else{
			WirePtr w = (WirePtr)obj;
			return (w->wireId == wireId && w->generator == generator);
		}
	}

}
