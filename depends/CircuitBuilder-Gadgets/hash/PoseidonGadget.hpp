
#pragma once

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>
#include <fstream>
#include <iostream>
#include <json_tree.hpp>


namespace CircuitBuilder {
namespace Gadgets {

	class PoseidonGadget : public Gadget {

	private:
                static int NUM_ROUNDS_F ;
                static vector<uint8_t> NUM_ROUNDS_P ;
                static vector<vector<vector<BigInteger>>> M ;
                static vector<vector<vector<BigInteger>>> P ;
                static vector<vector<BigInteger>> C ;
                static vector<vector<BigInteger>> S ;

                int t ;
                int nRoundsP ;
                int nRoundsF ;
                vector<BigInteger> c;
                vector<BigInteger> s;
                vector<vector<BigInteger>> m ;
                vector<vector<BigInteger>> p ;
                Wires outWires ;
                Wires state ;

                WirePtr exp5(WirePtr a) ;
                void exp5state() ;
                void ark(int it) ;
                void mix(vector<vector<BigInteger>> m) ;

                static void load_opt_constants() ;
	
	protected:
                void buildCircuit() ;

	public: 
		
		PoseidonGadget(CircuitGenerator * generator, const Wires & inputs, string desc = "") ;

		Wires & getOutputWires() ;

                static void init_static_members() ;
        
	};

}}

