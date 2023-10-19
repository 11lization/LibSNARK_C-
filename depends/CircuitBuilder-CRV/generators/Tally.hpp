
/*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/

 
#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>


namespace CircuitBuilder {
namespace CRV {

    class Tally : public CircuitBuilder::CircuitGenerator {
    
    
    private:

        /********************* INPUT ***************************/
        WirePtr Gx , Gy ;
        WirePtr Ux , Uy;
        WirePtr Vsum_x , Vsum_y;
        WirePtr Wsum_x , Wsum_y;
        WirePtr msgsum; 
        
        /********************* Witness ***************************/
        WirePtr SK;

        
    public:

        static const int EXPONENT_BITWIDTH = 254; // in bits

    private:

        Wires expwire(WirePtr input) ;
        
    protected:

        void buildCircuit() ;

    public: 

        Tally(string circuitName, Config &config) ;

        void assignInputs(CircuitEvaluator &evaluator) ;

    };

}}

 