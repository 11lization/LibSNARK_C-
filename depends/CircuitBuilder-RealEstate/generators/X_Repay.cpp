// /*******************************************************************************
//  * Authors: Yujin Song
//  * 			Thomas Haywood
//  *******************************************************************************/

// #include <limits>

// #include <api.hpp>
// #include <global.hpp>
// #include <BigInteger.hpp>
// #include <Instruction.hpp>
// #include <Wire.hpp>
// #include <WireArray.hpp>
// #include <BitWire.hpp>
// #include <ConstantWire.hpp>
// #include <LinearCombinationBitWire.hpp>
// #include <LinearCombinationWire.hpp>
// #include <VariableBitWire.hpp>
// #include <VariableWire.hpp>
// #include <WireLabelInstruction.hpp>

// #include <BasicOp.hpp>
// #include <MulBasicOp.hpp>
// #include <AssertBasicOp.hpp>

// #include <Exceptions.hpp>
// #include <CircuitGenerator.hpp>
// #include <CircuitEvaluator.hpp>

// #include "Repay.hpp"
// #include "../Gadgets/BondDecryptionGadget.hpp"
// #include <HashGadget.hpp>
// #include <MerkleTreePathGadget.hpp>
// #include <ECGroupOperationGadget.hpp>
// #include <ECGroupGeneratorGadget.hpp>

// #include <logging.hpp>
// #include <DecryptionGadget.hpp>

// using namespace CircuitBuilder::Gadgets;

// namespace CircuitBuilder
// {
//     namespace RealEstate
//     {

//         Repay::Repay(string circuitName, int __treeHeight, const BigInteger &__G1_GENERATOR, Config &config)
//             : CircuitBuilder::CircuitGenerator(circuitName, config),
//               treeHeight(__treeHeight),
//               G1_GENERATOR(__G1_GENERATOR)
//         {
//         }

//         void Repay::buildCircuit()
//         {

//             /* statements */
//             rt = createInputWire("rt");
//             sn = createInputWire("sn");
//             addr = createInputWire("addr"); // PK: pk_0 -> addr (sender)
//             k_b = createInputWire("k_b");   //     pk_1 -> own
//             k_u = createInputWire("k_u");   //     pk_2 -> enc
//             cm_ = createInputWire("cm_");   //cm_{new}
//             cout = createInputWireArray(ctLength, "cout"); // ct' -> sct_{new}
//             pv = createInputWire("pv");         //v_{in}^{pub}
//             pv_ = createInputWire("pv_");       //v_{out}^{pub}
//             G_r = createInputWire("G_r");       // CT: c_0
//             K_u = createInputWire("K_u");       //     c_1
//             K_a = createInputWire("K_a");       //     c_2
//             CT = createInputWireArray(3, "CT"); //     c_3      다 합해서 pct_{new}
//             G = createConstantWire(G1_GENERATOR);
//             apk = createInputWire("apk");
//             cin = createInputWireArray(ctLength, "cin"); // ct -> sct_{old}

//             // /* add- statements */
//             id_bond = createInputWire("id_bond");
//             CT_debtorKey = createInputWire("CT_debtorKey");
//             CT_bond_balance = createInputWire("CT_bond_balance");
//             CT_bondData = createInputWireArray(13, "CT_bondData");

//             /* witnesses */
//             sk = createProverWitnessWire("sk"); //usk
//             cm = createProverWitnessWire("cm"); //cm_{old}
//             du = createProverWitnessWire("du"); //o_{old}
//             dv = createProverWitnessWire("dv"); //v_{in}^{priv}
//             addr_r = createProverWitnessWire("addr_r"); // PK': pk'_0 -> addr (receiver)
//             k_b_ = createProverWitnessWire("k_b_");     //      pk'_1 -> own
//             k_u_ = createProverWitnessWire("k_u_");     //      pk'_2 -> enc
//             du_ = createProverWitnessWire("du_");       //du_ => o_{new}
//             dv_ = createProverWitnessWire("dv_");       //dv_ => v_{out}^{priv}
//             r = createProverWitnessWire("r");   //aux
//             k = createProverWitnessWire("k");   //aux
//             directionSelector = createProverWitnessWire("direction");                               // Path(cm)
//             intermediateHashWires = createProverWitnessWireArray(treeHeight, "intermediateHashes"); //

//             // // /* add - witnesses */
//             CT_r_ = createProverWitnessWire("CT_r_");
//             bond_balance = createProverWitnessWire("bond_balance");
//             k_msg = createProverWitnessWire("k_msg");
//             bond_data = createProverWitnessWireArray(bondDataLength, "bond_data"); // : 13




//             vector<WirePtr> nextInputWires;
//             HashGadget *hashGadget;

