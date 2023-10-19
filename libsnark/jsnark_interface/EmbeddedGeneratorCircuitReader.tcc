

#include <Instruction.hpp>
#include <WireLabelInstruction.hpp>
#include <Wire.hpp>
#include <CircuitEvaluator.hpp>
#include <BasicOp.hpp>
#include <ConstMulBasicOp.hpp>

#include <logging.hpp>

 
 
template<typename FieldT >
EmbeddedGeneratorCircuitReader<FieldT>::EmbeddedGeneratorCircuitReader(
					CircuitGenerator * generator, 
					ProtoboardPtr<FieldT> pb,
					wire2VariableMap_t** wire_variable_map , 
					uint32_t* wire_variable_map_count , 
					uint32_t** zero_variables_idx , 
					uint32_t* zero_variables_count ,
					libff::profiling & profile ,
					libsnark::Context_base * context)
{
	libff::UNUSED(context);
	this->pb = pb;
	this->generator = generator ;
	numInputs = numNizkInputs = numOutputs = 0;

	std::vector<WIRE> inputWireIds;
	std::vector<WIRE> nizkWireIds;
	std::vector<WIRE> outputWireIds;

	profile.enter_block("Circuit Reader" ); 
	
	numWires = generator->getNumWires();
	wireUseCounters.resize(numWires);
	wireLinearCombinations.resize(numWires);

	inputWireIds.get_allocator().allocate(numWires);
	nizkWireIds.get_allocator().allocate(numWires);
	outputWireIds.get_allocator().allocate(numWires);

	EvaluationQueue& evalSequence = generator->getEvaluationQueue();
	
	for ( auto e : evalSequence ){
		
		if ( ! e->doneWithinCircuit()) { continue ; }
		
		if (e->instanceof_WireLabelInstruction()){
			
			WireLabelInstruction* label_instr = (WireLabelInstruction*)e ;
			WIRE wireId = label_instr->getWire()->getWireId() ;
			
			switch(label_instr->getType()){
				case input :
					numInputs++;
					inputWireIds.push_back(wireId);
					break ;
				case nizkinput :
					numNizkInputs++;
					nizkWireIds.push_back(wireId);
					break ;
				case output :
					numOutputs++;
					outputWireIds.push_back(wireId);
					wireUseCounters[wireId]++;
					break ;
				case debug :
					break ;
			}
		}

		if (e->instanceof_BasicOp()) {
			
			BasicOp* Op = (BasicOp*)e ;
			const short opcode = (short)Op->op_code() ;
			
			for ( WirePtr w : Op->getInputs() ){
				WIRE inWireId = w->getWireId();
				wireUseCounters[inWireId] ++ ;
			}

			if ( opcode == CONSTRAINT_OPCODE ){
				wireUseCounters[ Op->getOutputs()[0]->getWireId() ] ++;
			}

		}
	}
 
    LOGD("numInputs:%d , numOutputs:%d , numNizkInputs:%d \n", numInputs, numOutputs, numNizkInputs);

    
	currentVariableIdx = currentLinearCombinationIdx = 0;

	for (wireID_t i = 0; i < numInputs; i++) {
		variables.push_back(make_shared<Variable<FieldT>>( pb->g_cxt() , "input"));
		variableMap[inputWireIds[i]] = currentVariableIdx;
		currentVariableIdx++;
	}

	for (wireID_t i = 0; i < numOutputs; i++) {
		variables.push_back(make_shared<Variable<FieldT>>( pb->g_cxt()  , "output"));
		variableMap[outputWireIds[i]] = currentVariableIdx;
		currentVariableIdx++;
	}

	for (wireID_t i = 0; i < numNizkInputs; i++) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt()  , "nizk input"));
		variableMap[nizkWireIds[i]] = currentVariableIdx;
		currentVariableIdx++;
	}


	
	profile.enter_block("Construct Circuit"); 
	constructCircuit( );
	profile.leave_block("Construct Circuit" ); 


	size_t ix ;
	
	wire2VariableMap_t* w_v_map  = (wire2VariableMap_t*) malloc ( sizeof(wire2VariableMap_t) * variableMap.size() ) ;
	*wire_variable_map = w_v_map ;
	*wire_variable_map_count = variableMap.size() ;
	variable_wireIds_map.get_allocator().allocate(variableMap.size()) ;
	ix = 0 ;

	for ( auto iter : variableMap ) {
		variable_wireIds_map.push_back({ iter.first , variables[iter.second] }) ;
		w_v_map[ix++] = { .wire_idx = static_cast<uint32_t>(iter.first) , 
						  .variable_idx = static_cast<uint32_t>( variables[iter.second]->index() )
						} ;
	}


	uint32_t* z_v_map = ( zeropMap.size() ) ? (uint32_t*) malloc ( sizeof(uint32_t) * zeropMap.size() ) : NULL ;
	*zero_variables_idx = z_v_map ;
	*zero_variables_count = zeropMap.size();
	zerop_variable_wireIds_map.get_allocator().allocate(zeropMap.size()) ;
	ix = 0 ;

	for ( auto iter  : zeropMap){
		zerop_variable_wireIds_map.push_back({ iter.first ,  variables[iter.second] }) ;
		z_v_map[ix++] = static_cast<uint32_t>( variables[iter.second]->index() ) ;
	}

	
	wireUseCounters.clear() ;
	inputWireIds.clear() ;
	nizkWireIds.clear() ;
	outputWireIds.clear() ;
	variables.clear();
	variableMap.clear();
	zeropMap.clear();
	wireLinearCombinations.clear() ;

	profile.leave_block("Circuit Reader" ); 
}

