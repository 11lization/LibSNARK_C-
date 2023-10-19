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
#include <Instruction.hpp>
#include <ConstantWire.hpp>

#include "../math/FieldDivisionGadget.hpp"
#include "../math/ModConstantGadget.hpp"
#include "ECGroupGeneratorGadget.hpp"

#include <integer_functions.hpp>

namespace CircuitBuilder {
namespace Gadgets {

    BigInteger ECGroupGeneratorGadget::Global_COEFF_A ;
 
    Wires ECGroupGeneratorGadget::expwire(WirePtr input) {
        ModConstantGadget * mod = allocate<ModConstantGadget>(generator , input, generator->config.CURVE_ORDER);
        modvalue = mod->getOutputWires()[0];
        Wires temp ( modvalue->getBitWires(SECRET_BITWIDTH)->asArray() );
        return temp;
	}
    
    
    
    ECGroupGeneratorGadget::
        ECGroupGeneratorGadget(CircuitGenerator * generator, 
                               WirePtr baseX, WirePtr secret, 
                               string desc) 
        : Gadget(generator , desc), COEFF_A(Global_COEFF_A) , basePoint(0) , outputPublicValue(Wires(2))
    {
        
        secretBits = expwire(secret);
        basePoint =  AffinePoint(baseX);
        checkSecretBits();
        
        // For efficiency reasons, we rely on affine coordinates
        computeYCoordinates(); 
        
        buildCircuit();
    }


    ECGroupGeneratorGadget::
        ECGroupGeneratorGadget(CircuitGenerator * generator, 
                               WirePtr baseX, WirePtr baseY, WirePtr secret, 
                               string desc) 
        : Gadget(generator , desc), COEFF_A(Global_COEFF_A) , basePoint(0) , outputPublicValue(Wires(2)) , evaluator(NULL)
    {
        
        secretBits = expwire(secret);
        basePoint =  AffinePoint(baseX,baseY);
        checkSecretBits();
        
        buildCircuit();
    }


    ECGroupGeneratorGadget::~ECGroupGeneratorGadget(){
        if ( evaluator ) { try{ delete evaluator ; }catch(exception e){} }
    }

    
    void ECGroupGeneratorGadget::buildCircuit() {

        /**
         * The reason this operates on affine coordinates is that in our setting, this's
         * slightly cheaper than the formulas in
         * https://cr.yp.to/ecdh/curve25519-20060209.pdf. Concretely, the following
         * equations save 1 multiplication gate per bit. (we consider multiplications by
         * constants cheaper in our setting, so they are not counted)
         */

        baseTable = preprocess(basePoint); 
        
        AffinePoint output = mul(basePoint, secretBits, baseTable);
         
        outputPublicValue[0] = output.x;
        outputPublicValue[1] = output.y;

    }

    
    void ECGroupGeneratorGadget::checkSecretBits() {
        /**
         * The secret key bits must be of length SECRET_BITWIDTH and are expected to
         * follow a little endian order. The most significant bit should be 1, and the
         * three least significant bits should be zero.
         */
        if (secretBits.size() != SECRET_BITWIDTH) {
            throw invalid_argument("secretBits.size() != SECRET_BITWIDTH");
        }
        
    }



    class Evaluator : public Instruction  {
        ECGroupGeneratorGadget* parent ;
    public:
        Evaluator(CircuitGenerator * generator , ECGroupGeneratorGadget* __parent) 
            : Instruction(generator) , parent(__parent) 
        {}

        void evaluate(CircuitEvaluator & evaluator) {
            BigInteger x = evaluator.getWireValue(parent->basePoint.x);
            evaluator.setWireValue(parent->basePoint.y, parent->computeYCoordinate(x, generator->config.FIELD_PRIME ));
        }
    } ;
    
    void ECGroupGeneratorGadget::computeYCoordinates() {

        // Easy to handle if baseX is constant, otherwise, let the prover input
        // a witness and verify some properties
        if (basePoint.x->instanceof_ConstantWire() ) {
            BigInteger x = ((ConstantWire*) basePoint.x)->getConstant();
            basePoint.y = generator->createConstantWire(computeYCoordinate(x, generator->config.FIELD_PRIME ));
        } else {
            basePoint.y = generator->create_prover_witness_wire("Group.basepoint.y");
            evaluator = new Evaluator (generator, this);
            generator->specifyProverWitnessComputation(evaluator);
        }

    }

    
    void ECGroupGeneratorGadget::assertValidPointOnEC(WirePtr x, WirePtr y) {
        WirePtr ySqr = y->mul(y);
        WirePtr xSqr = x->mul(x);
        WirePtr xCube = xSqr->mul(x);
        generator->addEqualityAssertion(ySqr, xCube->add(xSqr->mul(COEFF_A))->add(x));
    }


