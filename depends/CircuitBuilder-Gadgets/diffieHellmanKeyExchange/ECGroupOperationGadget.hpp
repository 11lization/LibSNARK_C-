 /*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/

/**
 * This gadget implements cryptographic key exchange using a customized elliptic
 * curve that is efficient to represent as a SNARK circuit. It follows the
 * high-level guidelines used for the design of Curve25519, while having the
 * cost model of QAP-based SNARKs in mind. Details in section 6:
 * https://eprint.iacr.org/2015/1093.pdf
 * 
 * Detailed comments about the inputs and outputs of the circuit are below.
 * 
 * Note: By default, this gadget validates only the secret values that are
 * provided by the prover, such as the secret key, and any intermediate
 * auxiliary witnesses that the prover uses in the circuit. In the default mode,
 * the gadget does not check the public input keys, e.g. it does not verify that
 * the base point or the other party's input have the appropriate order, as such
 * inputs could be typically public and can be checked outside the circuit if
 * needed. The Curve25519 paper as well claims that validation is not necessary
 * (although there is debate about some cases online). If any validation is
 * desired, there is a separate method called validateInputs() that do
 * validation, but is not called by default.
 * 
 * 
 * 
 */



#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <CircuitGenerator.hpp>
#include <AffinePoint.hpp>


namespace CircuitBuilder {
namespace Gadgets {

    class ECGroupOperationGadget : public Gadget {
        
    public: 

        static BigInteger Global_COEFF_A ;
        Wires outWires;
    
    private: 
        
        const BigInteger COEFF_A ;


        // The Affine point representation is used as it saves one gate per bit
        AffinePoint basePoint;  // The Base point both parties agree to
        AffinePoint hPoint;     // H is the other party's public value
                                // H = (other party's secret)* Base <- scalar EC
                                // multiplication

        // gadget outputs
        WirePtr outputPublicValue;  // the x-coordinate of the key exchange
                                    // material to be sent to the other party
                                    // outputPublicValue = ((this party's
                                    // secret)*Base).x

        WirePtr xout;
        WirePtr yout;
        Wires outputPubValue ;


        void computeYCoordinates() ;

        // this is only called, when Wire y is provided as witness by the prover
        // (not as input to the gadget)
        void assertValidPointOnEC(WirePtr x, WirePtr y) ;

        AffinePoint addAffinePoints(AffinePoint p1, AffinePoint p2) ;

        Instruction* evaluator_1 ;
        Instruction* evaluator_2 ;
        friend class Evaluator_1 ;
        friend class Evaluator_2 ;

    protected: 
        
        void buildCircuit() ;  
        
    
    public:
    

        /**
         * This gadget receives two points: Base = (baseX) and H = (hX), and the secret
         * key Bits and outputs the scalar EC multiplications: secret*Base, secret*H
         * 
         * The secret key bits must be of length SECRET_BITWIDTH and are expected to
         * follow a little endian order. The most significant bit should be 1, and the
         * three least significant bits should be zero.
         * 
         * This gadget can work with both static and dynamic inputs If public keys are
         * static, the wires of base and h should be made ConstantWires when creating
         * them (before calling this gadget).
         * 
         * 
         */
        ECGroupOperationGadget(CircuitGenerator * generator, 
                               WirePtr X, WirePtr Y, 
                               string desc = "") ;

        ECGroupOperationGadget(CircuitGenerator * generator, 
                               WirePtr pt1_x, WirePtr exp1, 
                               WirePtr pt2_x, WirePtr exp2, 
                               string desc = "" ) ;

        ECGroupOperationGadget(CircuitGenerator * generator, 
                               WirePtr pt1_x, WirePtr pt1_y, WirePtr exp1, 
                               WirePtr pt2_x, WirePtr pt2_y, WirePtr exp2, 
                               string desc = ""); 
    

        ~ECGroupOperationGadget();

        Wires & getOutputWires() ;

        BigInteger computeYCoordinate(const BigInteger & x, const BigInteger & FIELD_PRIME ) ;

        WirePtr getOutputPublicValue() ;

        static void init_static_members() ;

    };
}}
