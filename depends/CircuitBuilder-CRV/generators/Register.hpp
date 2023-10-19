
/*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>
#include <MiMC7Gadget.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace CircuitBuilder {
namespace CRV {

    class Register : public CircuitBuilder::CircuitGenerator {
    
    private:

        /********************* INPUT ***************************/
        WirePtr HashOut ;
        
        /********************* Witness ***************************/
        WirePtr SK_id;
        
        MiMC7Gadget * MiMC7 ;
        

    public:

        /******************* BigInteger Values  ******************/
        BigInteger sk_id;
        
    protected:
        void buildCircuit() ;

    public: 
        
        Register(string circuitName, Config &config ) ;
        
        void assignInputs(CircuitEvaluator &evaluator) ;
        
    };

}}

 