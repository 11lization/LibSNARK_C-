
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>

#include "SubsetSumHashGadget.hpp"

#include <logging.hpp>

namespace CircuitBuilder {
namespace Gadgets {

	map< int , SubsetSumHashGadget::Static_t > SubsetSumHashGadget::StaticDataList ;
	
	/**
	 * @param ins
	 *            The bitwires of the input.
	 * @param binaryOutput
	 *            Whether the output digest should be splitted into bits or not.
	 * @param desc
	 */
	void SubsetSumHashGadget::buildCircuit() {

		int INPUT_LENGTH = static_data->INPUT_LENGTH ;
		vector<vector<BigInteger>> & COEFFS = static_data->COEFFS ;

		Wires outDigest(  generator->zeroWire , DIMENSION  );

		for (int i = 0; i < DIMENSION; i++) {
			for (int j = 0; j < INPUT_LENGTH ; j++) {
				WirePtr t = inputWires[j]->mul(COEFFS[i][j] );
				WirePtr w = outDigest[i]->add(t) ; 
				outDigest.set(i , w ) ;
			}
		}

		if (!binaryOutput) {
			outWires = outDigest;
		} else {
			
			outWires = Wires (DIMENSION * generator->config.LOG2_FIELD_PRIME);
			
			for (int i = 0; i < DIMENSION; i++) {
				Wires bits = outDigest[i]->getBitWires(generator->config.LOG2_FIELD_PRIME)->asArray();
				LOGD("SubsetSumHashGadget::test:: %zu :: %zu" , generator->config.LOG2_FIELD_PRIME , bits.size());
				for (size_t j = 0; j < bits.size(); j++) {
					int out_i = j + i * generator->config.LOG2_FIELD_PRIME ;
					outWires.set(out_i , bits.get(j));
				}
			}
		}
	}

	
	SubsetSumHashGadget::
		SubsetSumHashGadget(CircuitGenerator * generator, 
							Wires &ins, 
							bool __binaryOutput, 
							string desc ) 
		: Gadget(generator , desc)
	{	

		static_data = & StaticDataList[ generator->config.EC_Selection ] ;
		
		int INPUT_LENGTH = static_data->INPUT_LENGTH ;
		
		int numBlocks = (int) ceil(ins.size() * 1.0 / INPUT_LENGTH);

		if (numBlocks > 1) {
			throw invalid_argument ("Only one block is supported at this point");
		}

		int rem = numBlocks * INPUT_LENGTH - ins.size();

		Wires pad(rem) ; 
		for (size_t i = 0; i < pad.size(); i++) {
			pad.set(i , generator->zeroWire) ;  // TODO: adjust padding
		}
		inputWires = Util::concat(ins, pad);
		binaryOutput = __binaryOutput;
		buildCircuit();
	}


	Wires & SubsetSumHashGadget::getOutputWires() {
		return outWires;	
	}


	void SubsetSumHashGadget::init_static_members( const Config & config ) {

		// length in bits 254
		int INPUT_LENGTH = (2 * DIMENSION * config.LOG2_FIELD_PRIME) ;
		
		BigInteger::RandState rand;
		rand.reset();

		vector<vector<BigInteger>> COEFFS = vector<vector<BigInteger>> (DIMENSION , vector<BigInteger>(INPUT_LENGTH , BigInteger(0l) ));
		for ( int i = 0; i < DIMENSION; i++) {
			for (int k = 0; k < INPUT_LENGTH; k++) {
				COEFFS[i][k] = Util::nextRandomBigInteger(rand , config.FIELD_PRIME );
			}
		}

		StaticDataList[ config.EC_Selection ] = { INPUT_LENGTH , COEFFS } ;

		return ;
	}	


}}