

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

#include <libsnark/gadgetlib2/integration.hpp>
#include <libsnark/gadgetlib2/adapters.hpp>
#include <libff/common/profiling.hpp>

#include <OpCode.hpp>
#include <CircuitGenerator.hpp>


using namespace std;
using namespace libsnark;
using namespace gadgetlib2;


typedef struct {
	uint32_t wire_idx;
	uint32_t variable_idx ;
} wire2VariableMap_t ;


template<typename FieldT >
class EmbeddedGeneratorCircuitReader  {

public:

	typedef ::std::shared_ptr<LinearCombination<FieldT>> LinearCombinationPtr;
	typedef int32_t WIRE ;
	typedef ::std::map<WIRE, unsigned int> WireMap;

	EmbeddedGeneratorCircuitReader(CircuitGenerator * generator, 
					ProtoboardPtr<FieldT> pb , 
					wire2VariableMap_t** wire_variable_map , 
					uint32_t* wire_variable_map_count , 
					uint32_t** zero_variables_idx , 
					uint32_t* zero_variables_count ,
					libff::profiling & profile ,
					libsnark::Context_base * context );
	
	~EmbeddedGeneratorCircuitReader();

	void evaluateInputs( libff::profiling & profile );

	int getNumInputs() { return numInputs;}
	int getNumOutputs() { return numOutputs;}
	

private:
	
	CircuitGenerator * generator ;

	ProtoboardPtr<FieldT> pb;
	
	std::vector<VariablePtr<FieldT>> variables;
	std::vector<LinearCombinationPtr> wireLinearCombinations;
	std::vector<LinearCombinationPtr> zeroPwires;

	WireMap variableMap;
	WireMap zeropMap;

	std::vector<pair< WIRE , VariablePtr<FieldT> >>  variable_wireIds_map ; 
	std::vector<pair< WIRE , VariablePtr<FieldT> >>  zerop_variable_wireIds_map ; 

	std::vector<unsigned int> wireUseCounters;
	std::vector<FieldT> wireValues;

	std::vector<WIRE> toClean;

	wireID_t numWires;
	wireID_t numInputs, numNizkInputs, numOutputs;

	unsigned int currentVariableIdx , currentLinearCombinationIdx;

	void constructCircuit( ); 
	void evaluate_inputs( );
	void mapValuesToProtoboard();

	int find(WIRE, LinearCombinationPtr&, bool intentionToEdit = false);
	void clean();

	void addMulConstraint(const Wires & inputs, const Wires & outputs);
	void addXorConstraint(const Wires & inputs, const Wires & outputs);

	void addOrConstraint(const Wires & inputs, const Wires & outputs);
	void addAssertionConstraint(const Wires & inputs, const Wires & outputs);

	void addSplitConstraint(const Wires & inputs, const Wires & outputs , unsigned short);
	void addNonzeroCheckConstraint(const Wires & inputs, const Wires & outputs);

	void handleAddition(const Wires & inputs, const Wires & outputs);
	void handlePackOperation(const Wires & inputs, const Wires & outputs, unsigned short);
	void handleMulConst( const BigInteger & constant , const Wires & inputs, const Wires & outputs);
	void handleMulNegConst( const BigInteger & constant , const Wires & inputs, const Wires & outputs);

};
