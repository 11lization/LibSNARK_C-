
#include <OpCode.hpp>

#include <logging.hpp>



static void readIds(char* str, std::vector<unsigned int>& vec){
	istringstream iss_i(str, istringstream::in);
	unsigned int id;
	while (iss_i >> id) {
		vec.push_back(id);
	}
}


template<typename FieldT> FieldT readFieldElementFromHex(char* inputStr){
	char constStrDecimal[150];
	mpz_t integ;
	mpz_init_set_str(integ, inputStr, 16);
	mpz_get_str(constStrDecimal, 10, integ);
	mpz_clear(integ);
	FieldT f = FieldT(constStrDecimal);
	return f;
}



template<typename FieldT >
ArithFileCircuitReader<FieldT>::ArithFileCircuitReader( const string & arithFilePath , 
														const string & inputsFilePath ,
														ProtoboardPtr<FieldT> pb ,
														libsnark::Context_base * context ) 
{
	libff::UNUSED(context);
	
	std::chrono::high_resolution_clock::time_point begin , end ;
	this->pb = pb;
	numWires = 0;
	numInputs = numNizkInputs = numOutputs = 0;

	begin = std::chrono::high_resolution_clock::now();
	parseAndEval(arithFilePath, inputsFilePath);
	LOGD_HOW_LONG("parseAndEval" , begin );

	begin = std::chrono::high_resolution_clock::now();
	constructCircuit(arithFilePath);
	LOGD_HOW_LONG("constructCircuit" , begin );

	
	variableMap_wireIds.resize(variableMap.size()) ;
	size_t i = 0 ;
	for (WireMap::iterator iter = variableMap.begin(); iter != variableMap.end(); ++iter, i++ ) {
		variableMap_wireIds[i] = iter->first;
	}

	begin = std::chrono::high_resolution_clock::now();
	mapValuesToProtoboard();
	LOGD_HOW_LONG("mapValuesToProtoboard" , begin );


	wireLinearCombinations.clear();
	wireValues.clear();
	variables.clear();
	variableMap.clear();
	zeropMap.clear();
	zeroPwires.clear();
}


template<typename FieldT >
ArithFileCircuitReader<FieldT>::~ArithFileCircuitReader(){
	variables.clear() ;
	wireLinearCombinations.clear() ;
	zeroPwires.clear() ;
	variableMap.clear() ;
	variableMap_wireIds.clear() ;
	zeropMap.clear();
	wireUseCounters.clear() ;
	wireValues.clear() ;
	toClean.clear() ;
	inputWireIds.clear() ;
	nizkWireIds.clear() ;
	outputWireIds.clear() ;
	wireValues.clear();
	variables.clear();
	variableMap.clear();
	zeropMap.clear();
	zeroPwires.clear();
}


