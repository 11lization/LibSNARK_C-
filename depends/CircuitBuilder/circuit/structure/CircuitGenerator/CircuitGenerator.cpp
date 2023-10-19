 
#include <BigInteger.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <WireArray.hpp>
#include <BitWire.hpp>
#include <ConstantWire.hpp>
#include <LinearCombinationBitWire.hpp>
#include <LinearCombinationWire.hpp>
#include <VariableBitWire.hpp>
#include <VariableWire.hpp>
#include <WireLabelInstruction.hpp>

#include <BasicOp.hpp>
#include <MulBasicOp.hpp>
#include <AssertBasicOp.hpp>

#include <Exceptions.hpp>
#include <CircuitGenerator.hpp>



#include <logging.hpp>
#include <ostream> 


namespace CircuitBuilder {

	CircuitGenerator::CircuitGenerator(string __circuitName, Config __config) 
		: 	numOfConstraints(0), 
			circuitEvaluator(NULL),
			circuitName (__circuitName) , 
			config(__config) , 
		  	zeroWire(NULL), 
		  	oneWire(NULL), 
		  	currentWireId(0),
		  	ignore_failed_assertion(false)
	{}


	
	void CircuitGenerator::generateCircuit() {
		
		LOGD("Running Circuit Generator for < %s >\n" , circuitName.c_str());
		 
		initCircuitConstruction();
		buildCircuit();
		
		LOGD("\nEmbedded Circuit [%s] Generation Done \n" , circuitName.c_str() );
		LOGD("    Constraints     : %d \n", getNumOfConstraints());
		LOGD("    Input Wires     : %zu \n", inWires.size());
		LOGD("    Output Wires    : %zu \n", outWires.size());
		LOGD("    Witness Wires   : %zu \n", proverWitnessWires.size());
		// LOGD("    EvaluationQueue : %lu\n" , evaluationQueue.size() );
		LOGD("    List of primary inputs to assign : \n");
		
		for ( auto Itr : primary_inputs ){
			const string &input_name = Itr.first ;
			LOGD( "\t\t- %s \n", input_name.c_str());
		}
	
		for ( auto Itr : primary_array_inputs ){
			const string &input_name = Itr.first ;
			vector<BigInteger> & values = Itr.second.second ;
			LOGD( "\t\t- %s , %zu \n", input_name.c_str() , values.size());
		}
		
		LOGD("\t----------------------------------\n");


		clean_deallocated();
		evaluationQueue.clear_quick_search_map() ;

	}


	void CircuitGenerator::initCircuitConstruction() {

		op_desc_list.get_allocator().allocate( 100000 ) ;
		op_desc_list.push_back( "" );
		last_desc_id = 0 ;

		// pre-allocate following vector memories
		inWires.get_allocator().allocate( config.inWires_size ) ;
		outWires.get_allocator().allocate( config.outWires_size ) ;
		proverWitnessWires.get_allocator().allocate( config.proverWitnessWires_size ) ;

		evaluationQueue = EvaluationQueue ( config.evaluationQueue_size ) ;

		oneWire = allocate<ConstantWire>(this , currentWireId++ , BigInteger::ONE() ) ;
		knownConstantWires[oneWire->getConstant()] = oneWire ;
		addToEvaluationQueue( allocate<WireLabelInstruction>( this , LabelType::input, oneWire , "The one-input wire."));
		inWires.push_back(oneWire);

		zeroWire = (ConstantWirePtr) oneWire->mul(0l);

		return ;
	}



	string CircuitGenerator::getName() {
		return circuitName;
	}

	int CircuitGenerator::getNumOfConstraints() {
		return numOfConstraints;
	}

	int CircuitGenerator::getNumWires() {
		return currentWireId;
	}

	vector<WirePtr> & CircuitGenerator::getInWires() {
		return inWires;
	}

	vector<WirePtr> & CircuitGenerator::getOutWires() {
		return outWires;
	}

	vector<WirePtr> & CircuitGenerator::getProverWitnessWires() {
		return proverWitnessWires;
	}


	// TODO optimize with hashCode aka std::map
	bool CircuitGenerator::In_inWires(WirePtr wire ){
    	for ( WirePtr w : inWires ){
    		if( w == wire ){ return true ; }
    	}
    	return false ;
	}

