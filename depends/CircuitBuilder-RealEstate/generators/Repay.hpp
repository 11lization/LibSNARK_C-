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

	class Repay : public CircuitBuilder::CircuitGenerator {
	
	private:


		/********************* INPUT ***************************/

		WirePtr u_CT_SKE_bondBalance, CT_SKE_bondBalance, old_CT_SKE_bondBalance, cnt;

		WirePtr PK_own_creditor, pk_own_debtor;
		WirePtr H_repayAmountToReceive_creditor; 

		WirePtr H_originalValue_debtor, nf_debtor, H_updateValue_debtor;
		WirePtr rt, G_r_PKE_repayAmountToReceive_creditor, CT_creditorPKE_repayAmountToReceive;

		WiresPtr CT_repayAmountToReceive_creditor;
		WirePtr H_monthlyRepaymentTable;


		/********************* Witness ***************************/
        WirePtr value_ENA_old_debtor, repayAmountToReceive_creditor, value_ENA_new_debtor;

        WirePtr  bondKey, old_bondBalance, bondBalance, r_H_repayAmountToReceive_creditor;

		WirePtr PK_enc_creditor, ENA_new_debtor, ENA_creditor, old_ENA_debtor, r_CT_SKE_bondBalance;
		WirePtr r_H_originalValue_debtor, sk_own_debtor, r_H_updateValue_debtor;
		WirePtr G_PKE_repayAmountToReceive_creditor, r_PKE_repayAmountToReceive_creditor;
		WirePtr k_PKE_repayAmountToReceive_creditor, r_SKE_repayAmountToReceive_creditor;
		WirePtr k_ENA_debtor, r_ENA_debtor, r_old_ENA_debtor, wt;

		WiresPtr datas_H_repayAmountToReceive_creditor, monthlyRepaymentTable;
		WirePtr update_bondBalance;
		/********************* MerkleTree ***************************/
		WirePtr direction;
		WiresPtr intermediateHashes;
		static const int CTLength = 3;
		static const int ctLength = 2;
		static const int pkLength = 3;
		int treeHeight;
		BigInteger G1_GENERATOR;

		static const int bondDataLength = 13;

		static const int tableBalanceLength = 12; //1년으로 고정함
		
		// /********************* INPUT ***************************/
        // WirePtr id_bond, CT_debtorKey, CT_creditorKey, CT_bond_balance;
		

		// /********************* Witness ***************************/
        
        // WirePtr CT_r_, bond_balance, k_msg;


		// /********************* MerkleTree ***************************/
		// WirePtr directionSelector;
		// WiresPtr intermediateHashWires;
		// static const int PKECipherLength = 3;
		// static const int SECipherLength = 2;
		// static const int pkLength = 3;
		// int treeHeight;
		// BigInteger G1_GENERATOR;

		// static const int bondDataLength = 14;

	protected:

		void buildCircuit() ;

		void finalize();

	public: 

		Repay(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

	};

}}