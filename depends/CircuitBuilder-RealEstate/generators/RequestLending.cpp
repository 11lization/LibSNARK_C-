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

#include "RequestLending.hpp"
#include "../Gadgets/BondDecryptionGadget.hpp"
#include <HashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>

#include <logging.hpp>

using namespace CircuitBuilder::Gadgets;

namespace CircuitBuilder
{
    namespace RealEstate
    {

        RequestLending::RequestLending(string circuitName, const BigInteger &__G1_GENERATOR, Config &config)
            : CircuitBuilder::CircuitGenerator(circuitName, config),
              G1_GENERATOR(__G1_GENERATOR)
        {
        }

        void RequestLending::buildCircuit()
        {
                    
            /* statements */
            CT_debtorPKE_bondKey = createInputWire("CT_debtorPKE_bondKey");     //채무자의 pk로 Enc     CT_debtorKey
            CT_creditorPKE_bondKey = createInputWire("CT_creditorPKE_bondKey"); //채권자의 pk로 Enc     CT_creditorKey
            CT_SKE_bondData = createInputWireArray(13, "CT_SKE_bondData");  //채권에 적혀있는 data    random을 뺌 -> CT_r  CT_bondData
            CT_SKE_bondBalance = createInputWire("CT_SKE_bondBalance");   //채권에 적혀있는 잔금                         CT_bond_balance
            
            H_bondID = createInputWire("H_bondID");   // id_bond

            //PKE에서 필요함 
            G_r = createInputWire("G_r");       // CT: c_0              c_0
            G = createInputWire("G");           // 이거 추가함

            H_monthlyRepaymentTable = createInputWire("H_monthlyRepaymentTable");         // CT_table


            /* witnesses */
            r_debtorPKE = createProverWitnessWire("r_debtorPKE");           // r
            r_CT_SKE_bondBalance = createProverWitnessWire("r_CT_SKE_bondBalance");   //CT_bond_balance에 대한 SKE때 필요한 random값  ,  CT_r_
            r_CT_SEK_bondData = createProverWitnessWire("r_CT_SEK_bondData");     //CT_bondData에 대한 SKE때 필요한 random값      ,  CT_r
            bondKey = createProverWitnessWire("bondKey");           // k
            ENA_debtor = createProverWitnessWire("ENA_debtor");     
            ENA_creditor = createProverWitnessWire("ENA_creditor");
            index_bondID = createProverWitnessWire("index_bondID");              //index
            bondBalance = createProverWitnessWire("bondBalance"); //bond_balance
            PK_enc_debtor = createProverWitnessWire("PK_enc_debtor");
            PK_enc_creditor = createProverWitnessWire("PK_enc_creditor");

            bondData = createProverWitnessWireArray(bondDataLength, "bondData"); // : 13   bond_data
            monthlyRepaymentTable = createProverWitnessWireArray(tableBalanceLength, "monthlyRepaymentTable");   //table_balance


            vector<WirePtr> nextInputWires;
            HashGadget *hashGadget;

            //H_bondID = H(ENA_creditor, ENA_debtor, index_bondID)
            // nextInputWires = {ENA_debtor, ENA_creditor, index_bondID}; //concat
            nextInputWires = {ENA_debtor, ENA_creditor, index_bondID}; //concat
            hashGadget = allocate<HashGadget>(this, nextInputWires);
            addEqualityAssertion(hashGadget->getOutputWires()[0], H_bondID, "H_bondID not equal");
            
            // //PKE 시작 -> PK_enc_debtor
            // //c_0 = G^r    -> G_r = G^r
            // ECGroupGeneratorGadget *ecGadget = allocate<ECGroupGeneratorGadget>(this, G, r_debtorPKE);
            // addEqualityAssertion(ecGadget->getOutputWires()[0], G_r, "G_r not equal");
            
            // //c_1 = k*pk_1^r  -> CT_debtorPKE_bondKey = bondKey * PK_enc_debtor^r_debtorPKE
            // ecGadget = allocate<ECGroupGeneratorGadget>(this, PK_enc_debtor, r_debtorPKE);
            // addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(bondKey), CT_debtorPKE_bondKey, "CT_debtorPKE_bondKey not equal");
            
            // //c_2 = k*pk_2^r  -> CT_creditorPKE_bondKey = bondKey * PK_enc_creditor^r_debtorPKE
            // ecGadget = allocate<ECGroupGeneratorGadget>(this, PK_enc_creditor, r_debtorPKE);
            // addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(bondKey), CT_creditorPKE_bondKey, "CT_creditorPKE_bondKey not equal");

            // //c_3 = SKE.ENC_k(msg) -> CT_SKE_bondData = SKE.ENC_bondKey(bondData)
            // //r 랜덤으로 뽑고, sct <- msg + PRF_k(r)  ... PRF = H(k||r)   ->   CT_SKE_bondData <- bondData + PRF_bondKey(r_CT_bondData)
            // WirePtr CT_bondData_temp;
            // for(int i = 0 ; i < 13 ; i++)
            // {
            //     nextInputWires = {bondKey, r_CT_SEK_bondData->add(i)};
            //     hashGadget = allocate<HashGadget>(this,nextInputWires);
            //     CT_bondData_temp = bondData->get(i)->add(hashGadget->getOutputWires()[0]);
            //     addEqualityAssertion(CT_bondData_temp, CT_SKE_bondData->get(i), "invalid CT_SKE_bondData");
            // }

            // //c_3 = SKE.ENC_k(msg) -> CT_SKE_bondBalance = SKE.ENC_bondKey(bondBalance) 
            // nextInputWires = {bondKey, r_CT_SKE_bondBalance};
            // hashGadget = allocate<HashGadget>(this, nextInputWires);
            // addEqualityAssertion(bondBalance->add(hashGadget->getOutputWires()[0]),CT_SKE_bondBalance, "invalid CT_SKE_bondBalance");


            // //H_monthlyRepaymentTable = H(monthlyRepaymentTable) -> array...
            // for(int i = 0 ; i < tableBalanceLength ; i++){
            //     nextInputWires.push_back(monthlyRepaymentTable->get(i));
            // }

            // hashGadget = allocate<HashGadget>(this, nextInputWires);
            // addEqualityAssertion(hashGadget->getOutputWires()[0], H_monthlyRepaymentTable, "H_monthlyRepaymentTable != H(monthlyRepaymentTable)");

            // //채권 안에 있는 대출 계약 내용 중에 양식에 맞춰 제대로 작성되었는지 체크하는 부분
            // //////////////////////////////////////////// 숫자
            // /*bond_data[13]= 
            // 0 : 채무자ENA, 
            // 1 : 채권자ENA, 
            // 2 : 상품명(상품번호), 
            // 3 : 대출신청금액, 
            // 4 : 대출기간, 
            // 5 : 거치기간, 
            // 6 : 대출개시일, 
            // 7 : 대출기간만료일, 
            // 8 : 상환방법, 
            // 9 : 주택담보대출비율, 
            // 10: 대출금리, 
            // 11: 총이자, 
            // 12: 담보가치*/
            
            // WirePtr loanPeriod= bondData->get(4);       //대출기간

            // BigInteger num = BigInteger("12");
            // // loanPeriod = 12
            // addEqualityAssertion(loanPeriod, num, "invalid period");

            // //대출기간만료일 = 대출개시일 + 대출기간
            // WirePtr result = bondData->get(6)->add(bondData->get(4));
            // addEqualityAssertion(bondData->get(7), result,"invalid result");
            
            // //대출금리 = 3% 고정
            // num = BigInteger("3");
            // addEqualityAssertion(bondData->get(10), num,"loanInterestRate != 3"); //3%
            
            // //LTV.최소(1%) <= 대출신청금액 / 담보가치*100 <= LTV.최대(70%)

            // //LTV.최소(1%) * 담보가치 <= 대출신청금액 * 100 
            // //LTV.최대(70%) * 담보가치 >= 대출신청금액 * 100

            // //대출신청금액 : bond_data->get(3), 담보가치 : bond_data->get(12)
            // BigInteger min = BigInteger("1");
            // BigInteger max = BigInteger("70");

            // BigInteger num100 = BigInteger("100");

            // WirePtr leftResult = bondData->get(12)->mul(min);
            // WirePtr rightResult = bondData->get(3)->mul(num100);
            
            // addOneAssertion(leftResult->isLessThanOrEqual(rightResult, config.LOG2_FIELD_PRIME - 1),"left greater than right");

            // leftResult = bondData->get(12)->mul(max);
            // addOneAssertion(leftResult->isGreaterThanOrEqual(rightResult,config.LOG2_FIELD_PRIME - 1), "left less than right");

            return;
        }

        void RequestLending::finalize()
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

        void RequestLending::assignInputs(CircuitEvaluator &circuitEvaluator)
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

        auto generator = new CircuitBuilder::RealEstate::RequestLending("RealEstate", G1_GENERATOR, config);
        generator->generateCircuit();

        return generator;
    }

}