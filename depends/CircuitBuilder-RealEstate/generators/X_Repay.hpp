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

// 	class Repay : public CircuitBuilder::CircuitGenerator {
	
// 	private:


// 		/********************* INPUT ***************************/
//         WirePtr apk, rt, sn, addr, k_b, k_u, cm_;  //sn = nf
// 		WiresPtr cin, cout, CT;
//         WirePtr pv, pv_, G_r, K_u, K_a, G;

// 		WirePtr id_bond, CT_debtorKey, CT_bond_balance;
// 		WiresPtr CT_bondData;

// 		/********************* Witness ***************************/
//         WirePtr sk, cm, du, dv, addr_r, k_b_, k_u_, du_, dv_, r, k;
        
// 		WirePtr CT_r_, bond_balance, k_msg;
// 		WiresPtr bond_data;

// 		/********************* MerkleTree ***************************/
// 		WirePtr directionSelector;
// 		WiresPtr intermediateHashWires;
// 		static const int CTLength = 3;
// 		static const int ctLength = 2;
// 		static const int pkLength = 3;
// 		int treeHeight;
// 		BigInteger G1_GENERATOR;

// 		static const int bondDataLength = 14;

		
// 		// /********************* INPUT ***************************/
//         // WirePtr id_bond, CT_debtorKey, CT_creditorKey, CT_bond_balance;
		

// 		// /********************* Witness ***************************/
        
//         // WirePtr CT_r_, bond_balance, k_msg;


// 		// /********************* MerkleTree ***************************/
// 		// WirePtr directionSelector;
// 		// WiresPtr intermediateHashWires;
// 		// static const int PKECipherLength = 3;
// 		// static const int SECipherLength = 2;
// 		// static const int pkLength = 3;
// 		// int treeHeight;
// 		// BigInteger G1_GENERATOR;

// 		// static const int bondDataLength = 14;

// 	protected:

// 		void buildCircuit() ;

// 		void finalize();

// 	public: 

// 		Repay(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
// 		void assignInputs(CircuitEvaluator &evaluator) ;

// 	};

// }}