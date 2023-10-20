/*******************************************************************************
 * Authors: Yujin Song
 * 			Thomas Haywood
 *******************************************************************************/

#include <limits>

#include <api.hpp>
#include <global.hpp>
#include <BigInteger.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <WireArray.hpp>
#include <BitWire.hpp>
#include <ConstantWire.hpp>
#include <LinearCombinationBitWire.hpp>
#include <LinearCombinationWire.hpp>
#include <VariableBitWire.hpp>
#include <VariableWire.hpp>
#include <WireLabelInstruction.hpp>

#include <BasicOp.hpp>
#include <MulBasicOp.hpp>
#include <AssertBasicOp.hpp>

#include <Exceptions.hpp>
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>

#include "Repay.hpp"
#include "../Gadgets/BondDecryptionGadget.hpp"
#include <HashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>

#include <logging.hpp>
#include <DecryptionGadget.hpp>

using namespace CircuitBuilder::Gadgets;

namespace CircuitBuilder
{
    namespace RealEstate
    {

        Repay::Repay(string circuitName, int __treeHeight, const BigInteger &__G1_GENERATOR, Config &config)
            : CircuitBuilder::CircuitGenerator(circuitName, config),
              treeHeight(__treeHeight),
              G1_GENERATOR(__G1_GENERATOR)
        {
        }