template<typename FieldT >
EmbeddedGeneratorCircuitReader<FieldT>::~EmbeddedGeneratorCircuitReader(){
	generator = NULL ;
	variables.clear() ;
	wireLinearCombinations.clear() ;
	zeroPwires.clear() ;
	variableMap.clear() ;
	zeropMap.clear();
	wireUseCounters.clear() ;
	wireValues.clear() ;
	toClean.clear() ;
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::constructCircuit( ){

	LOGD("Translating Constraints \n");

	EvaluationQueue& evalSequence = generator->getEvaluationQueue();
	
	for ( auto e : evalSequence ){
		
		if ( ! e->doneWithinCircuit()) {
			continue ;
		}
		
		if(e->instanceof_BasicOp()){
			
			BasicOp* Op = (BasicOp*)e ;
			const short opcode = (short)Op->op_code() ;
			const unsigned int numGateInputs = Op->getInputs().size();
			const unsigned int numGateOutputs = Op->getOutputs().size() ;

			 
			if (opcode == ADD_OPCODE) {
				assert(numGateOutputs == 1);
				handleAddition( Op->getInputs() , Op->getOutputs() );
			} else if (opcode == MUL_OPCODE) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addMulConstraint(Op->getInputs() , Op->getOutputs());
			} else if (opcode == XOR_OPCODE) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addXorConstraint(Op->getInputs() , Op->getOutputs());
			} else if (opcode == OR_OPCODE) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addOrConstraint(Op->getInputs() , Op->getOutputs());
			} else if (opcode == CONSTRAINT_OPCODE) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addAssertionConstraint(Op->getInputs() , Op->getOutputs());
			} else if (opcode == MULCONST_OPCODE) {
				ConstMulBasicOp* const_mul = (ConstMulBasicOp*)Op ;
				if (const_mul->get_inSign()) {
					assert(numGateInputs == 1 && numGateOutputs == 1);
					handleMulNegConst( const_mul->getOpCodeConstInteger() , Op->getInputs() , Op->getOutputs());
				} else{
					assert(numGateInputs == 1 && numGateOutputs == 1);
					handleMulConst( const_mul->getOpCodeConstInteger() , Op->getInputs() , Op->getOutputs());
				}
			} else if (opcode == NONZEROCHECK_OPCODE) {
				assert(numGateInputs == 1 && numGateOutputs == 2);
				addNonzeroCheckConstraint(Op->getInputs() , Op->getOutputs());
			} else if (opcode == SPLIT_OPCODE) {
				assert(numGateInputs == 1);
				addSplitConstraint(Op->getInputs() , Op->getOutputs(), numGateOutputs);
			}else if (opcode == PACK_OPCODE){
				assert(numGateOutputs == 1);
				handlePackOperation(Op->getInputs() , Op->getOutputs() , numGateInputs);
			}
			
		}

		clean();
	}

	LOGD("Translating Constraints : Done \n" );
}