	bool CircuitGenerator::In_outWires(WirePtr wire ){
		for ( WirePtr w : outWires ){
    		if( w == wire ){ return true ; }
    	}
    	return false ;
	}

	bool CircuitGenerator::In_ProverWitnessWires(WirePtr wire ){
		for ( WirePtr w : proverWitnessWires ){
    		if( w == wire ){ return true ; }
    	}
    	return false ;
	}







	//
	//  Constant Wires
	//

	WirePtr CircuitGenerator::createNegConstantWire(BigInteger &x, const string & desc) {
		BigInteger negate = x.negate() ;
		return oneWire->mul(negate, desc);
	}

	WirePtr CircuitGenerator::createNegConstantWire(long x, const string & desc) {
		return oneWire->mul(-x, desc);
	}

	WirePtr CircuitGenerator::createConstantWire(const BigInteger &x, const string & desc) {
		return oneWire->mul(x, desc);
	}

	WirePtr CircuitGenerator::createConstantWire(long x, const string & desc) {
		return oneWire->mul(x, desc);
	}
	




	//
	// Wires
	//

	WirePtr CircuitGenerator::makeVariable(WirePtr wire, const string & desc) {
		WirePtr outputWire = allocate<VariableWire>(this , currentWireId++);
		BasicOp* op = allocate<MulBasicOp>(this , wire, oneWire, outputWire, desc);
		Wires * cachedOutputs = addToEvaluationQueue(op);
		if(cachedOutputs == NULL){
			return outputWire;
		}
		else{
			deallocate(op );
			currentWireId--;
			return cachedOutputs->get(0);
		}
	}


	WirePtr CircuitGenerator::create_input_wire(const string & desc) {
		WirePtr newInputWire = allocate<VariableWire>( this , currentWireId++);
		addToEvaluationQueue ( allocate<WireLabelInstruction>(this , LabelType::input, newInputWire, desc));
		inWires.push_back(newInputWire);
		return newInputWire;
	}

	WirePtr CircuitGenerator::createInputWire(const string & desc) {
		WirePtr newInputWire = create_input_wire(desc);
		if( desc.size() ){
			primary_inputs[desc] = std::pair<WirePtr, BigInteger >(newInputWire , BigInteger::ZERO() ) ;
		}
		return newInputWire;
	}

	
	WirePtr CircuitGenerator::create_prover_witness_wire(const string & desc) {
		WirePtr wire = allocate<VariableWire> ( this , currentWireId++);
		addToEvaluationQueue(allocate<WireLabelInstruction>( this , LabelType::nizkinput, wire, desc));
		proverWitnessWires.push_back(wire);
		return wire;
	}

	WirePtr CircuitGenerator::createProverWitnessWire(const string & desc) {
		WirePtr newInputWire = create_prover_witness_wire(desc) ;
		if(desc.size()){
			primary_inputs[desc] = std::pair<WirePtr, BigInteger >(newInputWire , BigInteger::ZERO() )  ;
		}
		return newInputWire;
	}

	
	WirePtr CircuitGenerator::makeOutput(WirePtr wire, const string & desc) {
		
		WirePtr outputWire = wire;
		
		if ( !(wire->instanceof_VariableWire() || wire->instanceof_VariableBitWire()) || In_inWires(wire) ) {
			wire->packIfNeeded();
			outputWire = makeVariable(wire, desc);
		} else if ( In_inWires(wire) || In_ProverWitnessWires(wire)) {
			outputWire = makeVariable(wire, desc);
		} else {
			wire->packIfNeeded();
		}

		outWires.push_back(outputWire);
		addToEvaluationQueue(allocate<WireLabelInstruction>(this , LabelType::output, outputWire, desc));
		
		return outputWire;
	}





	//
	//  Assert Wires
	//

