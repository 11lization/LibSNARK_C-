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

#include "AcceptLending.hpp"
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

        AcceptLending::AcceptLending(string circuitName, int __treeHeight, const BigInteger &__G1_GENERATOR, Config &config)
            : CircuitBuilder::CircuitGenerator(circuitName, config),
              treeHeight(__treeHeight),
              G1_GENERATOR(__G1_GENERATOR)
        {
        }

        void AcceptLending::buildCircuit()
        {

            /* statements */
            CT_SKE_bondData = createInputWireArray(13, "CT_SKE_bondData");  //채권에 적혀있는 data    random을 뺌 -> CT_r  CT_bondData
            CT_SKE_bondBalance = createInputWire("CT_SKE_bondBalance");   //채권에 적혀있는 잔금 
            H_bondID = createInputWire("H_bondID");   // id_bond

            CT_creditorPKE_bondKey = createInputWire("CT_creditorPKE_bondKey"); //채권자의 pk로 Enc     CT_creditorKey
            H_monthlyRepaymentTable = createInputWire("H_monthlyRepaymentTable");         // CT_table

            H_loanAmountToReceive_debtor = createInputWire("H_loanAmountToReceiev_debtor");         // C_v_debtor
            PK_own_debtor = createInputWire("PK_own_debtor"); 
            
            CT_loanAmountToReceive_debtor = createInputWireArray(3, "CT_loanAmountToReceive_debtor"); // 수정 원래는 위에.

            //추가
            G_r_PKE_loanAmountToReceive_debtor = createInputWire("G_r_PKE_loanAmountToReceive_debtor");       
            CT_debtorPKE_loanAmountToReceive = createInputWire("CT_debtorPKE_loanAmountToReceive");
            pk_own_creditor = createInputWire("pk_own_creditor"); 
            H_originalValue_creditor = createInputWire("H_originalValue_creditor");
            nf_creditor = createInputWire("nf_creditor"); 
            H_updateValue_creditor = createInputWire("H_updateValue_creditor");

            rt = createInputWire("rt");
            wt = createInputWire("wt");

            /*witnesses */
            //k_msg = createProverWitnessWire("k_msg");
            bondBalance = createProverWitnessWire("bondBalance");
            index_bondID = createProverWitnessWire("index_bondID");
            PK_enc_creditor = createProverWitnessWire("PK_enc_creditor");
            PK_enc_debtor = createProverWitnessWire("PK_enc_debtor");

            ENA_debtor = createProverWitnessWire("ENA_debtor");
            ENA_new_creditor = createProverWitnessWire("ENA__new_creditor");
            ENA_old_creditor = createProverWitnessWire("ENA_old_creditor"); //old_ENA_creditor

            value_ENA_old_creditor = createProverWitnessWire("value_ENA_old_creditor");//, 이전 ENA 잔고  value_old_creditor
            loanAmountToReceive_debtor = createProverWitnessWire("loanAmountToReceive_debtor");//value_new_debtor, 채권자가 채무자한테 보내야할 돈
            value_ENA_new_creditor = createProverWitnessWire("value_ENA_new_creditor");//채권자가 돈 보낸 후 ENA 잔고, value_new_creditor


            bondData = createProverWitnessWireArray(bondDataLength, "bondData"); // : 13
            monthlyRepaymentTable = createProverWitnessWireArray(tableBalanceLength, "monthlyRepaymentTable"); //table_balance
            
            //추가부분!!!!!!!!
            //PKE에서 필요함  
            SK_enc_creditor = createProverWitnessWire("SK_enc_creditor");  //pkLength = 3;...skLength...?
            bondKey = createProverWitnessWire("bondKey"); //k

            datas_H_loanAmountToReceive_debtor = createProverWitnessWireArray(3, "datas_H_loanAmountToReceive_debtor");

            sk_own_creditor = createProverWitnessWire("sk_own_creditor");


            //r_creditorPKE = createProverWitnessWire("r_creditorPKE");
            //bondKey = createProverWitnessWire("bondKey"); //k
            k_ENA_creditor = createProverWitnessWire("k_ENA_creditor");
            r_H_loanAmountToReceive_debtor = createProverWitnessWire("r_H_loanAmountToReceive_debtor"); //r_C_v_debtor

            k_PKE_loanAmountToReceive_debtor = createProverWitnessWire("k_PKE_loanAmountToReceive_debtor");
            r_SKE_loanAmountToReceive_debtor = createProverWitnessWire("r_SKE_loanAmountToReceive_debtor");
            
            r_PKE_loanAmountToReceive_debtor = createProverWitnessWire("r_PKE_loanAmountToReceive_debtor");
            G_PKE_loanAmountToReceive_debtor = createProverWitnessWire("G_PKE_loanAmountToReceive_debtor");

            r_ENA_creditor = createProverWitnessWire("r_ENA_creditor"); // r_ENA_creditor
            r_old_ENA_creditor = createProverWitnessWire("r_old_ENA_creditor"); //r_old_ENA_creditor

            r_H_originalValue_creditor = createProverWitnessWire("r_H_originalValue_creditor");
            r_H_updateValue_creditor = createProverWitnessWire("r_H_updateValue_creditor");

            direction = createProverWitnessWire("direction");                               // Path(cm)
            intermediateHashes = createProverWitnessWireArray(treeHeight, "intermediateHashes");

            vector<WirePtr> nextInputWires;
            HashGadget *hashGadget;
            


            //bondBalance = v'(loanAmountToReceive_debtor) : 채무자한테 줄 돈 dv_->v_priv_out
            addEqualityAssertion(bondBalance, loanAmountToReceive_debtor, "invalid bondBalance");

            //돈 보내는 부분
            //C_v = H(pk_own_bank, r, v)  ->    H_originalValue_creditor = H(pk_own_creditor, r_H_originalValue_creditor, value_ENA_old_creditor)
            nextInputWires = {pk_own_creditor, r_H_originalValue_creditor, value_ENA_old_creditor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_originalValue_creditor, "H_originalValue_creditor not equal");

            //pk_own_creditor = H(sk_own_creditor)  
            hashGadget = allocate<HashGadget>(this, sk_own_creditor);
            addEqualityAssertion(hashGadget->getOutputWires()[0], pk_own_creditor, "pk_own_creditor not equal");

            //nf = H(sk_own_creditor || C_v)   ->    nf_creditor = H(sk_own_creditor || H_originalValue_creditor)
            nextInputWires = {sk_own_creditor, H_originalValue_creditor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], nf_creditor, "nf_creditor not equal");
            
            //H_updateValue_creditor = H(pk_own_creditor, r_H_updateValue_creditor, value_ENA_new_creditor)
            nextInputWires = {pk_own_creditor, r_H_updateValue_creditor, value_ENA_new_creditor}; //concat
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_updateValue_creditor, "H_updateValue_creditor not equal");

            //H_loanAmountToReceive_debtor = H(pk_own_debtor,r_H_loanAmountToReceive_debtor, loanAmountToReceive_debtor), 보낼 돈 커밋하고
            nextInputWires = {PK_own_debtor, r_H_loanAmountToReceive_debtor, loanAmountToReceive_debtor}; //concat
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_loanAmountToReceive_debtor, "H_loanAmountToReceive_debtor not equal");


            // membership check
            Wires leafWires = {wt};
            MerkleTreePathGadget *merkleTreeGadget = allocate<MerkleTreePathGadget>(this, direction, leafWires, *intermediateHashes, treeHeight, true);
            addOneAssertion(value_ENA_old_creditor->isEqualTo(zeroWire)->add(rt->isEqualTo(merkleTreeGadget->getOutputWires()[0]))->checkNonZero(),
                            "membership failed");







            // 여기 수정!! -> 쓸 수 있게 값들 ENC
            //c_0 = G^r    -> G_r_PKE_loanAmountToReceive_debtor = G_PKE_loanAmountToReceive_debtor^r_PKE_loanAmountToReceive_debtor
            ECGroupGeneratorGadget *ecGadget = allocate<ECGroupGeneratorGadget>(this, G_PKE_loanAmountToReceive_debtor, r_PKE_loanAmountToReceive_debtor);
            addEqualityAssertion(ecGadget->getOutputWires()[0], G_r_PKE_loanAmountToReceive_debtor, "G_r_PKE_loanAmountToReceive_debtor not equal");
            
            //c_1 = k*pk_1^r  -> CT_debtorPKE_loanAmountToReceive = k_PKE_loanAmountToReceive_debtor * PK_enc_debtor^r_PKE_loanAmountToReceive_debtor
            ecGadget = allocate<ECGroupGeneratorGadget>(this, PK_enc_debtor, r_PKE_loanAmountToReceive_debtor);
            addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(k_PKE_loanAmountToReceive_debtor), CT_debtorPKE_loanAmountToReceive, "CT_debtorPKE_loanAmountToReceive not equal");
            
            //CT_loanAmountToReceive_debtor = PKE.Enc(PK_enc_debtor, (PK_own_debtor, r_H_loanAmountToReceive_debtor, loanAmountToReceive_debtor))
            //c_3 = SKE.ENC_k(msg) -> CT_loanAmountToReceive_debtor = SKE.ENC_k_PKE_loanAmountToReceive_debtor(datas_H_loanAmountToReceive_debtor)
            //r 랜덤으로 뽑고, sct <- msg + PRF_k_PKE_loanAmountToReceive_debtor(r)  ... PRF = H(k||r)   ->   CT_loanAmountToReceive_debtor <- datas_H_loanAmountToReceive_debtor[i] + PRF_k_PKE_loanAmountToReceive_debtor(r_SKE_loanAmountToReceive_debtor)
            WirePtr CT_debtorPKE_loanAmountToReceive_temp;
            for(int i = 0 ; i < 3 ; i++)
            {
                nextInputWires = {k_PKE_loanAmountToReceive_debtor->add(i), r_SKE_loanAmountToReceive_debtor};
                hashGadget = allocate<HashGadget>(this,nextInputWires);
                CT_debtorPKE_loanAmountToReceive_temp = datas_H_loanAmountToReceive_debtor->get(i)->add(hashGadget->getOutputWires()[0]);
                addEqualityAssertion(CT_debtorPKE_loanAmountToReceive_temp, CT_loanAmountToReceive_debtor->get(i), "invalid CT_loanAmountToReceive_debtor");
            }


            //v' = v+ v'' (이전 ENA 잔고(value_ENA_old_creditor), 채권자가 돈 보낸 후 ENA 잔고(value_ENA_new_creditor),   채권자가 채무자한테 보내야할 돈(loanAmountToReceive_debtor))
            WirePtr v_eval = value_ENA_new_creditor->add(loanAmountToReceive_debtor);
            addEqualityAssertion(value_ENA_old_creditor, v_eval, "invalid value_ENA_old_creditor");

            //ENA_creditor = SKE.Enc(k_ENA_creditor, value_ENA_new_creditor) -> 현재 남아있는(update된) 채권자 ENA 잔금 
            nextInputWires = {k_ENA_creditor, r_ENA_creditor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(value_ENA_new_creditor->add(hashGadget->getOutputWires()[0]),ENA_new_creditor, "invalid ENA_new_creditor");

            //old_ENA_creditor = SKE.Enc(k_ENA_creditor, value_ENA_old_creditor) -> 이전 채권자 ENA 잔금
            nextInputWires = {k_ENA_creditor, r_old_ENA_creditor};
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(value_ENA_old_creditor->add(hashGadget->getOutputWires()[0]),ENA_old_creditor, "invalid ENA_old_creditor");


            // assert value_ENA_old_creditor >= 0
            addOneAssertion(value_ENA_old_creditor->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "value_ENA_old_creditor less than 0");

            // assert value_ENA_new_creditor >= 0
            addOneAssertion(value_ENA_new_creditor->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "value_ENA_new_creditor less than 0");

            // assert loanAmountToReceive_debtor >= 0
            addOneAssertion(loanAmountToReceive_debtor->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "loanAmountToReceive_debtor less than 0");


            return;
        }

        void AcceptLending::finalize()
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

        void AcceptLending::assignInputs(CircuitEvaluator &circuitEvaluator)
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

        auto generator = new CircuitBuilder::RealEstate::AcceptLending("RealEstate", treeHeight, G1_GENERATOR, config);
        generator->generateCircuit();

        return generator;
    }

}