template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::evaluateInputs(  libff::profiling & profile ){

	LOGD("Parsing and evaluating the inputs\n");
	profile.enter_block("Parsing and evaluating the inputs" );

	EvaluationQueue& evalSequence = generator->getEvaluationQueue();
	
	wireValues.clear();
	wireValues.resize(numWires);

	for ( auto e : evalSequence){
		if ( e->instanceof_WireLabelInstruction()){ 
			WireLabelInstruction* wl = (WireLabelInstruction*) e ;
			if ( (wl->getType() == LabelType::input) || 
				 (wl->getType() == LabelType::nizkinput)) 
			{
				const WIRE wireId = wl->getWire()->getWireId();
				wireValues[wireId] = convert2FieldT<FieldT>( generator->getCircuitEvaluator()->getAssignment( wireId )) ;
			}
		}		
	}

	if (wireValues[0] != FieldT::one()) {
		LOGD(">> Warning: when using jsnark circuit generator, the first input wire (#0) must have the value of 1.\n");
		LOGD("\t If the circuit was generated using Pinocchio *without modification*, you can ignore this warning. Pinocchio uses a different indexing for the one-wire input. \n");
	}

	profile.leave_block("Parsing and evaluating the inputs" );

	evaluate_inputs( );
	
	LOGD("Map values to protoboard\n");
	profile.enter_block("Map values to protoboard" );
	mapValuesToProtoboard();
	profile.leave_block("Map values to protoboard" );
	
	wireValues.clear();

	return ;
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::evaluate_inputs( ) {

	FieldT oneElement = FieldT::one();
	FieldT zeroElement = FieldT::zero();
	FieldT negOneElement = FieldT(-1);

	EvaluationQueue& evalSequence = generator->getEvaluationQueue();
 
	for ( auto e : evalSequence ){
		
		if ( ! e->doneWithinCircuit()) { continue ; }
		
		if (e->instanceof_BasicOp()) {
			
			BasicOp* Op = (BasicOp*)e ;
			const short opcode = (short)Op->op_code() ;
			std::vector<FieldT> inValues;
			std::vector<WIRE> outWires;
			

			for ( WirePtr w : Op->getInputs() ){
				WIRE inWireId = w->getWireId();
				inValues.push_back(wireValues[inWireId]);		
			}

			for ( WirePtr w : Op->getOutputs() ){
				WIRE outWireId = w->getWireId();
				outWires.push_back(outWireId);		
			}

			
			FieldT constant;

			if ( opcode == MULCONST_OPCODE ) {
				ConstMulBasicOp* const_mul = (ConstMulBasicOp*)Op ;
				if ( const_mul->get_inSign()) {
					constant = convert2FieldT<FieldT>(const_mul->getOpCodeConstInteger()) * negOneElement ;
				} else{
					constant = convert2FieldT<FieldT>(const_mul->getOpCodeConstInteger()) ;
				}
			}


			if (opcode == ADD_OPCODE) {
				FieldT sum;
				for (auto &v : inValues)
					sum += v;
				wireValues[outWires[0]] = sum;
			} else if (opcode == MUL_OPCODE) {
				wireValues[outWires[0]] = inValues[0] * inValues[1];
			} else if (opcode == XOR_OPCODE) {
				wireValues[outWires[0]] =
						(inValues[0] == inValues[1]) ? zeroElement : oneElement;
			} else if (opcode == OR_OPCODE) {
				wireValues[outWires[0]] =
						(inValues[0] == zeroElement
								&& inValues[1] == zeroElement) ?
								zeroElement : oneElement;
			} else if (opcode == NONZEROCHECK_OPCODE) {
				wireValues[outWires[1]] =
						(inValues[0] == zeroElement) ? zeroElement : oneElement;
			} else if (opcode == PACK_OPCODE) {
				FieldT sum, coeff;
				FieldT two = oneElement;
				for (auto &v : inValues) {
					sum += two * v;
					two += two;
				}
				wireValues[outWires[0]] = sum;
			} else if (opcode == SPLIT_OPCODE) {
				int size = outWires.size();
				FElem<FieldT> inVal = inValues[0];
				for (int i = 0; i < size; i++) {
					wireValues[outWires[i]] = inVal.getBit(i, R1P);
				}
			} else if (opcode == MULCONST_OPCODE) {
				wireValues[outWires[0]] = constant * inValues[0];
			}
		}
	}
	

}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::mapValuesToProtoboard() {

	int zeropGateIndex = 0;

	for (auto variable_wireId : variable_wireIds_map) {
		pb->val( *   variable_wireId.second ) = wireValues[variable_wireId.first];
	}


	for ( auto Iter  : zerop_variable_wireIds_map){
		LinearCombination<FieldT> l = *zeroPwires[zeropGateIndex++];
		if (pb->val(l) == 0) {
			pb->val(* Iter.second ) = 0;
		} else {
			pb->val(* Iter.second ) = pb->val(l).inverse(pb->fieldType_);
		}
	}

	#ifdef DEBUG
	if (!pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED)) {
		LOGD("Note: Protoboard Not Satisfied .. \n");
		assert(false);
	}
	#endif
}

