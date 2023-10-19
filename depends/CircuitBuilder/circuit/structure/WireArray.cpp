

#include <Wire.hpp>
#include <LinearCombinationWire.hpp>
#include <ConstantWire.hpp>
#include <WireArray.hpp>
#include <utilities.hpp>
#include <AddBasicOp.hpp>
#include <PackBasicOp.hpp>

#include <logging.hpp>

namespace CircuitBuilder {

	
	Wires::Wires () : base_array( 0 , NULL ) {} 
	
	Wires::Wires (size_t count) : base_array(count , NULL ) {}
	
	Wires::Wires (WirePtr w , size_t count) : base_array(count , w ) {}

	Wires::Wires (WirePtr w1, WirePtr w2) : base_array( 0 , NULL ) {
		base_array.push_back(w1);
		base_array.push_back(w2);
	}

	Wires::Wires (const Wires & other) : base_array(other.base_array) {}

	Wires::Wires (const vector<WirePtr> & other ) : base_array(other) {}

	Wires::~Wires(){}
	
	Wires & Wires::operator=( const Wires &lhs ){
		base_array = lhs.base_array ;
		return *this;
	}

	WirePtr & Wires::operator[](size_t index) const { 
		assert( index < base_array.size() );
		return (WirePtr&)base_array[index];
	}
	

	WirePtr Wires::get(size_t index) const { 
		assert( index < base_array.size() );
		return (WirePtr&)base_array[index];
	}
	

	void Wires::set(size_t index , WirePtr w){ 
		assert( index < base_array.size() );
		base_array[index] = w ;
		return ;
	}


	size_t Wires::copyOfRange2(size_t from , size_t to , Wires & dst_wires ){

		size_t new_size = to - from ;
	
		dst_wires.base_array.clear();
		dst_wires.base_array.resize( new_size ) ;

		size_t src_i = from ;
		size_t dst_i = 0 ;
		const size_t cpy_count = min( base_array.size() , to ) ;
		for ( ; src_i < cpy_count ;	src_i++ , dst_i++ ){
			dst_wires.base_array[dst_i] = base_array[src_i] ;
		}

		return new_size ;
	}


	Wires Wires::copyOfRange(size_t from , size_t to ){
		Wires rtn ;
		copyOfRange2(from , to, rtn );
		return rtn ;
	}

	
	string Wires::toString(const char * separator ) const {
 		
		if ( base_array.size() == 1 ){
			
			return base_array[0]->toString() ;
		
		}else if ( base_array.size() > 1 ){
			
			string ret_val = "" ;

			const size_t cc = base_array.size() - 1 ;
			
			for( size_t i = 0 ; i < cc ; i++ ){
				ret_val.append(base_array[i]->toString()) ;
				ret_val.append(separator) ;
			}

			ret_val.append(base_array[ base_array.size() - 1 ]->toString()) ;

			return ret_val ;

		}else{
			return "" ;
		}

    }

    string Wires::toString(string separator ) const {
    	return toString(separator.c_str()) ;
    }

    string Wires::toString() const {
    	return toString(" ") ;
    }

    std::ostream& operator<<(std::ostream &out, const Wires  &ws){
	    out << ws.toString();
		return out ;
	}


	Wire_iterator Wires::begin(){ 
		return Wire_iterator( 0 ,  & base_array ); 	
	}
    
    Wire_iterator Wires::end(){ 
    	return Wire_iterator( base_array.size() ,  & base_array ); 	
    }
    
    const_Wire_iterator Wires::begin() const { 
    	return const_Wire_iterator( 0 , & base_array );
    }
    
    const_Wire_iterator Wires::end() const { 
    	return const_Wire_iterator( base_array.size() , & base_array );
    }

}





namespace CircuitBuilder {

	WireArray::WireArray(CircuitGenerator * __generator) 
		: Object(), base_array() , generator(__generator)
	{
		class_id = class_id | Object::WireArray_Mask ;
	}

	WireArray::WireArray(CircuitGenerator * __generator, size_t n) 
		: Object(), base_array(n) , generator(__generator)
	{
		class_id = class_id | Object::WireArray_Mask ;
	}


	WireArray::WireArray(CircuitGenerator * __generator, WirePtr w , size_t count) 
		: Object(), base_array(w , count ) , generator(__generator)
	{
		class_id = class_id | Object::WireArray_Mask ;
	}


