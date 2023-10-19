// /*******************************************************************************
//  * Authors: Yujin Song
//  * 			Thomas Haywood
//  *******************************************************************************/

 
// #pragma once


// #include <global.hpp>
// #include <Config.hpp>
// #include <CircuitGenerator.hpp>


// namespace CircuitBuilder {
// namespace RealEstate {

// 	class ReturnFunds : public CircuitBuilder::CircuitGenerator {
	
// 	private:

// 		/********************* INPUT ***************************/
//         WirePtr CT_creditor_value, CT_debtor_value;
		

// 		/********************* Witness ***************************/
//         WirePtr bidAmount, value_new_debtor, value_new_creditor, PK_own_debtor, PK_own_creditor,r,r1,r2;


// 		/********************* MerkleTree ***************************/
// 		WirePtr directionSelector;
// 		WiresPtr intermediateHashWires;
// 		static const int PKECipherLength = 3;
// 		static const int SECipherLength = 2;
// 		static const int pkLength = 3;
// 		int treeHeight;
// 		BigInteger G1_GENERATOR;

// 		static const int bondDataLength = 14;

// 	protected:

// 		void buildCircuit() ;

// 		void finalize();

// 	public: 

// 		ReturnFunds(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
// 		void assignInputs(CircuitEvaluator &evaluator) ;

// 	};

// }}