template<typename FieldT >
int EmbeddedGeneratorCircuitReader<FieldT>::find(WIRE wireId, LinearCombinationPtr& lc, bool intentionToEdit) {

	LinearCombinationPtr p = wireLinearCombinations[wireId];
	if (p) {
		wireUseCounters[wireId]--;
		if (wireUseCounters[wireId] == 0) {
			toClean.push_back(wireId);
			lc = p;
		} else {
			if (intentionToEdit) {
				lc = make_shared<LinearCombination<FieldT>>(*p);
			} else {
				lc = p;
			}
		}
		return 1;
	} else {
		wireUseCounters[wireId]--;
		lc = make_shared<LinearCombination<FieldT>>(
				LinearCombination<FieldT>(*variables[variableMap[wireId]]));
		if (wireUseCounters[wireId] == 0) {
			toClean.push_back(wireId);
		}
		return 2;
	}
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::clean() {

	for (WIRE wireId : toClean) {
		wireLinearCombinations[wireId].reset();
	}
	toClean.clear();
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::addMulConstraint(const Wires & inputs, const Wires & outputs) {

	WIRE outputWireId, inWireId1, inWireId2;

	inWireId1 = inputs[0]->getWireId();
	inWireId2 = inputs[1]->getWireId();
	outputWireId = outputs[0]->getWireId();

	LinearCombinationPtr l1, l2;
	find(inWireId1, l1);
	find(inWireId2, l2);

	if (variableMap.find(outputWireId) == variableMap.end()) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "mul out"));
		variableMap[outputWireId] = currentVariableIdx;
		pb->addRank1Constraint(*l1, *l2, *variables[currentVariableIdx], "Mul ..");
		currentVariableIdx++;
	} else {
		pb->addRank1Constraint(*l1, *l2, *variables[variableMap[outputWireId]], "Mul ..");
	}
}


template<typename FieldT > 
void EmbeddedGeneratorCircuitReader<FieldT>::addXorConstraint(const Wires & inputs, const Wires & outputs) {

	WIRE outputWireId, inWireId1, inWireId2;

	inWireId1 = inputs[0]->getWireId();
	inWireId2 = inputs[1]->getWireId();
	outputWireId = outputs[0]->getWireId();

	LinearCombinationPtr lp1, lp2;
	find(inWireId1, lp1);
	find(inWireId2, lp2);
	LinearCombination<FieldT> l1, l2;
	l1 = *lp1;
	l2 = *lp2;
	if (variableMap.find(outputWireId) == variableMap.end()) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "xor out"));
		variableMap[outputWireId] = currentVariableIdx;
		pb->addRank1Constraint(2 * l1, l2,
				l1 + l2 - *variables[currentVariableIdx], "XOR ..");
		currentVariableIdx++;
	} else {
		pb->addRank1Constraint(2 * l1, l2,
				l1 + l2 - *variables[variableMap[outputWireId]], "XOR ..");
	}
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::addOrConstraint(const Wires & inputs, const Wires & outputs) {

	WIRE outputWireId, inWireId1, inWireId2;

	inWireId1 = inputs[0]->getWireId();
	inWireId2 = inputs[1]->getWireId();
	outputWireId = outputs[0]->getWireId();

	LinearCombinationPtr lp1, lp2;
	find(inWireId1, lp1);
	find(inWireId2, lp2);
	LinearCombination<FieldT> l1, l2;
	l1 = *lp1;
	l2 = *lp2;
	if (variableMap.find(outputWireId) == variableMap.end()) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "or out"));
		variableMap[outputWireId] = currentVariableIdx;
		pb->addRank1Constraint(l1, l2, l1 + l2 - *variables[currentVariableIdx],
				"OR ..");
		currentVariableIdx++;
	} else {
		pb->addRank1Constraint(l1, l2,
				l1 + l2 - *variables[variableMap[outputWireId]], "OR ..");
	}
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::addAssertionConstraint(const Wires & inputs, const Wires & outputs) {

	WIRE outputWireId, inWireId1, inWireId2;

	inWireId1 = inputs[0]->getWireId();
	inWireId2 = inputs[1]->getWireId();
	outputWireId = outputs[0]->getWireId();

	LinearCombinationPtr lp1, lp2, lp3;
	find(inWireId1, lp1);
	find(inWireId2, lp2);
	find(outputWireId, lp3);

	LinearCombination<FieldT> l1, l2, l3;
	l1 = *lp1;
	l2 = *lp2;
	l3 = *lp3;
	pb->addRank1Constraint(l1, l2, l3, "Assertion ..");

}

 
template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::addSplitConstraint(const Wires & inputs, const Wires & outputs, unsigned short n) {

	WIRE inWireId;
	inWireId = inputs[0]->getWireId();
	
	LinearCombinationPtr l;
	find(inWireId, l);

	LinearCombination<FieldT> sum;
	FElem<FieldT> two_i = FieldT ("1");


	for (int i = 0; i < n; i++) {
		WIRE bitWireId;
		bitWireId = outputs[i]->getWireId();
		VariablePtr<FieldT> vptr;
		if (variableMap.find(bitWireId) == variableMap.end()) {
			variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "bit out"));
			variableMap[bitWireId] = currentVariableIdx;
			vptr = variables[currentVariableIdx];
			currentVariableIdx++;
		} else {
			vptr = variables[variableMap[bitWireId]];
		}
		pb->enforceBooleanity(*vptr);
		sum += LinearTerm<FieldT>(*vptr, two_i);
		two_i += two_i;
	}


	pb->addRank1Constraint(*l, 1, sum, "Split Constraint");
}

 
template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::addNonzeroCheckConstraint(const Wires & inputs, const Wires & outputs) {

	Variable<FieldT> auxConditionInverse_(pb->g_cxt());
	WIRE outputWireId, inWireId;

	inWireId = inputs[0]->getWireId();
	outputWireId = outputs[1]->getWireId();
	LinearCombinationPtr l;

	find(inWireId, l);
	VariablePtr<FieldT> vptr;
	if (variableMap.find(outputWireId) == variableMap.end()) {
		variables.push_back(make_shared<Variable<FieldT>
		>(pb->g_cxt() , "zerop out"));
		variableMap[outputWireId] = currentVariableIdx;
		vptr = variables[currentVariableIdx];
		currentVariableIdx++;
	} else {
		vptr = variables[variableMap[outputWireId]];
	}
	variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "zerop aux"));
	pb->addRank1Constraint(*l, 1 - *vptr, 0, "condition * not(output) = 0");
	pb->addRank1Constraint(*l, *variables[currentVariableIdx], *vptr,
			"condition * auxConditionInverse = output");

	zeroPwires.push_back(l);
	zeropMap[outputWireId] = currentVariableIdx;
	currentVariableIdx++;

}

 
template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::handlePackOperation(const Wires & inputs, const Wires & outputs, unsigned short n){

	WIRE outputWireId;
	outputWireId = outputs[0]->getWireId();

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of a pack operation was either defined before, or is declared directly as circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}


	LinearCombinationPtr sum;
	WIRE bitWireId;
	bitWireId = inputs[0]->getWireId();
	find(bitWireId, sum, true);
	FElem<FieldT> two_i = FieldT ("1");
	for (int i = 1; i < n; i++) {
		bitWireId = inputs[i]->getWireId();
		LinearCombinationPtr l;
		find(bitWireId, l);
		two_i += two_i;
		*sum += two_i * (*l);
	}
	wireLinearCombinations[outputWireId] = sum;
}


