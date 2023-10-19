/*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/



#include <global.hpp>
#include <Config.hpp>
#include <Wire.hpp>
#include <WireArray.hpp>
#include <Gadget.hpp>
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>

#include "../hash/SubsetSumHashGadget.hpp"
#include "../math/FieldDivisionGadget.hpp"
#include "ECGroupOperationGadget.hpp"
#include "ECGroupGeneratorGadget.hpp"

#include <integer_functions.hpp>

namespace CircuitBuilder {
namespace Gadgets {

    BigInteger ECGroupOperationGadget::Global_COEFF_A  ;
    
    ECGroupOperationGadget::
        ECGroupOperationGadget(CircuitGenerator * generator, 
                               WirePtr X, WirePtr Y, 
                               string desc)
        :   Gadget(generator , desc) ,
            COEFF_A(Global_COEFF_A) ,
            basePoint(X),
            hPoint (Y) ,
            evaluator_1(NULL) , evaluator_2(NULL)
    {
        computeYCoordinates();
        buildCircuit();
    }


    ECGroupOperationGadget::
        ECGroupOperationGadget(CircuitGenerator * generator, 
                               WirePtr pt1_x, WirePtr exp1, 
                               WirePtr pt2_x, WirePtr exp2, 
                               string desc ) 
        :   Gadget(generator , desc) ,
            COEFF_A(Global_COEFF_A) ,
            basePoint(0),
            hPoint (0),
            evaluator_1(NULL) , evaluator_2(NULL)
    {
    
        ECGroupGeneratorGadget * gadget1 = allocate<ECGroupGeneratorGadget>(generator, pt1_x, exp1, desc);  
        ECGroupGeneratorGadget * gadget2 = allocate<ECGroupGeneratorGadget>(generator, pt2_x, exp2, desc);  
        
        basePoint = AffinePoint(gadget1->getOutputWires().get(0));
        hPoint = AffinePoint(gadget2->getOutputWires().get(0));
        computeYCoordinates();
        
        buildCircuit();
    }


    ECGroupOperationGadget::
        ECGroupOperationGadget(CircuitGenerator * generator, 
                               WirePtr pt1_x, WirePtr pt1_y, WirePtr exp1, 
                               WirePtr pt2_x, WirePtr pt2_y, WirePtr exp2, 
                               string desc ) 
        :   Gadget(generator , desc) ,
            COEFF_A(Global_COEFF_A) ,
            basePoint(0),
            hPoint (0),
            evaluator_1(NULL) , evaluator_2(NULL)
    {
    
        ECGroupGeneratorGadget * gadget1 = allocate<ECGroupGeneratorGadget>(generator, pt1_x, pt1_y, exp1, desc);  
        ECGroupGeneratorGadget * gadget2 = allocate<ECGroupGeneratorGadget>(generator, pt2_x, pt2_y, exp2, desc);  
        
        Wires g1_out = gadget1->getOutputWires();
        Wires g2_out = gadget2->getOutputWires();
        basePoint = AffinePoint( g1_out[0] , g1_out[1] );
        hPoint = AffinePoint(g2_out[0] , g2_out[1] );
        
        buildCircuit();
    }



    ECGroupOperationGadget::~ECGroupOperationGadget(){
        if ( evaluator_1 ) { try{ delete evaluator_1 ; }catch(exception e){} }
        if ( evaluator_2 ) { try{ delete evaluator_2 ; }catch(exception e){} }
    }

    
    void ECGroupOperationGadget::buildCircuit() {

        /**
         * The reason this operates on affine coordinates is that in our setting, this's
         * slightly cheaper than the formulas in
         * https://cr.yp.to/ecdh/curve25519-20060209.pdf. Concretely, the following
         * equations save 1 multiplication gate per bit. (we consider multiplications by
         * constants cheaper in our setting, so they are not counted)
         */
        
        AffinePoint out = addAffinePoints(basePoint, hPoint);

        xout = basePoint.x;
        yout = hPoint.x;
        outputPubValue = Wires(2);
        outputPubValue[0] = out.x;
        outputPubValue[1] = out.y;
        outWires = Wires (outputPubValue[0] ,outputPubValue[1] );
        
        return ;        
    }

    

    class Evaluator_1 : public Instruction  {
        ECGroupOperationGadget* parent ;
    public:
        Evaluator_1(CircuitGenerator * generator , ECGroupOperationGadget* __parent) 
        : Instruction(generator) , parent(__parent) {}

        void evaluate(CircuitEvaluator &evaluator) {
            BigInteger x = evaluator.getWireValue(parent->basePoint.x);
            evaluator.setWireValue(parent->basePoint.y, parent->computeYCoordinate(x, parent->generator->config.FIELD_PRIME));
        }
    } ;

    class Evaluator_2 : public Instruction  {
        ECGroupOperationGadget* parent ;
    public:
        Evaluator_2(CircuitGenerator * generator , ECGroupOperationGadget* __parent) 
        : Instruction(generator) , parent(__parent) {}

        void evaluate(CircuitEvaluator &evaluator) {
            BigInteger x = evaluator.getWireValue(parent->hPoint.x);
            evaluator.setWireValue(parent->hPoint.y, parent->computeYCoordinate(x, parent->generator->config.FIELD_PRIME));
        }
    } ;



    void ECGroupOperationGadget::computeYCoordinates() {

        // Easy to handle if pt1_x is constant, otherwise, let the prover input
        // a witness and verify some properties

        if (basePoint.x->instanceof_ConstantWire()) {

            BigInteger x = ((ConstantWire*) basePoint.x)->getConstant();
            basePoint.y = generator->createConstantWire(computeYCoordinate(x, generator->config.FIELD_PRIME));
        } else {
            basePoint.y = generator->create_prover_witness_wire("basepoint.y");
            evaluator_1 = new Evaluator_1(generator, this);
            generator->specifyProverWitnessComputation( evaluator_1 );
            assertValidPointOnEC(basePoint.x, basePoint.y);
        }

        if (hPoint.x->instanceof_ConstantWire() ) {
            BigInteger x = ((ConstantWire*) hPoint.x)->getConstant();
            hPoint.y = generator->createConstantWire(computeYCoordinate(x , generator->config.FIELD_PRIME ));
        } else {
            hPoint.y = generator->create_prover_witness_wire("hpoint.y");
            evaluator_2 = new Evaluator_2(generator, this);
            generator->specifyProverWitnessComputation( evaluator_2 );
            assertValidPointOnEC(hPoint.x, hPoint.y);

        }
    }


    // this is only called, when Wire y is provided as witness by the prover
    // (not as input to the gadget)
    void ECGroupOperationGadget::assertValidPointOnEC(WirePtr x, WirePtr y) {
        WirePtr ySqr = y->mul(y);
        WirePtr xSqr = x->mul(x);
        WirePtr xCube = xSqr->mul(x);
        generator->addEqualityAssertion(ySqr, xCube->add(xSqr->mul(COEFF_A))->add(x));
    }

    

    AffinePoint ECGroupOperationGadget::addAffinePoints(AffinePoint p1, AffinePoint p2) {
        WirePtr diffY = p1.y->sub(p2.y);
        WirePtr diffX = p1.x->sub(p2.x);
        FieldDivisionGadget* fdg = allocate<FieldDivisionGadget>(
                                        generator, diffY, diffX);
        WirePtr q = fdg->getOutputWires().get(0);
        WirePtr q2 = q->mul(q);
        WirePtr q3 = q2->mul(q);
        WirePtr newX = q2->sub(COEFF_A)->sub(p1.x)->sub(p2.x);
        WirePtr newY = p1.x->mul(2l)->add(p2.x)->add(COEFF_A)->mul(q)->sub(q3)->sub(p1.y);
        return AffinePoint(newX, newY);
    }


    Wires & ECGroupOperationGadget::getOutputWires() {
        return outWires ;
    }


    BigInteger ECGroupOperationGadget::computeYCoordinate(const BigInteger & x, const BigInteger & FIELD_PRIME ) {
        BigInteger xSqred = x.multiply(x).mod(FIELD_PRIME);
        BigInteger xCubed = xSqred.multiply(x).mod(FIELD_PRIME);
        BigInteger ySqred = xCubed.add(COEFF_A.multiply(xSqred)).add(x).mod(FIELD_PRIME);
        BigInteger y = IntegerFunctions::ressol(ySqred, FIELD_PRIME);
        return y;
    }

    WirePtr ECGroupOperationGadget::getOutputPublicValue() {
        return outputPublicValue;
    }



    void ECGroupOperationGadget::init_static_members() {
        // parameterization in https://eprint.iacr.org/2015/1093.pdf
        Global_COEFF_A = BigInteger("126932" , 10 ) ;
    }

    
}}