template<typename FieldT >
void ArithFileCircuitReader<FieldT>::parseAndEval(const string & arithFilePath , const string & inputsFilePath ) {

	ifstream arithfs(arithFilePath, ifstream::in);
	ifstream inputfs(inputsFilePath, ifstream::in);
	string line;

	if (!arithfs.good()) {
		LOGD("Unable to open circuit file %s \n", arithFilePath.c_str() );
		exit(-1);
	}
	else{
        LOGD("arith ok\n");
    }

    if (!inputfs.good()) {
		LOGD("Unable to open circuit file %s \n", inputsFilePath.c_str());
		exit(-1);
	}
	else{
        LOGD("inputfs ok\n");
    }


	LOGD("Parsing and Evaluating the circuit\n");


	
	getline(arithfs, line);
	int ret = sscanf(line.c_str(), "total %u", &numWires);

	if (ret != 1) {
		LOGD("File Format Does not Match\n");
		exit(-1);
	}

	wireValues.resize(numWires);
	wireUseCounters.resize(numWires);
	wireLinearCombinations.resize(numWires);

	if (!inputfs.good()) {
		LOGD("Bad inputs text content \n" );
		exit(-1);
	} else {
	    LOGD("input ok\n");
		char* inputStr;
		while (getline(inputfs, line)) {
			if (line.length() == 0) {
				continue;
			}
			WIRE wireId;
			inputStr = new char[line.size()];
			if (2 == sscanf(line.c_str(), "%u %s", &wireId, inputStr)) {
				wireValues[wireId] = readFieldElementFromHex<FieldT>(inputStr);
			} else {
				LOGD("Error in Input\n");
				exit(-1);
			}
			delete[] inputStr;
		}
		inputfs.close();
	}

	if (wireValues[0] != FieldT::one()) {
		LOGD(">> Warning: when using jsnark circuit generator, the first input wire (#0) must have the value of 1.\n");
		LOGD("\t If the circuit was generated using Pinocchio *without modification*, you can ignore this warning. Pinocchio uses a different indexing for the one-wire input. \n");
	}

	char type[200];
	char* inputStr;
	char* outputStr;
	unsigned int numGateInputs, numGateOutputs;

	WIRE wireId;

	FieldT oneElement = FieldT::one();
	FieldT zeroElement = FieldT::zero();
	FieldT negOneElement = FieldT(-1);

	// Parse the circuit: few lines were imported from Pinocchio's code.

	while (getline(arithfs, line)) {
		if (line.length() == 0) {
			continue;
		}
		inputStr = new char[line.size()];
		outputStr = new char[line.size()];

		if (line[0] == '#') {
			continue;
		} else if (1 == sscanf(line.c_str(), "input %u", &wireId)) {
			numInputs++;
			inputWireIds.push_back(wireId);
		} else if (1 == sscanf(line.c_str(), "nizkinput %u", &wireId)) {
			numNizkInputs++;
			nizkWireIds.push_back(wireId);
		} else if (1 == sscanf(line.c_str(), "output %u", &wireId)) {
			numOutputs++;
			outputWireIds.push_back(wireId);
			wireUseCounters[wireId]++;
		} else if (5
				== sscanf(line.c_str(), "%s in %u <%[^>]> out %u <%[^>]>", type,
						&numGateInputs, inputStr, &numGateOutputs, outputStr)) {

			istringstream iss_i(inputStr, istringstream::in);
			std::vector<FieldT> inValues;
			std::vector<WIRE> outWires;
			WIRE inWireId;
			while (iss_i >> inWireId) {
				wireUseCounters[inWireId]++;
				inValues.push_back(wireValues[inWireId]);
			}
			
			readIds(outputStr, outWires);

			short opcode = 0;
			FieldT constant;
			if (strcmp(type, "add") == 0) {
				opcode = ADD_OPCODE;
			} else if (strcmp(type, "mul") == 0) {
				opcode = MUL_OPCODE;
			} else if (strcmp(type, "xor") == 0) {
				opcode = XOR_OPCODE;
			} else if (strcmp(type, "or") == 0) {
				opcode = OR_OPCODE;
			} else if (strcmp(type, "assert") == 0) {
				wireUseCounters[outWires[0]]++;
				opcode = CONSTRAINT_OPCODE;
			} else if (strcmp(type, "pack") == 0) {
				opcode = PACK_OPCODE;
			} else if (strcmp(type, "zerop") == 0) {
				opcode = NONZEROCHECK_OPCODE;
			} else if (strcmp(type, "split") == 0) {
				opcode = SPLIT_OPCODE;
			} else if (strstr(type, "const-mul-neg-")) {
				opcode = MULCONST_OPCODE;
				char* constStr = type + sizeof("const-mul-neg-") - 1;
				constant = readFieldElementFromHex<FieldT>(constStr) * negOneElement;
			} else if (strstr(type, "const-mul-")) {
				opcode = MULCONST_OPCODE;
				char* constStr = type + sizeof("const-mul-") - 1;
				constant = readFieldElementFromHex<FieldT>(constStr);
			} else {
				LOGD("Error: unrecognized line: %s\n", line.c_str());
				assert(0);
			}

			// TODO: separate evaluation from parsing completely to get accurate evaluation cost
			//	 Calling  libff::get_nsec_time(); repetitively as in the old version adds much overhead
			// TODO 2: change circuit format to enable skipping some lines during evaluation
			//       Not all intermediate wire values need to be computed in this phase
			// TODO 3: change circuit format to make common constants defined once

			//begin = libff::get_nsec_time();
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
			//end =  libff::get_nsec_time();
			//evalTime += (end - begin);
		} else {
			LOGD("Error: unrecognized line: %s\n", line.c_str());
			assert(0);
		}
		delete[] inputStr;
		delete[] outputStr;
	}

	arithfs.close();
	
	// LOGD("\t Evaluation Done in %lf seconds \n", (double) (evalTime) * 1e-9);
 	LOGD("Parsing and Evaluating the circuit done\n");
}