template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::handleAddition( const Wires & inputs, const Wires & outputs ) {

	WIRE inWireId, outputWireId;
	outputWireId = outputs[0]->getWireId();

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of an add operation was either defined before, or is declared directly as circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}

	LinearCombinationPtr s, l;
	inWireId = inputs[0]->getWireId();
	find(inWireId, l, true);
	s = l;
	for ( size_t i = 1 ; i < inputs.size() ; i++ ){
		inWireId = inputs[i]->getWireId();
		find(inWireId, l);
		*s += *l;
	}
	wireLinearCombinations[outputWireId] = s;
}

 
template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::handleMulConst( const BigInteger & constant , const Wires & inputs, const Wires & outputs) {

	WIRE outputWireId, inWireId;

	outputWireId = outputs[0]->getWireId();

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of a const-mul operation was either defined before, or is declared directly as a circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}

	inWireId = inputs[0]->getWireId();
	LinearCombinationPtr l;
	find(inWireId, l, true);
	wireLinearCombinations[outputWireId] = l;
	*(wireLinearCombinations[outputWireId]) *= convert2FieldT<FieldT>(constant) ;
}

 
template<typename FieldT >
void EmbeddedGeneratorCircuitReader<FieldT>::handleMulNegConst(const BigInteger & constant , const Wires & inputs, const Wires & outputs ) {

	WIRE outputWireId, inWireId;
	outputWireId = outputs[0]->getWireId();

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of a const-mul-neg operation was either defined before, or is declared directly as circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}

	inWireId = inputs[0]->getWireId();

	LinearCombinationPtr l;
	find(inWireId, l, true);

	wireLinearCombinations[outputWireId] = l;
	*(wireLinearCombinations[outputWireId]) *= convert2FieldT<FieldT>(constant) ;
	*(wireLinearCombinations[outputWireId]) *= FieldT(-1); //TODO: make shared FieldT constants

}
