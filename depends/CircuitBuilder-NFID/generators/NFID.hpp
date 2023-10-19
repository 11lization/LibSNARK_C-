/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk2889@kookmin.ac.kr>
 * 			Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>


namespace CircuitBuilder {
namespace NFID {

	class NFID : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
        WirePtr op, ref_val, rt; 
		/********************* Witness ***************************/
        //WirePtr val;
        WirePtr r;
		//WirePtr cm;

		/********************** Commitment *************************/
		WirePtr issuer_pk, issuer_addr, holder_addr;
		WirePtr attr_key, attr_type, attr_value;

		/********************* MerkleTree ***************************/
		WirePtr directionSelector;
		WiresPtr intermediateHashWires;
		int treeHeight;

	protected:

		void buildCircuit() ;
		
		void finalize();

	public: 

		NFID(string circuitName, int treeHeight , Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

	};

}}