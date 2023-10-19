/*******************************************************************************
 * Authors: Jaekyoung Choi <cjk2889@kookmin.ac.kr>
 * 			Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>

namespace CircuitBuilder {
namespace DID {

	class DID : public CircuitBuilder::CircuitGenerator {
	
	private:

		/********************* INPUT ***************************/
		WirePtr iPk, bHash, sName, sYear, sUniv, sDegree, sMajor, sSN;

		/********************* Witness ***************************/
		WirePtr wName, wYear, wUniv, wDegree, wMajor, wSN;

	public:

		 static const int EXPONENT_BITWIDTH = 254; // in bits

	protected:

		void buildCircuit() ;

	public: 
		
		DID(string circuitName, Config &config) ;
		
		void assignInputs(CircuitEvaluator &evaluator) ;
		void generateSampleInput(CircuitEvaluator &eval) ;
		void finalize();
			
	};

}}