        void Repay::buildCircuit()
        {

            /* statements */
            u_CT_SKE_bondBalance = createInputWire("u_CT_SKE_bondBalance");
            CT_SKE_bondBalance = createInputWire("CT_SKE_bondBalance"); //CT_bond_balance
            cnt = createInputWire("cnt");
            old_CT_SKE_bondBalance = createInputWire("old_CT_SKE_bondBalance"); //old_CT_bond_balance
            PK_own_creditor = createInputWire("PK_own_creditor");
            pk_own_debtor = createInputWire("pk_own_debtor");
            H_repayAmountToReceive_creditor = createInputWire("H_repayAmountToReceive_creditor"); //C_v_creditor
            H_originalValue_debtor = createInputWire("H_originalValue_debtor");
            nf_debtor = createInputWire("nf_debtor"); 
            H_updateValue_debtor = createInputWire("H_updateValue_debtor"); 
            rt = createInputWire("rt");

            G_r_PKE_repayAmountToReceive_creditor = createInputWire("G_r_PKE_repayAmountToReceive_creditor");
            CT_creditorPKE_repayAmountToReceive = createInputWire("CT_creditorPKE_repayAmountToReceive");

            CT_repayAmountToReceive_creditor = createInputWireArray(3, "CT_repayAmountToReceive_creditor");

            H_monthlyRepaymentTable = createInputWire("H_monthlyRepaymentTable");         // CT_table



            /* witnesses */
            wt = createProverWitnessWire("wt");
            value_ENA_old_debtor = createProverWitnessWire("value_ENA_old_debtor"); //value_old_debtor
            repayAmountToReceive_creditor = createProverWitnessWire("repayAmountToReceive_creditor"); //value_new_creditor
            value_ENA_new_debtor = createProverWitnessWire("value_ENA_new_debtor"); //value_new_debtor
            bondKey = createProverWitnessWire("bondKey"); //k_msg
            old_bondBalance = createProverWitnessWire("old_bondBalance"); //old_bond_balance

            update_bondBalance = createProverWitnessWire("update_bondBalance");
            bondBalance = createProverWitnessWire("bondBalance"); //bond_balance

            r_H_repayAmountToReceive_creditor = createProverWitnessWire("r_H_repayAmountToReceive_creditor"); //r_C_v_creditor

            PK_enc_creditor = createProverWitnessWire("PK_enc_creditor");
            ENA_new_debtor = createProverWitnessWire("ENA_new_debtor");
            ENA_creditor = createProverWitnessWire("ENA_creditor");
            old_ENA_debtor = createProverWitnessWire("old_ENA_debtor");

            r_CT_SKE_bondBalance = createProverWitnessWire("r_CT_SKE_bondBalance"); //r_update_bond_balance

            r_H_originalValue_debtor = createProverWitnessWire("r_H_originalValue_debtor");
            sk_own_debtor = createProverWitnessWire("sk_own_debtor");
            r_H_updateValue_debtor = createProverWitnessWire("r_H_updateValue_debtor");

            G_PKE_repayAmountToReceive_creditor = createProverWitnessWire("G_PKE_repayAmountToReceive_creditor"); 
            r_PKE_repayAmountToReceive_creditor = createProverWitnessWire("r_PKE_repayAmountToReceive_creditor"); 

            k_PKE_repayAmountToReceive_creditor = createProverWitnessWire("k_PKE_repayAmountToReceive_creditor");
            r_SKE_repayAmountToReceive_creditor = createProverWitnessWire("r_SKE_repayAmountToReceive_creditor");

            datas_H_repayAmountToReceive_creditor = createProverWitnessWireArray(3, "datas_H_repayAmountToReceive_creditor"); 

            k_ENA_debtor = createProverWitnessWire("k_ENA_debtor");
            r_ENA_debtor = createProverWitnessWire("r_ENA_debtor");
            r_old_ENA_debtor = createProverWitnessWire("r_old_ENA_debtor");
            monthlyRepaymentTable = createProverWitnessWireArray(12, "monthlyRepaymentTable");   //table_balance

            vector<WirePtr> nextInputWires;
            HashGadget *hashGadget;

            //원래 2000만, 보낸 후 1000만
            //v' = v+ v'' (이전 ENA 잔고(value_ENA_old_debtor), 채무자가 돈 보낸 후 ENA 잔고(value_ENA_new_debtor),채무자가 채권자한테 보내야할 돈(repayAmountToReceive_creditor))
            WirePtr v_eval = value_ENA_new_debtor->add(repayAmountToReceive_creditor);
            addEqualityAssertion(value_ENA_old_debtor, v_eval, "invalid value_ENA_old_debtor");

            // old_bondBalance = SKE.Dec(bondKey, old_CT_SKE_bondBalance) -> 이전
            // DecryptionGadget *decGadget2 = allocate<DecryptionGadget>(this, *old_CT_SKE_bondBalance, bondKey);
            // WirePtr old_bondBalance = decGadget2->getOutputWires()[0];

             // old_CT_SKE_bondBalance = SKE.Enc(bondKey, old_bondBalance)
            nextInputWires = {bondKey, r_CT_SKE_bondBalance};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(old_bondBalance->add(hashGadget->getOutputWires()[0]),old_CT_SKE_bondBalance, "invalid old_CT_SKE_bondBalance");

            //bondBalance = old_bondBalance - repayAmountToReceive_creditor (update = old - 채권자한테 보내야할 돈)
            WirePtr result = old_bondBalance->sub(repayAmountToReceive_creditor);
            addEqualityAssertion(result, update_bondBalance, "bondBalance not equal result");

            //CT_SKE_bondBalance = SKE.Enc(bondKey, bondBalance) : 업데이트가 잘 되었는지 
            nextInputWires = {bondKey, r_CT_SKE_bondBalance}; //r_update_bond_balance
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(update_bondBalance->add(hashGadget->getOutputWires()[0]),u_CT_SKE_bondBalance, "invalid u_CT_SKE_bondBalance");

            //돈 보내는 부분
           //H_originalValue_debtor = H(pk_own_debtor, r_H_originalValue_debtor, value_ENA_old_debtor)  
            nextInputWires = {pk_own_debtor, r_H_originalValue_debtor, value_ENA_old_debtor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_originalValue_debtor, "H_originalValue_debtor not equal");

            //pk_own_debtor = H(sk_own_debtor)  
            // hashGadget = allocate<HashGadget>(this, sk_own_debtor);
            // addEqualityAssertion(hashGadget->getOutputWires()[0], pk_own_debtor, "pk_own_debtor not equal");

            //nf_debtor = H(sk_own_debtor || H_originalValue_debtor)   
            nextInputWires = {sk_own_debtor, H_originalValue_debtor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], nf_debtor, "nf_debtor not equal");
            
            //H_repayAmountToReceive_creditor = H(pk_own_creditor,r_H_repayAmountToReceive_creditor, repayAmountToReceive_creditor), 보낼 돈 커밋하고
            nextInputWires = {PK_own_creditor, r_H_repayAmountToReceive_creditor, repayAmountToReceive_creditor}; //concat
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_repayAmountToReceive_creditor, "H_repayAmountToReceive_creditor not equal");

