/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk2889@kookmin.ac.kr>
 * 			Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>

namespace CircuitBuilder {
namespace CRV {

	class Vote : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
		WirePtr Gx, Gy;
		WirePtr Ux, Uy;
		WirePtr Vx_in, Wx_in;
		WirePtr Vy_in, Wy_in;
		WirePtr E_id;
		WirePtr root_in;
		WirePtr sn_in;
		WirePtr root_out;


		/********************* Witness ***************************/
		WirePtr Sx, Sy;
		WirePtr Tx, Ty;
		WirePtr sk_id;
		WirePtr msg; 
		WirePtr randomizedEnc;
		

		/********************* MerkleTree ***************************/
		WirePtr directionSelector;
		WiresPtr intermediateHashWires;
		int treeHeight;

	public:

		 static const int EXPONENT_BITWIDTH = 254; // in bits

	protected:

		void buildCircuit() ;

	public: 
		
		Vote(string circuitName, Config &config , int treeHeight) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;

		void finalize();
			
	};

}}