	/**
	 * Asserts an r1cs constraint. w1*w2 = w3
	 * 
	 */
	void CircuitGenerator::addAssertion(WirePtr w1, WirePtr w2, WirePtr w3, const string & desc) {
		if (w1->instanceof_ConstantWire() && w2->instanceof_ConstantWire() && w3->instanceof_ConstantWire()) {
			BigInteger const1 = ((ConstantWirePtr) w1)->getConstant();
			BigInteger const2 = ((ConstantWirePtr) w2)->getConstant();
			BigInteger const3 = ((ConstantWirePtr) w3)->getConstant();
			if (!const3.equals(const1.multiply(const2).mod(config.FIELD_PRIME))) {
				throw runtime_error ("Assertion failed on the provided constant wires .. ");
			}
		} else {
			w1->packIfNeeded();
			w2->packIfNeeded();
			w3->packIfNeeded();
			InstructionPtr op = allocate<AssertBasicOp>(this , w1, w2, w3, desc);
			addToEvaluationQueue(op);
		}
	}

	void CircuitGenerator::addZeroAssertion(WirePtr w, const string & desc) {
		addAssertion(w, oneWire, zeroWire, desc);
	}

	void CircuitGenerator::addOneAssertion(WirePtr w, const string & desc) {
		addAssertion(w, oneWire, oneWire, desc);
	}

	void CircuitGenerator::addBinaryAssertion(WirePtr w, const string & desc) {
		WirePtr inv = w->invAsBit(desc);
		addAssertion(w, inv, zeroWire, desc);
	}

	void CircuitGenerator::addEqualityAssertion(WirePtr w1, WirePtr w2, const string & desc) {
		if(!w1->equals(w2)){
			addAssertion(w1, oneWire, w2, desc);
		}
	}

	void CircuitGenerator::addEqualityAssertion(WirePtr w1, BigInteger &b, const string & desc) {
		addAssertion(w1, oneWire, createConstantWire(b, desc), desc);
	}




	//
	// Debug Instruction
	//

	void CircuitGenerator::addDebugInstruction(WirePtr w, const string & desc) {
		w->packIfNeeded();
		addToEvaluationQueue( allocate<WireLabelInstruction>(this , LabelType::debug, w , desc));
	}

	void CircuitGenerator::addDebugInstruction(Wires & wires, const string & desc) {
		for (size_t i = 0; i < wires.size(); i++) {
			wires[i]->packIfNeeded();
			string desc2 = (desc.size() > 0) ? (desc + " - " + to_string(i)) : "" ;
			addToEvaluationQueue( allocate<WireLabelInstruction>(this, LabelType::debug, wires.get(i) , desc2 ));
		}
	}





	//
	// Wire Array
	//

	WiresPtr CircuitGenerator::generateZeroWireArray(int n) {
		WiresPtr ws = new Wires ( (WirePtr)zeroWire , (size_t) n ) ;
		return ws ;
	}

	WiresPtr CircuitGenerator::generateOneWireArray(int n) {
		WiresPtr ws = new Wires ( (WirePtr)oneWire , (size_t) n ) ;
		return ws ;
	}

	WiresPtr CircuitGenerator::createConstantWireArray(vector<BigInteger> &a, const string & desc) {
		WiresPtr w = new Wires(a.size()) ; 
		for (size_t i = 0; i < a.size(); i++) {
			w->set(i , createConstantWire(a[i] , desc));
		}
		return w;
	}

	WiresPtr CircuitGenerator::createConstantWireArray(vector<long> a, const string & desc) {
		WiresPtr w = new Wires(a.size());
		for (size_t i = 0; i < a.size() ; i++) {
			w->set(i , createConstantWire( a[i] , desc));
		}
		return w;
	}

	WiresPtr CircuitGenerator::createInputWireArray(int n, const string & desc) {
		
		WiresPtr ws = new Wires((size_t) n) ;
		
		for (int i = 0; i < n; i++) {
			if (desc.size() == 0) {
				ws->set(i, create_input_wire());
			} else {
				string desc2 = desc + " " + to_string(i) ;
				ws->set(i , create_input_wire(desc2));
			}
		}

		if (desc.size()){
			primary_array_inputs[desc] = 
				std::pair<WiresPtr,vector<BigInteger>>(ws , vector<BigInteger>(ws->size(), BigInteger::ZERO())) ;
		}

		return ws ;
	}

	WiresPtr CircuitGenerator::createProverWitnessWireArray(int n, const string & desc) {

		WiresPtr ws = new Wires ( (size_t) n);
		
		for (int k = 0; k < n; k++) {
			if (desc.size() == 0) {
				ws->set(k , create_prover_witness_wire());
			} else {
				string str = desc + " " + to_string(k);
				ws->set(k , create_prover_witness_wire(str));
			}
		}

		if(desc.size()){
			primary_array_inputs[desc] = 
				std::pair<WiresPtr,vector<BigInteger>>(ws , vector<BigInteger>(ws->size(), BigInteger::ZERO() )) ;
		}
		
		return ws;
	}