            //H_updateValue_debtor = H(pk_own_debtor, r_H_updateValue_debtor, value_ENA_new_debtor) , 유저 본인 돈도 update
            nextInputWires = {pk_own_debtor, r_H_updateValue_debtor, value_ENA_new_debtor}; //concat
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_updateValue_debtor, "H_updateValue_debtor not equal");

            // membership check
            // Wires leafWires = {wt};
            // MerkleTreePathGadget *merkleTreeGadget = allocate<MerkleTreePathGadget>(this, direction, leafWires, *intermediateHashes, treeHeight, true);
            // addOneAssertion(value_ENA_old_debtor->isEqualTo(zeroWire)->add(rt->isEqualTo(merkleTreeGadget->getOutputWires()[0]))->checkNonZero(),
            //                 "membership failed");

            // 여기 수정!! -> 쓸 수 있게 값들 ENC
            //c_0 = G^r    -> G_r_PKE_repayAmountToReceive_creditor = G_PKE_repayAmountToReceive_creditor^r_PKE_repayAmountToReceive_creditor
            ECGroupGeneratorGadget *ecGadget = allocate<ECGroupGeneratorGadget>(this, G_PKE_repayAmountToReceive_creditor, r_PKE_repayAmountToReceive_creditor);
            addEqualityAssertion(ecGadget->getOutputWires()[0], G_r_PKE_repayAmountToReceive_creditor, "G_r_PKE_repayAmountToReceive_creditor not equal");

            //c_1 = k*pk_1^r  -> CT_creditorPKE_repayAmountToReceive = k_PKE_repayAmountToReceive_creditor * PK_enc_creditor^r_PKE_repayAmountToReceive_creditor
            ecGadget = allocate<ECGroupGeneratorGadget>(this, PK_enc_creditor, r_PKE_repayAmountToReceive_creditor);
            addEqualityAssertion((ecGadget->getOutputWires()[0])->mul(k_PKE_repayAmountToReceive_creditor), CT_creditorPKE_repayAmountToReceive, "CT_creditorPKE_repayAmountToReceive not equal");
  
            
            //CT_repayAmountToReceive_creditor = PKE.Enc(PK_enc_creditor, (pk_own_creditor,r_H_repayAmountToReceive_creditor, repayAmountToReceive_creditor))
            //c_3 = SKE.ENC_k(msg) -> CT_repayAmountToReceive_creditor = SKE.ENC_k_PKE_repayAmountToReceive_creditor(datas_H_repayAmountToReceive_creditor)
            //r 랜덤으로 뽑고, sct <- msg + PRF_k_PKE_repayAmountToReceive_creditor(r)  ... PRF = H(k||r)   ->   CT_repayAmountToReceive_creditor <- datas_H_repayAmountToReceive_creditor[i] + PRF_k_PKE_repayAmountToReceive_creditor(r_SKE_repayAmountToReceive_creditor)
            WirePtr CT_creditorPKE_repayAmountToReceive_temp;
            for(int i = 0 ; i < 3 ; i++)
            {
                nextInputWires = {k_PKE_repayAmountToReceive_creditor->add(i), r_SKE_repayAmountToReceive_creditor};
                hashGadget = allocate<HashGadget>(this,nextInputWires);
                CT_creditorPKE_repayAmountToReceive_temp = (datas_H_repayAmountToReceive_creditor->get(i))->add(hashGadget->getOutputWires()[0]);
                addEqualityAssertion(CT_creditorPKE_repayAmountToReceive_temp, CT_repayAmountToReceive_creditor->get(i), "invalid CT_repayAmountToReceive_creditor");
            }

            //ENA_debtor = SKE.Enc(k_ENA_debtor, value_ENA_new_debtor) -> 현재 남아있는(update된) 채무자 ENA 잔금 
            nextInputWires = {k_ENA_debtor, r_ENA_debtor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(value_ENA_new_debtor->add(hashGadget->getOutputWires()[0]),ENA_new_debtor, "invalid ENA_new_debtor");

            //old_ENA_debtor = SKE.Enc(k_msg, value_ENA_old_debtor) -> 이전 채무자 ENA 잔금
            nextInputWires = {k_ENA_debtor, r_old_ENA_debtor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(value_ENA_old_debtor->add(hashGadget->getOutputWires()[0]),old_ENA_debtor, "invalid old_ENA_debtor");


            //H_monthlyRepaymentTable = H(monthlyRepaymentTable) -> array...
            for(int i = 0 ; i < tableBalanceLength ; i++){
                nextInputWires.push_back(monthlyRepaymentTable->get(i));
            }
            
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_monthlyRepaymentTable, "H_monthlyRepaymentTable != H(table_balance)");

            // bond_balance_update = table[cnt]
            // BigInteger cntBigInteger = ((ConstantWire*) cnt)->getConstant();
            // string cntString = cntBigInteger.toString();
           // stol(cntString)
            addEqualityAssertion(monthlyRepaymentTable->get(2), update_bondBalance,"bondBalance_update != table[cnt]");
        

            return;
        }

        void Repay::finalize()
        {
            // CT_userKey, CT_bankKey, CT_bond, CT_bond_balance, PK_bank, PK_user
            // if (CT_userKey)
            // {
            //     delete CT_userKey;
            // }
            // if (CT_bankKey)
            // {
            //     delete CT_bankKey;
            // }
            // if (CT_bond)
            // {
            //     delete CT_bond;
            // }
            // if (CT_bond_balance)
            // {
            //     delete CT_bond_balance;
            // }
            // if (PK_bank)
            // {
            //     delete PK_bank;
            // }
            // if (PK_user)
            // {
            //     delete PK_user;
            // }
            // if (intermediateHashWires)
            // {
            //     delete intermediateHashWires;
            // }
            CircuitGenerator::finalize();
        }

        void Repay::assignInputs(CircuitEvaluator &circuitEvaluator)
        {
            assign_inputs(circuitEvaluator);
            return;
        }

    }
}

