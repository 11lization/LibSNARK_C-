/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk2889@kookmin.ac.kr>
 *          Thomas Haywood
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


#include "ZKlay.hpp"
#include <DecryptionGadget.hpp>
#include <HashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>


#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace CircuitBuilder {
namespace ZKlay {
    

    ZKlay::ZKlay(string circuitName, int __treeHeight, const BigInteger & __G1_GENERATOR , Config &config) 
        : CircuitBuilder::CircuitGenerator(circuitName , config) , 
          treeHeight(__treeHeight) ,
          G1_GENERATOR(__G1_GENERATOR)
    {}


    void ZKlay::buildCircuit() { 

        /* statements */
        apk = createInputWire("apk");
        rt = createInputWire("rt");
        sn = createInputWire("sn");
        addr = createInputWire("addr"); // PK: pk_0
        k_b = createInputWire("k_b");   //     pk_1
        k_u = createInputWire("k_u");   //     pk_2
        cm_ = createInputWire("cm_");
        cin = createInputWireArray(ctLength, "cin"); // ct
        cout = createInputWireArray(ctLength, "cout"); //ct'
        pv = createInputWire("pv");
        pv_ = createInputWire("pv_");
        G_r = createInputWire("G_r");               // CT: c_0
        K_u = createInputWire("K_u");               //     c_1
        K_a = createInputWire("K_a");               //     c_2
        CT = createInputWireArray(3, "CT");  //     c_3
        G = createConstantWire(G1_GENERATOR);

        /* witnesses */
        sk = createProverWitnessWire("sk");
        cm = createProverWitnessWire("cm");
        du = createProverWitnessWire("du");
        dv = createProverWitnessWire("dv");
        addr_r = createProverWitnessWire("addr_r"); // PK': pk'_0
        k_b_ = createProverWitnessWire("k_b_");     //      pk'_1
        k_u_ = createProverWitnessWire("k_u_");     //      pk'_2
        du_ = createProverWitnessWire("du_");
        dv_ = createProverWitnessWire("dv_");
        r = createProverWitnessWire("r");
        k = createProverWitnessWire("k");
        directionSelector = createProverWitnessWire("direction");                               // Path(cm)
        intermediateHashWires = createProverWitnessWireArray(treeHeight, "intermediateHashes"); //

        vector<WirePtr> nextInputWires;
        HashGadget* hashGadget;

        /* receive a commitment */
        hashGadget = allocate<HashGadget>(this, sk);
		addEqualityAssertion(hashGadget->getOutputWires()[0], k_b, "k_b not equal");

        nextInputWires = {k_b, k_u};
        hashGadget = allocate<HashGadget>(this, nextInputWires);
		addEqualityAssertion(hashGadget->getOutputWires()[0], addr, "addr not equal");

		nextInputWires = { du, dv, addr };
		hashGadget = allocate<HashGadget>(this, nextInputWires);
		addEqualityAssertion(hashGadget->getOutputWires()[0], cm, "cm not equal");

		nextInputWires = { cm, sk };
		hashGadget = allocate<HashGadget>(this, nextInputWires);
        addEqualityAssertion(hashGadget->getOutputWires()[0], sn, "sn not equal");

		// membership check
        Wires leafWires = { cm };
        MerkleTreePathGadget *merkleTreeGadget = allocate<MerkleTreePathGadget>(this, directionSelector, leafWires, *intermediateHashWires, treeHeight , true );
        addOneAssertion(dv->isEqualTo(zeroWire)->add(rt->isEqualTo(merkleTreeGadget->getOutputWires()[0]))->checkNonZero(),
                "membership failed");

        /* send a commitment */
        ECGroupGeneratorGadget *ecGadget = allocate<ECGroupGeneratorGadget>(this, G, r);
        addEqualityAssertion(ecGadget->getOutputWires()[0], G_r, "G_r not equal");

        ecGadget = allocate<ECGroupGeneratorGadget>(this, k_u_, r);
		addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(k), K_u, "K_u' not equal");

		ecGadget = allocate<ECGroupGeneratorGadget>(this, apk, r);
		addEqualityAssertion(ecGadget->getOutputWires()[0]->mul(k), K_a, "K_a' not equal");

        vector<WirePtr> plain = {du_, dv_, addr_r};
        for (int i = 0; i < 3; i++) {
			hashGadget = allocate<HashGadget>(this, k->add(i));
			addEqualityAssertion((*CT)[i], plain[i]->add(hashGadget->getOutputWires()[0]), "ciphertext is not equal");
		}