//             /* receive a commitment */
//             hashGadget = allocate<HashGadget>(this, sk);
//             addEqualityAssertion(hashGadget->getOutputWires()[0], k_b, "k_b not equal"); //hashGadget->getOutputWires()[0] 이것이 해시 output

//             nextInputWires = {k_b, k_u}; //concat
//             hashGadget = allocate<HashGadget>(this, nextInputWires);
//             addEqualityAssertion(hashGadget->getOutputWires()[0], addr, "addr not equal");

//             nextInputWires = {du, dv, addr};
//             hashGadget = allocate<HashGadget>(this, nextInputWires);
//             addEqualityAssertion(hashGadget->getOutputWires()[0], cm, "cm not equal");

//             nextInputWires = {cm, sk};
//             hashGadget = allocate<HashGadget>(this, nextInputWires);
//             addEqualityAssertion(hashGadget->getOutputWires()[0], sn, "sn not equal");

//             // membership check
//             Wires leafWires = {cm};
//             MerkleTreePathGadget *merkleTreeGadget = allocate<MerkleTreePathGadget>(this, directionSelector, leafWires, *intermediateHashWires, treeHeight, true);
//             addOneAssertion(dv->isEqualTo(zeroWire)->add(rt->isEqualTo(merkleTreeGadget->getOutputWires()[0]))->checkNonZero(),
//                             "membership failed");

//             /* send a commitment */
//             ECGroupGeneratorGadget *ecGadget = allocate<ECGroupGeneratorGadget>(this, G, r);
//             addEqualityAssertion(ecGadget->getOutputWires()[0], G_r, "G_r not equal");

//             ecGadget = allocate<ECGroupGeneratorGadget>(this, k_u_, r);
//             addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(k), K_u, "K_u' not equal");

//             ecGadget = allocate<ECGroupGeneratorGadget>(this, apk, r);
//             addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(k), K_a, "K_a' not equal");

//             vector<WirePtr> plain = {du_, dv_, addr_r}; //du_ => o_{new}, dv_ => v_{out}^{priv}, PK': pk'_0 -> addr (receiver)
//             for (int i = 0; i < 3; i++)
//             {
//                 hashGadget = allocate<HashGadget>(this, k->add(i));
//                 addEqualityAssertion((*CT)[i], plain[i]->add(hashGadget->getOutputWires()[0]), "ciphertext is not equal");
//             }
//             //여기까지 PE.ENC 과정 

//             nextInputWires = {k_b_, k_u_};
//             hashGadget = allocate<HashGadget>(this, nextInputWires);
//             addEqualityAssertion(addr_r, hashGadget->getOutputWires()[0], "addr' is not equal");

//             nextInputWires = {du_, dv_, addr_r};
//             hashGadget = allocate<HashGadget>(this, nextInputWires);
//             addEqualityAssertion(hashGadget->getOutputWires()[0], cm_, "cm' not equal");

//             /* balance check */
//             DecryptionGadget *decGadget = allocate<DecryptionGadget>(this, *cin, sk);
//             WirePtr v_in = decGadget->getOutputWires()[0];      //v_{old}^{ENA}
//             v_in = v_in->mul(((*cin)[0]->add((*cin)[1]))->checkNonZero()); // if ct == 0 then v_in = 0

//             decGadget = allocate<DecryptionGadget>(this, *cout, sk);
//             WirePtr v_out = decGadget->getOutputWires()[0]; //v_{new}^{ENA}

//             // WirePtr zeroWire = getZeroWire();

//             // pin + v_ - v
//             WirePtr v_eval = v_in->add(dv)->sub(dv_)->add(pv)->sub(pv_);
//             addEqualityAssertion(v_out, v_eval, "invalid v_out");

//             addOneAssertion(dv->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "dv less than 0");
//             addOneAssertion(dv_->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "dv_ less than 0");
//             addOneAssertion(pv->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "pv less than 0");
//             addOneAssertion(pv_->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "pv_ less than 0");
//             // assert v >= 0
//             addOneAssertion(v_in->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "v less than 0");
//             // assert (vout >= 0)
//             addOneAssertion(v_out->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "v_ less than 0");


//             /*추가로 작성한 부분*/
//             //bond_balance = bond_balance - v''(value_new_creditor) -> dv_ -> v_priv_out
//             WirePtr v_eval = bond_balance->sub(dv_);
//             addEqualityAssertion(bond_balance, v_eval, "invalid bond_balance");


//             // k_msg = PKE.Dec(usk,CT_k_creditor)





//             // bond_balance = SKE.Dec(k_msg, CT_bond_balance)
//             DecryptionGadget *decGadget = allocate<DecryptionGadget>(this, *CT_bond_balance, k_msg);
//             addEqualityAssertion(decGadget->getOutputWires()[0], bond_balance, "bond_balance not equal");

