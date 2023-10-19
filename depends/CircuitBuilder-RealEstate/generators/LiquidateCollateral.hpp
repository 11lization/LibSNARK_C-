/*******************************************************************************
 * Authors: Yujin Song
 * 			Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>


namespace CircuitBuilder {
namespace RealEstate {

	class LiquidateCollateral : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
        WirePtr CT_SKE_bondBalance, H_monthlyRepaymentTable, cnt;
		
		/********************* Witness ***************************/
        
        WirePtr bondBalance, bondKey;
		WiresPtr monthlyRepaymentTable;

		/********************* MerkleTree ***************************/
		WirePtr directionSelector;
		WiresPtr intermediateHashWires;
		static const int PKECipherLength = 3;
		static const int SECipherLength = 2;
		static const int pkLength = 3;
		int treeHeight;
		BigInteger G1_GENERATOR;

		static const int bondDataLength = 13;

		static const int tableBalanceLength = 12; //1년으로 고정함

	protected:

		void buildCircuit() ;

		void finalize();

	public: 

		LiquidateCollateral(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

	};

}}