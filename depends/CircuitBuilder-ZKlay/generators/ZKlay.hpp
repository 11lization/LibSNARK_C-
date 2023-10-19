/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk2889@kookmin.ac.kr>
 * 			Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>


namespace CircuitBuilder {
namespace ZKlay {

	class ZKlay : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
        WirePtr apk, rt, sn, addr, k_b, k_u, cm_; 
		WiresPtr cin, cout, CT;
        WirePtr pv, pv_, G_r, K_u, K_a, G;

		/********************* Witness ***************************/
        WirePtr sk, cm, du, dv, addr_r, k_b_, k_u_, du_, dv_, r, k;
        

		/********************* MerkleTree ***************************/
		WirePtr directionSelector;
		WiresPtr intermediateHashWires;
		static const int CTLength = 3;
		static const int ctLength = 2;
		static const int pkLength = 3;
		int treeHeight;
		BigInteger G1_GENERATOR;

	protected:

		void buildCircuit() ;

		void finalize();

	public: 

		ZKlay(string circuitName, int treeHeight , const BigInteger & G1_GENERATOR , Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

	};

}}