	WiresPtr CircuitGenerator::makeOutputArray(Wires & wires, const string & desc) {
		WiresPtr outs = new Wires( wires.size());
		for (size_t i = 0; i < wires.size(); i++) {
			if (desc.size() == 0) {
				outs->set(i , makeOutput(wires.get(i) ));
			} else {
				string desc2 = desc + "[" + to_string(i) + "]" ;
				outs->set(i , makeOutput(wires.get(i), desc2 ));
			}
		}
		return outs;
	}
	 



	//
	// Evaluation 
	//
	
	EvaluationQueue & CircuitGenerator::getEvaluationQueue() {
		return evaluationQueue;
	}

	Wires * CircuitGenerator::addToEvaluationQueue(InstructionPtr e) {
		
		InstructionPtr find_e = evaluationQueue.find_Instruction_by_Inputs(e) ;

		if ( find_e ) {
			if (find_e->instanceof_BasicOp()) {
				return (Wires*) (& ((BasicOp*) find_e)->getOutputs());
			}
		}

		if (e->instanceof_BasicOp()) {
			numOfConstraints += ((BasicOp*) e)->getNumMulGates();
		}
		
		evaluationQueue.put(e) ;
		
		return NULL ;  // returning null means we have not seen this instruction before
	}

	void CircuitGenerator::evalCircuit() { 
		
		if (circuitEvaluator){ 
			circuitEvaluator->clear();
			delete circuitEvaluator ;
		}

		circuitEvaluator = new CircuitEvaluator(this);
		circuitEvaluator->setWireValue( oneWire , oneWire->getConstant() );
		circuitEvaluator->setWireValue( zeroWire , zeroWire->getConstant() );
		assignInputs(*circuitEvaluator);
		circuitEvaluator->evaluate();
	}


	CircuitEvaluator* CircuitGenerator::getCircuitEvaluator() {
		if (circuitEvaluator == NULL) {
			throw NullPointerException("evalCircuit() must be called before getCircuitEvaluator()");
		}
		return circuitEvaluator;
	}





	/**
	 * Use to support computation for prover witness values outside of the
	 * circuit. See Mod_Gadget and Field_Division gadgets for examples.
	 * 
	 * @param instruction
	 */
	void CircuitGenerator::specifyProverWitnessComputation(InstructionPtr instruction) {
		addToEvaluationQueue(instruction);
	}




	//
	// print and dump  
	//

	void CircuitGenerator::DumpCircuit(){
		printCircuit( std::cout );
	}

	void CircuitGenerator::printCircuit( std::ostream & printWriter , string endline ) {
			
		printWriter << "total " << currentWireId << endline ; 

		for ( auto e : evaluationQueue ){
			if (e->doneWithinCircuit()) {
				printWriter << e->toString() << endline ; 
			}
		}
	}

	void CircuitGenerator::DumpInputs(){
		printInputs( std::cout );
	}

	void CircuitGenerator::dumpAssignments(){
		for ( size_t i = 0 ; i < circuitEvaluator->size() ; i++) {
			LOGD("%zu : %s\n" , i , circuitEvaluator->getAssignment(i).toString(16).c_str()) ;
		}
		return ;
	}
	

	void CircuitGenerator::printInputs( std::ostream & printWriter , string endline ) {
		
		if (circuitEvaluator == NULL) {
			throw NullPointerException ("evalCircuit() must be called before prepFiles()");
		}
		circuitEvaluator->printInputs(printWriter , endline );
	}


	
	
	CircuitGenerator* CircuitGenerator::create_generator() {
		return NULL ;
	}





	//
	// making updates to primary inputs
	//

	int CircuitGenerator::update_primary_input(const char* input_name , const BigInteger & value ){
		
		if( primary_inputs.find(string(input_name)) == primary_inputs.end() ){
			LOGD( "** Error : Unknown primary input name [%s] **\n" , input_name );
			return 1 ;
		}
		
		primary_inputs[string(input_name)].second  = value ;
		
		// LOGD( "Set Primary Input [%s]=%s\n" , input_name , value.toString(16).c_str() );
		
		return 0 ;
	}