//             // bond_data = SKE.Dec(k_msg, CT_bondData)
//             decGadget = allocate<DecryptionGadget>(this, *CT_bondData, k_msg);
//             vector<WirePtr> bondDatas(13);
//             for(int i = 0 ; i < 13 ; i++)
//             {
//                 bondDatas.push_back(decGadget->getOutputWires()[i]);
//             }

//             for(int i = 0 ; i < 13 ; i++)
//             {
//                 addEqualityAssertion((*bond_data)[i], bondDatas[i], "bond_data not equal");
//             }


//             // //////////////////////////////////////////// 숫자
//             // /*bond_data[13]= 
//             // 0 : 채무자ENA, 
//             // 1 : 채권자ENA, 
//             // 2 : 상품명(상품번호), 
//             // 3 : 대출신청금액, 
//             // 4 : 대출기간, 
//             // 5 : 거치기간, 
//             // 6 : 대출개시일, 
//             // 7 : 대출기간만료일, 
//             // 8 : 상환방법, 
//             // 9 : 주택담보대출비율, 
//             // 10: 대출금리, 
//             // 11: 총이자, 
//             // 12: 담보가치*/
            
//             // //납입원금 = 상품명.대출신청금액(bond_data->get(3)) / 상품명.대출기간(bond_data->get(4))
//             // Wire


//             return;
//         }

//         void Repay::finalize()
//         {
//             // CT_userKey, CT_bankKey, CT_bond, CT_bond_balance, PK_bank, PK_user
//             // if (CT_userKey)
//             // {
//             //     delete CT_userKey;
//             // }
//             // if (CT_bankKey)
//             // {
//             //     delete CT_bankKey;
//             // }
//             // if (CT_bond)
//             // {
//             //     delete CT_bond;
//             // }
//             // if (CT_bond_balance)
//             // {
//             //     delete CT_bond_balance;
//             // }
//             // if (PK_bank)
//             // {
//             //     delete PK_bank;
//             // }
//             // if (PK_user)
//             // {
//             //     delete PK_user;
//             // }
//             // if (intermediateHashWires)
//             // {
//             //     delete intermediateHashWires;
//             // }
//             CircuitGenerator::finalize();
//         }

//         void Repay::assignInputs(CircuitEvaluator &circuitEvaluator)
//         {
//             assign_inputs(circuitEvaluator);
//             return;
//         }

//     }
// }

// namespace libsnark
// {
//     // circuit setup
//     CircuitGenerator *create_crv_real_estate_generator(const CircuitArguments &circuit_arguments, const Config &__config)
//     {
//         string hashType = "MiMC7";
//         const string treeHeightKey = "treeHeight";
//         int treeHeight = 0;
//         const string hashTypeKey = "hashType";

//         if (circuit_arguments.find(treeHeightKey) != circuit_arguments.end())
//         {
//             treeHeight = atoi(circuit_arguments.at(treeHeightKey).c_str());
//         }

//         if (circuit_arguments.find(hashTypeKey) != circuit_arguments.end())
//         {
//             hashType = circuit_arguments.at(hashTypeKey);
//         }

//         if (treeHeight != 8 && treeHeight != 16 && treeHeight != 32 && treeHeight != 64)
//         {
//             LOGD("\n\n");
//             LOGD("CreateGenerator     :\n");
//             LOGD("Invalid tree height : %d\n", treeHeight);
//             LOGD("Only support 8, 16, 32, 64 \n");
//             return NULL;
//         }

//         if (hashType.compare("MiMC7") != 0 && hashType.compare("SHA256") != 0 && hashType.compare("Poseidon") != 0)
//         {
//             LOGD("\n\n");
//             LOGD("CreateGenerator   :\n");
//             LOGD("Invalid hash type : %s\n", hashType.c_str());
//             LOGD("Only support MiMC7, SHA256, Poseidon \n");
//             return NULL;
//         }

//         Config config = __config;

//         BigInteger G1_GENERATOR = 0l;
//         if (config.EC_Selection == EC_BLS12_381)
//         {
//             G1_GENERATOR = BigInteger("67c5b5fed18254e8acb66c1e38f33ee0975ae6876f9c5266a883f4604024b3b8", 16);
//         }
//         else if (config.EC_Selection == EC_ALT_BN128)
//         {
//             G1_GENERATOR = BigInteger("16FD271AE0AD87DDAE03044AC6852EE1D2AC024D42CFF099C50EA7510D2A70A5", 16);
//         }

//         config.evaluationQueue_size = 53560;
//         config.inWires_size = 20;
//         config.outWires_size = 0;
//         config.proverWitnessWires_size = 2080;
//         config.hashType = hashType;

//         auto generator = new CircuitBuilder::RealEstate::Repay("RealEstate", treeHeight, G1_GENERATOR, config);
//         generator->generateCircuit();

//         return generator;
//     }

// }