namespace libsnark
{
    // circuit setup
    CircuitGenerator *create_crv_real_estate_generator(const CircuitArguments &circuit_arguments, const Config &__config)
    {
        string hashType = "MiMC7";
        const string treeHeightKey = "treeHeight";
        int treeHeight = 0;
        const string hashTypeKey = "hashType";

        if (circuit_arguments.find(treeHeightKey) != circuit_arguments.end())
        {
            treeHeight = atoi(circuit_arguments.at(treeHeightKey).c_str());
        }

        if (circuit_arguments.find(hashTypeKey) != circuit_arguments.end())
        {
            hashType = circuit_arguments.at(hashTypeKey);
        }

        if (treeHeight != 8 && treeHeight != 16 && treeHeight != 32 && treeHeight != 64)
        {
            LOGD("\n\n");
            LOGD("CreateGenerator     :\n");
            LOGD("Invalid tree height : %d\n", treeHeight);
            LOGD("Only support 8, 16, 32, 64 \n");
            return NULL;
        }

        if (hashType.compare("MiMC7") != 0 && hashType.compare("SHA256") != 0 && hashType.compare("Poseidon") != 0)
        {
            LOGD("\n\n");
            LOGD("CreateGenerator   :\n");
            LOGD("Invalid hash type : %s\n", hashType.c_str());
            LOGD("Only support MiMC7, SHA256, Poseidon \n");
            return NULL;
        }

        Config config = __config;

        BigInteger G1_GENERATOR = 0l;
        if (config.EC_Selection == EC_BLS12_381)
        {
            G1_GENERATOR = BigInteger("67c5b5fed18254e8acb66c1e38f33ee0975ae6876f9c5266a883f4604024b3b8", 16);
        }
        else if (config.EC_Selection == EC_ALT_BN128)
        {
            G1_GENERATOR = BigInteger("16FD271AE0AD87DDAE03044AC6852EE1D2AC024D42CFF099C50EA7510D2A70A5", 16);
        }

        config.evaluationQueue_size = 53560;
        config.inWires_size = 20;
        config.outWires_size = 0;
        config.proverWitnessWires_size = 2080;
        config.hashType = hashType;

        auto generator = new CircuitBuilder::RealEstate::Repay("RealEstate", treeHeight, G1_GENERATOR, config);
        generator->generateCircuit();

        return generator;
    }

}