		nextInputWires = { k_b_, k_u_ };
        hashGadget = allocate<HashGadget>(this, nextInputWires);
		addEqualityAssertion(addr_r, hashGadget->getOutputWires()[0], "addr' is not equal");

		nextInputWires = { du_, dv_, addr_r };
        hashGadget = allocate<HashGadget>(this, nextInputWires);
		addEqualityAssertion(hashGadget->getOutputWires()[0], cm_, "cm' not equal");
        
        /* balance check */
        DecryptionGadget *decGadget = allocate<DecryptionGadget>(this, *cin, sk);
        WirePtr v_in = decGadget->getOutputWires()[0];
        v_in = v_in->mul(((*cin)[0]->add((*cin)[1]))->checkNonZero()); // if ct == 0 then v_in = 0

        decGadget = allocate<DecryptionGadget>(this, *cout, sk);
        WirePtr v_out = decGadget->getOutputWires()[0];

        // WirePtr zeroWire = getZeroWire();

        // pin + v_ - v
        WirePtr v_eval = v_in->add(dv)->sub(dv_)->add(pv)->sub(pv_);
        addEqualityAssertion(v_out, v_eval, "invalid v_out");

        addOneAssertion(dv->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "dv less than 0");
        addOneAssertion(dv_->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "dv_ less than 0");
        addOneAssertion(pv->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "pv less than 0");
        addOneAssertion(pv_->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "pv_ less than 0");
        // assert v >= 0
        addOneAssertion(v_in->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "v less than 0");
        // assert (vout >= 0)
        addOneAssertion(v_out->isGreaterThanOrEqual(zeroWire, config.LOG2_FIELD_PRIME - 1), "v_ less than 0");

        return ;
    }

    void ZKlay::finalize(){
        if(cin){ delete cin ; }
        if(cout){ delete cout ; }
        if(CT) { delete CT ; }
        if(intermediateHashWires) { delete intermediateHashWires ; }
        CircuitGenerator::finalize();
    }
 
    void ZKlay::assignInputs(CircuitEvaluator &circuitEvaluator){
        assign_inputs(circuitEvaluator);
        return ;
    }

}}



namespace libsnark {

    CircuitGenerator* create_crv_zklay_generator (const CircuitArguments & circuit_arguments , const Config & __config ) {
        
        string hashType = "SHA256" ;
        const string treeHeightKey = "treeHeight";
        int treeHeight = 0;
        const string hashTypeKey = "hashType";


        if(circuit_arguments.find(treeHeightKey) != circuit_arguments.end() ){
            treeHeight = atoi ( circuit_arguments.at(treeHeightKey).c_str());
        }

        if(circuit_arguments.find(hashTypeKey) != circuit_arguments.end() ){
            hashType = circuit_arguments.at(hashTypeKey) ;
        }

        if (treeHeight != 8 && treeHeight != 16 && treeHeight != 32 && treeHeight != 64 ) {
            LOGD("\n\n" );
            LOGD("CreateGenerator     :\n" );
            LOGD("Invalid tree height : %d\n", treeHeight);
            LOGD("Only support 8, 16, 32, 64 \n");
            return NULL ;
        }
        
        if (hashType.compare("MiMC7") != 0 && hashType.compare("SHA256") != 0 && hashType.compare("Poseidon") != 0) {
            LOGD("\n\n" );
            LOGD("CreateGenerator   :\n" );
            LOGD("Invalid hash type : %s\n", hashType.c_str());
            LOGD("Only support MiMC7, SHA256, Poseidon \n");
            return NULL ;
        }
        

        Config config  = __config ;
        
        BigInteger G1_GENERATOR = 0l ;
        if ( config.EC_Selection == EC_BLS12_381 ) {
            G1_GENERATOR = BigInteger( "67c5b5fed18254e8acb66c1e38f33ee0975ae6876f9c5266a883f4604024b3b8", 16);    
        } else if ( config.EC_Selection == EC_ALT_BN128 ) {
            G1_GENERATOR = BigInteger( "16FD271AE0AD87DDAE03044AC6852EE1D2AC024D42CFF099C50EA7510D2A70A5", 16);
        }

        config.evaluationQueue_size = 53560;
        config.inWires_size = 20 ;
        config.outWires_size  = 0 ;
        config.proverWitnessWires_size = 2080 ;
        config.hashType = hashType ;

        auto generator = new CircuitBuilder::ZKlay::ZKlay( "ZKlay", treeHeight , G1_GENERATOR , config);
        generator->generateCircuit();

        return generator ;
    }

}