template<typename FieldT >
void ArithFileCircuitReader<FieldT>::constructCircuit(const string & arithFilePath) {

	ifstream ifs2(arithFilePath
	, ifstream::in);

	LOGD("Translating Constraints ... ");


	#ifndef NO_PROCPS
	LOGD("NO PROCPS");
	struct proc_t usage1, usage2;
	look_up_our_self(&usage1);
    #endif


	unsigned int i;
    LOGD("%d %d %d", numInputs, numOutputs, numNizkInputs);

	currentVariableIdx = currentLinearCombinationIdx = 0;
	for (i = 0; i < numInputs; i++) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "input"));
		variableMap[inputWireIds[i]] = currentVariableIdx;
		currentVariableIdx++;
	}
	for (i = 0; i < numOutputs; i++) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "output"));
		variableMap[outputWireIds[i]] = currentVariableIdx;
		currentVariableIdx++;
	}
	for (i = 0; i < numNizkInputs; i++) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "nizk input"));
		variableMap[nizkWireIds[i]] = currentVariableIdx;
		currentVariableIdx++;
	}

	char type[200];
	char* inputStr;
	char* outputStr;
	string line;
	unsigned int numGateInputs, numGateOutputs;

	
	// Parse the circuit: few lines were imported from Pinocchio's code.

	getline(ifs2, line);
	sscanf(line.c_str(), "total %d", &numWires);

	int lineCount = 0;
	while (getline(ifs2, line)) {
		lineCount++;
		/*LOGD("%d : %s", lineCount, line.c_str());
		if (lineCount % 100000 == 0) {
			LOGD("At Line:: %d\n", lineCount);
		}*/

		if (line.length() == 0) {
			continue;
		}
		inputStr = new char[line.size()];
		outputStr = new char[line.size()];

		if (5
				== sscanf(line.c_str(), "%s in %d <%[^>]> out %d <%[^>]>", type,
						&numGateInputs, inputStr, &numGateOutputs, outputStr)) {
			if (strcmp(type, "add") == 0) {
				assert(numGateOutputs == 1);
				handleAddition(inputStr, outputStr);
			} else if (strcmp(type, "mul") == 0) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addMulConstraint(inputStr, outputStr);
			} else if (strcmp(type, "xor") == 0) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addXorConstraint(inputStr, outputStr);
			} else if (strcmp(type, "or") == 0) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addOrConstraint(inputStr, outputStr);
			} else if (strcmp(type, "assert") == 0) {
				assert(numGateInputs == 2 && numGateOutputs == 1);
				addAssertionConstraint(inputStr, outputStr);
			} else if (strstr(type, "const-mul-neg-")) {
				assert(numGateInputs == 1 && numGateOutputs == 1);
				handleMulNegConst(type, inputStr, outputStr);
			} else if (strstr(type, "const-mul-")) {
				assert(numGateInputs == 1 && numGateOutputs == 1);
				handleMulConst(type, inputStr, outputStr);
			} else if (strcmp(type, "zerop") == 0) {
				assert(numGateInputs == 1 && numGateOutputs == 2);
				addNonzeroCheckConstraint(inputStr, outputStr);
			} else if (strstr(type, "split")) {
				assert(numGateInputs == 1);
				addSplitConstraint(inputStr, outputStr, numGateOutputs);
			} else if (strstr(type, "pack")) {
				assert(numGateOutputs == 1);
				// addPackConstraint(inputStr, outputStr, numGateInputs);
				handlePackOperation(inputStr, outputStr, numGateInputs);

			}
		} else {
//			assert(0);
		}
		delete[] inputStr;
		delete[] outputStr;
		clean();
	}

	ifs2.close() ;

	LOGD("\tConstraint translation done\n");

}

 
template<typename FieldT >
void ArithFileCircuitReader<FieldT>::mapValuesToProtoboard() {

	int zeropGateIndex = 0;

	for (WIRE wireId : variableMap_wireIds) {
		pb->val(*variables[variableMap[wireId]]) = wireValues[wireId];
	}


	for ( auto Iter  : zeropMap){
		LinearCombination<FieldT> l = *zeroPwires[zeropGateIndex++];
		if (pb->val(l) == 0) {
			pb->val(*variables[Iter.second]) = 0;
		} else {
			pb->val(*variables[Iter.second]) = pb->val(l).inverse(pb->fieldType_);
		}
	}

	#ifdef DEBUG
	if (!pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED)) {
		LOGD("Note: Protoboard Not Satisfied .. \n");
		// assert(false);
	}
	#endif
}


