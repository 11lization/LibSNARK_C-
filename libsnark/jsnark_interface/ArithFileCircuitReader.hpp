

#pragma once 


#include <memory.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <ctime>

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#ifndef NO_PROCPS
#include <proc/readproc.h>
#endif



#include <api/context_base.hpp>
#include <libsnark/gadgetlib2/integration.hpp>
#include <libsnark/gadgetlib2/adapters.hpp>
#include <libff/common/profiling.hpp>


using namespace libsnark;
using namespace gadgetlib2;
using namespace std;


template<typename FieldT >
class ArithFileCircuitReader  {
public:

	typedef ::std::shared_ptr<LinearCombination<FieldT>> LinearCombinationPtr;
	typedef unsigned int WIRE ;
	typedef ::std::map<WIRE, unsigned int> WireMap;

	ArithFileCircuitReader(const string & arithFilePath, 
						   const string & inputsFilePath, 
						   ProtoboardPtr<FieldT> pb , 
						   libsnark::Context_base * context );
	
	~ArithFileCircuitReader();
	 
	int getNumInputs() { return numInputs;}
	int getNumOutputs() { return numOutputs;}
	
	
private:
	
	ProtoboardPtr<FieldT> pb;
	unsigned int currentVariableIdx ;

	std::vector<VariablePtr<FieldT>> variables;
	std::vector<LinearCombinationPtr> wireLinearCombinations;
	std::vector<LinearCombinationPtr> zeroPwires;

	WireMap variableMap;
	vector<WIRE> variableMap_wireIds;
	WireMap zeropMap;

	std::vector<unsigned int> wireUseCounters;
	std::vector<FieldT> wireValues;

	std::vector<WIRE> toClean;

	std::vector<WIRE> inputWireIds;
	std::vector<WIRE> nizkWireIds;
	std::vector<WIRE> outputWireIds;

	unsigned int numWires;
	unsigned int numInputs, numNizkInputs, numOutputs;

	unsigned int currentLinearCombinationIdx;

	void parseAndEval(const string & arithFilePath, const string & inputsFilePath);
	void constructCircuit(const string & arithFilePath );  // Second Pass:
	void mapValuesToProtoboard();

	int find(unsigned int, LinearCombinationPtr&, bool intentionToEdit = false);
	void clean();

	void addMulConstraint(char*, char*);
	void addXorConstraint(char*, char*);

	void addOrConstraint(char*, char*);
	void addAssertionConstraint(char*, char*);

	void addSplitConstraint(char*, char*, unsigned short);

	void addNonzeroCheckConstraint(char*, char*);

	void handleAddition(char*, char*);
	void handlePackOperation(char*, char*, unsigned short);
	void handleMulConst(char*, char*, char*);
	void handleMulNegConst(char*, char*, char*);

};
