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

	class AcceptLending : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
        WiresPtr CT_SKE_bondData;
		WirePtr CT_SKE_bondBalance, H_bondID, CT_creditorPKE_bondKey;
		
		WirePtr H_monthlyRepaymentTable;
		
		WirePtr H_loanAmountToReceive_debtor; //azeroth에서 cm
		WirePtr PK_own_debtor;
		WiresPtr CT_loanAmountToReceive_debtor;

		//추가
		WirePtr G_r_PKE_loanAmountToReceive_debtor, CT_debtorPKE_loanAmountToReceive;
		WirePtr pk_own_creditor, H_originalValue_creditor, nf_creditor;
		WirePtr H_updateValue_creditor, rt, wt;

		/********************* Witness ***************************/
        WirePtr bondBalance, index_bondID, PK_enc_creditor, PK_enc_debtor;
		WirePtr ENA_debtor, ENA_new_creditor, ENA_old_creditor; // , 업데이트된 sct, 이전 sct
		WirePtr value_ENA_old_creditor, loanAmountToReceive_debtor, value_ENA_new_creditor;//이전 ENA 잔고, 보내야할 돈, 돈 보낸후 ENA잔고
		WiresPtr bondData;
		
		WiresPtr monthlyRepaymentTable;

		WirePtr G_r, SK_enc_creditor, bondKey;

		WirePtr k_ENA_creditor;

		WirePtr r_H_loanAmountToReceive_debtor, r;
		WirePtr r_ENA_creditor, r_old_ENA_creditor;


		//추가
		WirePtr G_PKE_loanAmountToReceive_debtor, r_PKE_loanAmountToReceive_debtor;
		WirePtr k_PKE_loanAmountToReceive_debtor, r_SKE_loanAmountToReceive_debtor;

		WiresPtr datas_H_loanAmountToReceive_debtor;
		WirePtr r_H_originalValue_creditor, sk_own_creditor, r_H_updateValue_creditor;
		
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
        // WiresPtr CT_bondData;
		// WirePtr CT_bond_balance, id_bond, CT_creditorKey;
		
		// WirePtr CT_table;
		
		// // WirePtr commitBankValue, commitUserNewValue, commitBankNewValue, nf;
		// // WiresPtr CT_userKey, CT_bankKey, CT_bond, CT_bond_balance, PK_bank, CT_userMoney, CT_bankMoney;
        // // WirePtr id_bond, LoanPeriod, RetentionPeriod;

		// /********************* Witness ***************************/
        // WirePtr  k_msg, bond_balance, index, value_creditor;
		// WiresPtr bond_data;
		
		// WiresPtr table_balance;

		
		// // WirePtr r, value_bank, value_new_user, value_new_bank, data_balance, SK_user, SK_bank, k ;
        // // WiresPtr PK_user;

		// /********************* MerkleTree ***************************/
		// WirePtr directionSelector;
		// WiresPtr intermediateHashWires;
		// static const int PKECipherLength = 3;
		// static const int SECipherLength = 2;
		// static const int pkLength = 3;
		// int treeHeight;
		// BigInteger G1_GENERATOR;

		// static const int bondDataLength = 13;

		// static const int tableBalanceLength = 12; //1년으로 고정함

	protected:

		void buildCircuit() ;

		void finalize();

	public: 

		AcceptLending(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

	};

}}