template<typename FieldT >
int ArithFileCircuitReader<FieldT>::find(WIRE wireId, LinearCombinationPtr& lc, bool intentionToEdit) {

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
void ArithFileCircuitReader<FieldT>::clean() {

	for (WIRE wireId : toClean) {
		wireLinearCombinations[wireId].reset();
	}
	toClean.clear();
}


template<typename FieldT > 
void ArithFileCircuitReader<FieldT>::addMulConstraint(char* inputStr, char* outputStr) {

	WIRE outputWireId, inWireId1, inWireId2;

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId1;
	iss_i >> inWireId2;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

	LinearCombinationPtr l1, l2;
	find(inWireId1, l1);
	find(inWireId2, l2);

	if (variableMap.find(outputWireId) == variableMap.end()) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "mul out"));
		variableMap[outputWireId] = currentVariableIdx;
		pb->addRank1Constraint(*l1, *l2, *variables[currentVariableIdx],
				"Mul ..");
		currentVariableIdx++;
	} else {
		pb->addRank1Constraint(*l1, *l2, *variables[variableMap[outputWireId]],
				"Mul ..");
	}
}


template<typename FieldT >
void ArithFileCircuitReader<FieldT>::addXorConstraint(char* inputStr, char* outputStr) {

	WIRE outputWireId, inWireId1, inWireId2;

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId1;
	iss_i >> inWireId2;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

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
void ArithFileCircuitReader<FieldT>::addOrConstraint(char* inputStr, char* outputStr) {

	WIRE outputWireId, inWireId1, inWireId2;

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId1;
	iss_i >> inWireId2;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

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
void ArithFileCircuitReader<FieldT>::addAssertionConstraint(char* inputStr, char* outputStr) {

	WIRE outputWireId, inWireId1, inWireId2;

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId1;
	iss_i >> inWireId2;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

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
void ArithFileCircuitReader<FieldT>::addSplitConstraint(char* inputStr, char* outputStr,
		unsigned short n) {

	WIRE inWireId;
	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId;

	LinearCombinationPtr l;
	find(inWireId, l);

	istringstream iss_o(outputStr, istringstream::in);

	LinearCombination<FieldT> sum;
	FElem<FieldT> two_i = FieldT ("1");

	/*
	for (int i = 0; i < n; i++) {
		Wire bitWireId;
		iss_o >> bitWireId;
		variables.push_back(make_shared<Variable>("bit out"));
		variableMap[bitWireId] = currentVariableIdx;
		VariablePtr vptr = variables[currentVariableIdx];
		pb->enforceBooleanity(*vptr);
		sum += LinearTerm(*vptr, two_i);
		two_i += two_i;
		currentVariableIdx++;
	} */

	for (int i = 0; i < n; i++) {
		WIRE bitWireId;
		iss_o >> bitWireId;
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
void ArithFileCircuitReader<FieldT>::addNonzeroCheckConstraint(char* inputStr, char* outputStr) {

	Variable<FieldT> auxConditionInverse_ (pb->g_cxt()) ;
	WIRE outputWireId, inWireId;

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;
	iss_o >> outputWireId;
	LinearCombinationPtr l;

	find(inWireId, l);
	VariablePtr<FieldT> vptr;
	if (variableMap.find(outputWireId) == variableMap.end()) {
		variables.push_back(make_shared<Variable<FieldT>>(pb->g_cxt() , "zerop out"));
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
void ArithFileCircuitReader<FieldT>::handlePackOperation(char* inputStr, char* outputStr, unsigned short n){

	WIRE outputWireId;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of a pack operation was either defined before, or is declared directly as circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}


	istringstream iss_i(inputStr, istringstream::in);
	LinearCombinationPtr sum;
	WIRE bitWireId;
	iss_i >> bitWireId;
	find(bitWireId, sum, true);
	FElem<FieldT> two_i = FieldT  ("1");
	for (int i = 1; i < n; i++) {
		iss_i >> bitWireId;
		LinearCombinationPtr l;
		find(bitWireId, l);
		two_i += two_i;
		*sum += two_i * (*l);
	}
	wireLinearCombinations[outputWireId] = sum;
}


template<typename FieldT >
void ArithFileCircuitReader<FieldT>::handleAddition(char* inputStr, char* outputStr) {

	WIRE inWireId, outputWireId;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of an add operation was either defined before, or is declared directly as circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}

	istringstream iss_i(inputStr, istringstream::in);
	LinearCombinationPtr s, l;
	iss_i >> inWireId;
	find(inWireId, l, true);
	s = l;
	while (iss_i >> inWireId) {
		find(inWireId, l);
		*s += *l;
	}
	wireLinearCombinations[outputWireId] = s;
}

 
template<typename FieldT >
void ArithFileCircuitReader<FieldT>::handleMulConst(char* type, char* inputStr, char* outputStr) {

	char* constStr = type + sizeof("const-mul-") - 1;
	WIRE outputWireId, inWireId;

	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of a const-mul operation was either defined before, or is declared directly as a circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId;
	LinearCombinationPtr l;
	find(inWireId, l, true);
	wireLinearCombinations[outputWireId] = l;
	*(wireLinearCombinations[outputWireId]) *= readFieldElementFromHex<FieldT>(constStr);
}


template<typename FieldT >
void ArithFileCircuitReader<FieldT>::handleMulNegConst(char* type, char* inputStr, char* outputStr) {

	char* constStr = type + sizeof("const-mul-neg-") - 1;
	WIRE outputWireId, inWireId;
	istringstream iss_o(outputStr, istringstream::in);
	iss_o >> outputWireId;

	if (variableMap.find(outputWireId) != variableMap.end()) {
		LOGD("An output of a const-mul-neg operation was either defined before, or is declared directly as circuit output. Non-compliant Circuit.\n");
                LOGD("\t If the second, the wire has to be multiplied by a wire the has the value of 1 first (input #0 in circuits generated by jsnark) . \n");
		exit(-1);
	}

	istringstream iss_i(inputStr, istringstream::in);
	iss_i >> inWireId;

	LinearCombinationPtr l;
	find(inWireId, l, true);

	wireLinearCombinations[outputWireId] = l;
	*(wireLinearCombinations[outputWireId]) *= readFieldElementFromHex<FieldT>(constStr);
	*(wireLinearCombinations[outputWireId]) *= FieldT(-1); //TODO: make shared FieldT constants

}