    vector<AffinePoint> ECGroupGeneratorGadget::preprocess(AffinePoint p) {
        vector<AffinePoint> precomputedTable (secretBits.size()+1, AffinePoint(0)) ; // = new AffinePoint[secretBits.length+1];
        precomputedTable[0] = p;  
        for (size_t j = 1; j <= secretBits.size(); j += 1) {
            precomputedTable[j] = doubleAffinePoint(precomputedTable[j - 1]);
        }
        return precomputedTable;
    }


    AffinePoint 
    ECGroupGeneratorGadget::mul(AffinePoint p, 
                                Wires & secretBits, 
                                vector<AffinePoint> & precomputedTable)
    {
        UNUSEDPARAM(p)

        AffinePoint result = AffinePoint(precomputedTable[secretBits.size()]);
        
        for (int j = secretBits.size() - 1; j >= 0; j--) {
            AffinePoint tmp = addAffinePoints(result, precomputedTable[j]);
            WirePtr isOne = secretBits.get(j);
            WirePtr tx1 = tmp.x->sub(result.x);
            WirePtr ty1 = tmp.y->sub(result.y);
            WirePtr tx2 = isOne->mul(tx1);
            WirePtr ty2 = isOne->mul(ty1);
            result.x = result.x->add(tx2);
            result.y = result.y->add(ty2);
        }
        
        result = subAffinePoints(result, precomputedTable[secretBits.size()] );
        return result;
    }

    
    AffinePoint ECGroupGeneratorGadget::doubleAffinePoint(AffinePoint p) {
        WirePtr x_2 = p.x->mul(p.x);
        WirePtr tmpx = x_2->mul(3)->add(p.x->mul(COEFF_A)->mul(2))->add(1);
        FieldDivisionGadget * fdg = allocate<FieldDivisionGadget>(generator, tmpx, p.y->mul(2) );
        WirePtr l1 = fdg->getOutputWires().get(0);
        WirePtr l2 = l1->mul(l1);
        WirePtr newX = l2->sub(COEFF_A)->sub(p.x)->sub(p.x);
        WirePtr newY = p.x->mul(3)->add(COEFF_A)->sub(l2)->mul(l1)->sub(p.y);
        return AffinePoint(newX, newY);
    }

    
    AffinePoint ECGroupGeneratorGadget::addAffinePoints(AffinePoint p1, AffinePoint p2) {
        WirePtr diffY = p1.y->sub(p2.y);
        WirePtr diffX = p1.x->sub(p2.x);
        FieldDivisionGadget * fdg = allocate<FieldDivisionGadget>(generator, diffY, diffX );
        WirePtr q = fdg->getOutputWires().get(0);
        WirePtr q2 = q->mul(q);
        WirePtr newX1 = q2->sub(COEFF_A);
        WirePtr newX2 = newX1->sub(p1.x);
        WirePtr newX3 = newX2->sub(p2.x);
        WirePtr newY1 = p1.x->sub(newX3);
        WirePtr newY2 = newY1->mul(q);
        WirePtr newY3 = newY2->sub(p1.y);
        return AffinePoint(newX3, newY3);
    }

    
    AffinePoint ECGroupGeneratorGadget::subAffinePoints(AffinePoint p1, AffinePoint p2) {
        WirePtr negp2y = p2.y->mul(-1l);
        AffinePoint negp2 = AffinePoint(p2.x, negp2y);
        AffinePoint newAffinepoint = addAffinePoints(p1, negp2);
        return newAffinepoint;
    }

    

    Wires & ECGroupGeneratorGadget::getOutputWires() {
        return outputPublicValue ;
    }

    
    BigInteger ECGroupGeneratorGadget::computeYCoordinate(BigInteger & x , const BigInteger & FIELD_PRIME ) {
        BigInteger xSqred = x.multiply(x).mod(FIELD_PRIME);
        BigInteger xCubed = xSqred.multiply(x).mod(FIELD_PRIME);
        BigInteger ySqred = xCubed.add(COEFF_A.multiply(xSqred)).add(x).mod(FIELD_PRIME);
        BigInteger y = IntegerFunctions::ressol(ySqred, FIELD_PRIME);
        return y;
    }


    WirePtr ECGroupGeneratorGadget::getOutputPublicValue() {
        return outputPublicValue[0];
    }


    void ECGroupGeneratorGadget::init_static_members() {
        Global_COEFF_A = BigInteger("126932" , 10 ) ;
    }
}}