	WireArray::WireArray(CircuitGenerator * __generator, WirePtr w1 , WirePtr w2 )
		: Object(), base_array(w1 , w2 ) , generator(__generator)
	{
		class_id = class_id | Object::WireArray_Mask ;
	}


	WireArray::WireArray(CircuitGenerator * __generator, Wires &ws ) 
		: Object(), base_array(ws) , generator(__generator)
	{
		class_id = class_id | Object::WireArray_Mask ;
	}
	

	WireArray::WireArray(CircuitGenerator * __generator, WireArray &wireArray) 
		: Object(), base_array(wireArray.base_array) , generator(__generator)
	{
		class_id = class_id | Object::WireArray_Mask ;
	}


	WireArray * WireArray::mulWireArray(WireArray * v, size_t desiredLength, const string & desc ) {
		WireVector new_ws1 , new_ws2 ;
		WireVector & ws1 = adjustLength2( base_array , new_ws1,  desiredLength);
		WireVector & ws2 = adjustLength2( v->base_array, new_ws2 , desiredLength);
		WireVector out(desiredLength);
		for (size_t i = 0; i < out.size() ; i++) {
			out[i] = ws1[i]->mul(ws2[i], desc);
		}
		return allocate<WireArray>(generator, out);
	}
	
	
	WirePtr WireArray::sumAllElements(const string & desc ) {
 		bool allConstant = true;
		WirePtr output;
		BigInteger sum = BigInteger::ZERO();
		
		for (WirePtr w : base_array) {
			if (!(w->instanceof_ConstantWire())) {
				allConstant = false;
				break;
			} else {
				sum.__add(((ConstantWirePtr) w)->getConstant());
			}
		}

		if (allConstant) {
			output = generator->createConstantWire(sum, desc);
		} else {
			output = allocate<LinearCombinationWire>(generator, generator->currentWireId++);
			BasicOp* op = allocate<AddBasicOp>(generator, base_array, output, desc);

			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return output;
			}else{
				generator->currentWireId--;
				generator->deallocate(op) ;
				return cachedOutputs->get(0);
			}	
		}
		return output;
	}
	
	
	WireArray * WireArray::addWireArray(WireArray * v, size_t desiredLength, const string & desc ) {
		WireVector new_ws1 , new_ws2 ;
		WireVector & ws1 = adjustLength2( base_array , new_ws1,  desiredLength);
		WireVector & ws2 = adjustLength2( v->base_array, new_ws2 , desiredLength);
		WireVector out(desiredLength);
		for (size_t i = 0; i < out.size(); i++) {
			out[i] = ws1[i]->add(ws2[i], desc);
		}
		return allocate<WireArray>( generator , out);
	}
	

	WireArray * WireArray::xorWireArray(WireArray * v, size_t desiredLength, const string & desc ) {
		WireVector new_ws1 , new_ws2 ;
		WireVector & ws1 = adjustLength2( base_array , new_ws1,  desiredLength);
		WireVector & ws2 = adjustLength2( v->base_array, new_ws2 , desiredLength);
		WireVector out(desiredLength);
		for (size_t i = 0; i < out.size() ; i++) {
			out[i] = ws1[i]->XOR( ws2[i], desc);
		}
		return allocate<WireArray>( generator , out );
	}
	

	WireArray * WireArray::xorWireArray(WireArray * v, const string & desc ) {
		if(size() != v->size()){
			throw invalid_argument(" WireArray::xorWireArray() [ size() != v->size()] ");
		}
		
		Wires &ws1 = base_array ;
		Wires &ws2 = v->base_array;
		
		Wires out(size());
		
		for (size_t i = 0; i < out.size(); i++) {
			out[i] = ws1[i]->XOR( ws2[i], desc);
		}
		
		return allocate<WireArray>(generator , out);
	}
	

	WireArray * WireArray::andWireArray(WireArray * v, size_t desiredLength, const string & desc ) {
		WireVector new_ws1 , new_ws2 ;
		WireVector & ws1 = adjustLength2( base_array , new_ws1,  desiredLength);
		WireVector & ws2 = adjustLength2( v->base_array, new_ws2 , desiredLength);
		WireVector out(desiredLength);

		for (size_t i = 0; i < out.size(); i++) {
			out[i] = ws1[i]->mul( ws2[i], desc);
		}
		return allocate<WireArray>( generator, out);
	}
	

	WireArray * WireArray::orWireArray(WireArray * v, size_t desiredLength, const string & desc ) {
		WireVector new_ws1 , new_ws2 ;
		WireVector & ws1 = adjustLength2( base_array , new_ws1,  desiredLength);
		WireVector & ws2 = adjustLength2( v->base_array, new_ws2 , desiredLength);
		WireVector out(desiredLength);

		for (size_t i = 0; i < out.size(); i++) {
			out[i] = ws1[i]->OR( ws2[i], desc);
		}
		return allocate<WireArray>( generator , out);
	}
	
	
	WireArray * WireArray::invAsBits(size_t desiredBitWidth, const string & desc ) {
		Wires out(desiredBitWidth);
		for(size_t i = 0; i < desiredBitWidth; i++){
			if(i < base_array.size()){
				out[i] = base_array[i]->invAsBit(desc);
			}
			else{
				out[i] = generator->oneWire;
			}
		}
		return allocate<WireArray>( generator , out);
	}	
		
	
	Wires & WireArray::adjustLength2 (Wires &original , Wires &new_list , size_t desiredLength) {
		
		if(original.size() == desiredLength){
			return original ;
		}

		original.copyOfRange2(0, desiredLength , new_list ) ;

		if (original.size() < desiredLength) {
			for (size_t i = original.size(); i < desiredLength; i++) {
				new_list[i] = generator->zeroWire ;
			}
		}
		
		return new_list ;
	}


	WireArray * WireArray::adjustLength(size_t desiredLength) {
		Wires __newWs ;
		Wires &newWs = adjustLength2( base_array , __newWs , desiredLength );
		return allocate<WireArray>( generator , newWs); 
	}


	checkResultTy WireArray::checkIfConstantBits(const string & desc ){
		
		bool allConstant = true;
		
		BigInteger sum = BigInteger::ZERO();

		for(size_t i = 0; i < base_array.size() ; i++){
			WirePtr w = base_array[i];
			
			if(w->instanceof_ConstantWire()){
				ConstantWirePtr cw = (ConstantWirePtr)w;
				BigInteger v = cw->getConstant();
				
				if(v.equals(BigInteger::ONE())){
					v.__shiftLeft(i) ;
					sum.__add(v);
				}
				else if (! v.equals(BigInteger::ZERO())){
				 	LOGD("Warning, one of the bit wires is constant but not binary : %s" , desc.c_str() );					
				}
				
			}
			else{
				allConstant = false;
			}
		}
		
		if(allConstant)
			return { .BInt = sum , .check = true } ;
		else
			return { .BInt = BigInteger::ZERO() , .check = false } ;
	}


	WirePtr WireArray::packAsBits(size_t from, size_t to, const string & desc ) {
		
		if (from > to || to > base_array.size()){
			throw invalid_argument("Invalid bounds: from > to");
		}
		
		Wires bits;
		base_array.copyOfRange2(from , to , bits );
		
		bool allConstant = true;
		BigInteger sum = BigInteger::ZERO();
		
		for(size_t i = 0; i < bits.size() ; i++){
			
			WirePtr w = bits[i];
			
			if(w->instanceof_ConstantWire()){
				ConstantWirePtr cw = (ConstantWirePtr)w;
				BigInteger v = cw->getConstant();
				
				if(v.equals(BigInteger::ONE())){
					v.__shiftLeft(i);
					sum.__add(v);
				}
				
				else if (!v.equals(BigInteger::ZERO())){
					throw runtime_error("Trying to pack non-binary constant bits : " + desc );					
				}
				
			}
			else{
				allConstant = false;
			}
		}

		if(!allConstant){
			WirePtr out = allocate<LinearCombinationWire>(generator, generator->currentWireId++);
			out->setBits(allocate<WireArray>(generator , bits));
			BasicOp* op = allocate<PackBasicOp>( generator , bits , out, desc);
			Wires * cachedOutputs = generator->addToEvaluationQueue(op);
			if(cachedOutputs == NULL){
				return out;
			}else{
				generator->currentWireId--;
				generator->deallocate( op ) ;
				return cachedOutputs->get(0);
			}
		} else{
			return generator->createConstantWire( sum, desc);

		}
	}
	

	WireArray * WireArray::rotateLeft(size_t numBits, size_t s) {
		WireVector new_ws1 ;
		WireVector & bits = adjustLength2( base_array , new_ws1,  numBits);
		WireVector rotatedBits(numBits);
		for (size_t i = 0; i < numBits; i++) {
			if (i < s)
				rotatedBits[i] = bits[i + (numBits - s)];
			else
				rotatedBits[i] = bits[i - s];
		}
		return allocate<WireArray>(generator , rotatedBits);
	}
	

	WireArray * WireArray::rotateRight(size_t numBits, size_t s) {
		WireVector new_ws1 ;
		WireVector & bits = adjustLength2( base_array , new_ws1,  numBits);
		WireVector rotatedBits(numBits);
		for (size_t i = 0; i < numBits; i++) {
			if (i >= numBits - s)
				rotatedBits[i] = bits[i - (numBits - s)];
			else
				rotatedBits[i] = bits[i + s];
		}
		return allocate<WireArray>(generator , rotatedBits);
	}
	

	WireArray * WireArray::shiftLeft(size_t numBits, size_t s) {
		WireVector new_ws1 ;
		WireVector & bits = adjustLength2( base_array , new_ws1,  numBits);
		WireVector shiftedBits(numBits);

		for (size_t i = 0; i < numBits; i++) {
			if (i < s)
				shiftedBits[i] = generator->zeroWire;
			else
				shiftedBits[i] = bits[i - s];
		}
		return allocate<WireArray>(generator, shiftedBits);
	}
	

	WireArray * WireArray::shiftRight(size_t numBits, size_t s) {
		WireVector new_ws1 ;
		WireVector & bits = adjustLength2( base_array , new_ws1,  numBits);
		WireVector shiftedBits(numBits);

		for (size_t i = 0; i < numBits; i++) {
			if (i >= numBits - s)
				shiftedBits[i] = generator->zeroWire;
			else
				shiftedBits[i] = bits[i + s];
		}
		return allocate<WireArray>(generator , shiftedBits);
	}


	Wires WireArray::packBitsIntoWords(size_t wordBitwidth , const string & desc ){
		UNUSEDPARAM(desc)

		int numWords = (int)ceil(base_array.size()*1.0/wordBitwidth);
		
		WireVector new_ws1 ;
		WireVector & padded = adjustLength2( base_array , new_ws1,  wordBitwidth*numWords);
		
		Wires result = (numWords);
		
		for(int i = 0; i < numWords; i++){

			WireVector padded_copy ;
			// copy_of_range(padded , padded_copy , i*wordBitwidth, (i+1)*wordBitwidth );
			padded.copyOfRange2(i*wordBitwidth, (i+1)*wordBitwidth, padded_copy);
			WireArray* ws = allocate<WireArray>( generator , padded_copy );
			result[i] = ws->packAsBits();
		}
		
		return result ;
	}


	Wires WireArray::packWordsIntoLargerWords(size_t wordBitwidth, int numWordsPerLargerWord , const string & desc ){
		UNUSEDPARAM(desc)

		int numLargerWords = (int)ceil(base_array.size()*1.0/numWordsPerLargerWord);

		Wires result (generator->zeroWire , numLargerWords);
		
		for(size_t i = 0; i < base_array.size(); i++){
			size_t subIndex = i % numWordsPerLargerWord;
			BigInteger bI(2l) ; 
			bI.__pow(subIndex*wordBitwidth) ;
			WirePtr w1 = base_array[i]->mul(bI) ;
			result[i/numWordsPerLargerWord] = result[i/numWordsPerLargerWord]->add(w1);
 		}
		
		return result ;
	}


	WireArray * WireArray::getBits(size_t bitwidth, const string & desc ) {
		Wires bits(bitwidth * base_array.size());
		size_t idx = 0;
		for (size_t i = 0; i < base_array.size(); i++) {
			Wires tmp = base_array[i]->getBitWires(bitwidth, desc)->asArray();
			for (size_t j = 0; j < bitwidth; j++) {
				bits[idx++] = tmp[j];
			}
		}
		return allocate<WireArray>(generator , bits);
	}


	WireArray * WireArray::reverse() {
		Wires tempArray(base_array.size());
		for(size_t i=0; i < base_array.size() / 2; i++) {
			tempArray[i] = base_array[base_array.size() - 1 - i];
			tempArray[base_array.size() - 1 - i] = base_array[i];
		}
		if (base_array.size() % 2 == 1) {
			tempArray[base_array.size() / 2] = base_array[base_array.size() / 2];
		}
		return allocate<WireArray>(generator , tempArray);
		
	}

}
