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

	class RequestLending : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
    WirePtr CT_debtorPKE_bondKey, CT_creditorPKE_bondKey, CT_SKE_bondBalance;
		WiresPtr CT_SKE_bondData;

        WirePtr H_bondID; 
        WirePtr G_r, G;

        WirePtr H_monthlyRepaymentTable;

		/********************* Witness ***************************/
    WirePtr r_debtorPKE,r_CT_SKE_bondBalance, r_CT_SEK_bondData;
		WirePtr bondKey, ENA_debtor, ENA_creditor, index_bondID, bondBalance, PK_enc_debtor, PK_enc_creditor;
        
		WiresPtr bondData;
        WiresPtr monthlyRepaymentTable;

		/********************* MerkleTree ***************************/
		//WirePtr directionSelector;
		//WiresPtr intermediateHashWires;
		static const int PKECipherLength = 3;
		static const int SECipherLength = 2;
        static const int bondDataLength = 13;
		//static const int pkLength = 3;
		//int treeHeight;
		BigInteger G1_GENERATOR;

        static const int tableBalanceLength = 12; //1년으로 고정함

	protected:

		void buildCircuit() ;

		void finalize();

	public: 

		RequestLending(string circuitName, const BigInteger & G1_GENERATOR , Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

	};

}}