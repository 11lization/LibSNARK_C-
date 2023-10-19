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

// #include "ReturnFunds.hpp"
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

//         ReturnFunds::ReturnFunds(string circuitName, int __treeHeight, const BigInteger &__G1_GENERATOR, Config &config)
//             : CircuitBuilder::CircuitGenerator(circuitName, config),
//               treeHeight(__treeHeight),
//               G1_GENERATOR(__G1_GENERATOR)
//         {
//         }

//         void ReturnFunds::buildCircuit()
//         {
//             /* statements */
//             CT_creditor_value = createInputWire("CT_creditor_value");
//             CT_debtor_value = createInputWire("CT_debtor_value");


//             // /* witnesses */
//             bidAmount = createProverWitnessWire("bidAmount");
//             value_new_debtor = createProverWitnessWire("value_new_debtor");
//             value_new_creditor = createProverWitnessWire("value_new_creditor");
//             PK_own_debtor = createProverWitnessWire("PK_own_debtor");
//             PK_own_creditor = createProverWitnessWire("PK_own_creditor");
//             r = createProverWitnessWire("r");
//             r1 = createProverWitnessWire("r1");
//             r2 = createProverWitnessWire("r2");


//             vector<WirePtr> nextInputWires;
//             HashGadget *hashGadget;


//             //v'' = v - v'(value_new_debtor = bidAmount - value_new_creditor)
//             WirePtr v_eval = bidAmount->sub(value_new_creditor);
//             addEqualityAssertion(value_new_debtor, v_eval, "invalid value_debtor");

//             return;
//         }

//         void ReturnFunds::finalize()
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

//         void ReturnFunds::assignInputs(CircuitEvaluator &circuitEvaluator)
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

//         auto generator = new CircuitBuilder::RealEstate::ReturnFunds("RealEstate", treeHeight, G1_GENERATOR, config);
//         generator->generateCircuit();

//         return generator;
//     }

// }