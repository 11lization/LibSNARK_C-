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

// 	class AcceptLending : public CircuitBuilder::CircuitGenerator {
	
// 	private:

// 		/********************* INPUT ***************************/
//         WirePtr apk, rt, sn, addr, k_b, k_u, cm_;  //sn = nf
// 		WiresPtr cin, cout, CT;
//         WirePtr pv, pv_, G_r, K_u, K_a, G;
// 		WirePtr c_0, c_1;

// 		WiresPtr CT_bondData;
// 		WirePtr CT_bond_balance, id_bond, CT_creditorKey;
// 		WirePtr CT_table;

// 		/********************* Witness ***************************/
//         WirePtr sk, cm, du, dv, addr_r, k_b_, k_u_, du_, dv_, r, k;
        

//         WirePtr  k_msg, bond_balance, index; // value_creditor 필요없을 것 같음
// 		WiresPtr bond_data;
// 		WiresPtr table_balance;

// 		/********************* MerkleTree ***************************/
// 		WirePtr directionSelector;
// 		WiresPtr intermediateHashWires;
// 		static const int CTLength = 3;
// 		static const int ctLength = 2;
// 		static const int pkLength = 3;
// 		int treeHeight;
// 		BigInteger G1_GENERATOR;

// 		static const int bondDataLength = 13;

// 		static const int tableBalanceLength = 12; //1년으로 고정함
	
// 		// /********************* INPUT ***************************/
//         // WiresPtr CT_bondData;
// 		// WirePtr CT_bond_balance, id_bond, CT_creditorKey;
		
// 		// WirePtr CT_table;
		
// 		// // WirePtr commitBankValue, commitUserNewValue, commitBankNewValue, nf;
// 		// // WiresPtr CT_userKey, CT_bankKey, CT_bond, CT_bond_balance, PK_bank, CT_userMoney, CT_bankMoney;
//         // // WirePtr id_bond, LoanPeriod, RetentionPeriod;

// 		// /********************* Witness ***************************/
//         // WirePtr  k_msg, bond_balance, index, value_creditor;
// 		// WiresPtr bond_data;
		
// 		// WiresPtr table_balance;

		
// 		// // WirePtr r, value_bank, value_new_user, value_new_bank, data_balance, SK_user, SK_bank, k ;
//         // // WiresPtr PK_user;

// 		// /********************* MerkleTree ***************************/
// 		// WirePtr directionSelector;
// 		// WiresPtr intermediateHashWires;
// 		// static const int PKECipherLength = 3;
// 		// static const int SECipherLength = 2;
// 		// static const int pkLength = 3;
// 		// int treeHeight;
// 		// BigInteger G1_GENERATOR;

// 		// static const int bondDataLength = 13;

// 		// static const int tableBalanceLength = 12; //1년으로 고정함

// 	protected:

// 		void buildCircuit() ;

// 		void finalize();

// 	public: 

// 		AcceptLending(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
// 		void assignInputs(CircuitEvaluator &evaluator) ;

// 	};

// }}