	int CircuitGenerator::update_primary_input(const char* input_name , int value ){
		return update_primary_input(input_name, BigInteger((long)value) ) ;
	}


	int CircuitGenerator::update_primary_input(const char* input_name , const char* value_str ){
		return update_primary_input(input_name, BigInteger(value_str , 16 ) ) ;
	}
	

	int CircuitGenerator::update_primary_input_array(const char* input_name , size_t array_index , const BigInteger &value ){
		
		if( primary_array_inputs.find(string(input_name)) == primary_array_inputs.end() ){
			LOGD( "** Error : Unknown primary input name [%s] ** \n" , input_name );
			return 1 ;
		}

		if( array_index >= primary_array_inputs[string(input_name)].second.size() ){
			LOGD( "** Error : Index [%zu] out of range for primary input array [%s] ** \n" , array_index , input_name );
			return 2 ;
		}

		primary_array_inputs[string(input_name)].second[array_index] = value ;
		
		// LOGD( "Set Primary Input [%s][%lu]=%s \n" , input_name , array_index , value.toString(16).c_str() );

		return 0 ;
	}


	int CircuitGenerator::reset_primary_input_array(const char* input_name , const BigInteger &value ){
		
		if( primary_array_inputs.find(string(input_name)) == primary_array_inputs.end() ){
			LOGD( "** Error : Unknown primary input name [%s] ** \n" , input_name );
			return 1 ;
		}

		std::vector<BigInteger> &primary_array = primary_array_inputs[string(input_name)].second ;
		for( size_t array_index = 0 ; array_index < primary_array.size() ; array_index++ ){		
			primary_array[array_index] = value ;
		}
		
		// LOGD( "Set Primary Input [%s][%lu]=%s \n" , input_name , array_index , value.toString(16).c_str() );

		return 0 ;
	}


	int CircuitGenerator::update_primary_input_array(const char* input_name , int array_index , int value ){
		return update_primary_input_array(input_name, (size_t)array_index, BigInteger((long)value) ) ;
	}
	
	int CircuitGenerator::update_primary_input_array(const char* input_name , int array_index , const char* value_str ){
		return update_primary_input_array(input_name, (size_t) array_index, BigInteger(value_str, 16 ) ) ;
	}

	int CircuitGenerator::reset_primary_input_array(const char* input_name , int value ){
		return reset_primary_input_array(input_name, BigInteger((long)value) ) ;
	}
	
	int CircuitGenerator::reset_primary_input_array(const char* input_name , const char* value_str ){
		return reset_primary_input_array(input_name , BigInteger(value_str, 16 ) ) ;
	}
	

	int CircuitGenerator::assign_inputs(CircuitEvaluator &evaluator ) {
		
		for ( auto Itr : primary_inputs ){
			
			WirePtr w = Itr.second.first ;
			const BigInteger & val = Itr.second.second ;
			evaluator.setWireValue( w , val );

			LOGD( "Assign Primary Input [%s]=%s\n" , Itr.first.c_str() , val.toString(16).c_str() );

		}
		
		for ( auto Itr : primary_array_inputs ){
			
			WiresPtr ws = Itr.second.first ;
			vector<BigInteger> & values = Itr.second.second ;

			for ( size_t ix = 0 ; ix < values.size() ; ix++ ){
				
				evaluator.setWireValue( (*ws)[ix] , values[ix] );
				
				LOGD( "Assign Primary Input [%s][%zu]=%s \n" , Itr.first.c_str() , ix , values[ix].toString(16).c_str() );
			
			}
		}

		return 0 ;
	}



	
	uint32_t CircuitGenerator::set_desc( const string & desc ){
		if( desc.size() ){
			last_desc_id++;
			op_desc_list.push_back( desc ) ;
			return (((uint32_t)op_desc_list.size()) - 1 ) ;
		}else{
			return 0 ;
		}
	}

	string CircuitGenerator::get_desc( uint32_t desc_id ) {
		if( op_desc_list.size() > ((size_t)desc_id) ){
			return op_desc_list[desc_id];
		}else{
			return "" ;
		}
	}


}
