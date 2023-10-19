
#pragma once

#include <global.hpp>
#include <Instruction.hpp>
#include <Wire.hpp>
#include <ConstMulBasicOp.hpp>
#include <CircuitGenerator.hpp>

 
namespace CircuitBuilder {

	class ConstantWire : public Wire {

		protected : BigInteger constant;

	public : 

		ConstantWire(CircuitGenerator * generator, int wireId, BigInteger& value) 
			: Wire(generator, wireId)
		{
			class_id = class_id | Object::ConstantWire_Mask ;
			constant.assign( value.mod(generator->config.FIELD_PRIME) );
		}


		ConstantWire(CircuitGenerator * generator, ConstantWire &other ) 
			: Wire(generator, other.wireId) , constant(other.constant)
		{
			class_id = class_id | Object::ConstantWire_Mask ;
		}


		ConstantWire(CircuitGenerator * generator, ConstantWirePtr other ) 
			: Wire(generator, other->wireId) , constant(other->constant)
		{
			class_id = class_id | Object::ConstantWire_Mask ;
		}
	
		BigInteger &getConstant() {
			return constant;
		}

		bool isBinary() {
			return constant.equals(BigInteger::ONE())
					|| constant.equals(BigInteger::ZERO());
		}

		
		WirePtr mul(long x , const string & desc = "" ) {
			return Wire::mul(x , desc);
		}		

		WirePtr mul( const WirePtr w, const string & desc = "" ) {
			if (w->instanceof_ConstantWire()) {
				BigInteger b = constant.multiply( ((ConstantWire*) w)->constant ) ;
				return generator->createConstantWire(b , desc);
			} else {
				return  w->mul(constant, desc);
			}
		}

	
		WirePtr mul(const BigInteger &b, const string & desc = "" ) {
			WirePtr out;
			bool sign = b.signum() == -1;
			BigInteger newConstant = constant.multiply(b).mod(generator->config.FIELD_PRIME);
			 	
			auto outIt = generator->knownConstantWires.find(newConstant);
			
			if (outIt == generator->knownConstantWires.end() ) {
				
				if(!sign){
					out = allocate<ConstantWire>(generator, generator->currentWireId++, newConstant);
				} else{
					out = allocate<ConstantWire>(generator, generator->currentWireId++, newConstant.subtract(generator->config.FIELD_PRIME));
				}			

				BasicOp* op = allocate<ConstMulBasicOp>(generator, this, out, b, desc);
				Wires * cachedOutputs = generator->addToEvaluationQueue(op);
				if(cachedOutputs == NULL){
					generator->knownConstantWires[newConstant] = out ;
					return out;
				}else{
					generator->currentWireId--;
					generator->deallocate(op) ;
					return cachedOutputs->get(0);
				}
				
			}else{
				out = outIt->second ;
			}

			return out;
		}

		WirePtr checkNonZero( const WirePtr w, const string & desc = "" ) {
			UNUSEDPARAM(w)
			UNUSEDPARAM(desc)
			if (constant.equals(BigInteger::ZERO())) {
				return generator->zeroWire;
			} else {
				return generator->oneWire;
			}
		}

		
		WirePtr invAsBit(const string & desc = "" ) {
			UNUSEDPARAM(desc)
			if (!isBinary()) {
				throw runtime_error("Trying to invert a non-binary constant!");
			}

			if (constant.equals(BigInteger::ZERO())) {
				return generator->oneWire;
			} else {
				return generator->zeroWire;
			}
		}

		WirePtr OR( const WirePtr w, const string & desc = "" ) {
			UNUSEDPARAM(desc)
			if (w->instanceof_ConstantWire()) {
				
				ConstantWirePtr cw = (ConstantWirePtr) w;
				
				if (isBinary() && cw->isBinary()) {
					if (constant.equals(BigInteger::ZERO()) && 
						cw->getConstant().equals(BigInteger::ZERO())) {
						return generator->zeroWire;
					} else {
						return generator->oneWire;
					}
				} else {
					throw runtime_error ("Trying to OR two non-binary constants");
				}
			} else {
				if (constant.equals(BigInteger::ONE())) {
					return generator->oneWire;
				} else {
					return w ;
				}
			}
		}

		WirePtr XOR( const WirePtr w, const string & desc = "" ) {
			
			if (w->instanceof_ConstantWire()) {
				ConstantWirePtr cw = (ConstantWirePtr)w;
				
				if (isBinary() && cw->isBinary()) {
					if (constant.equals(cw->getConstant())) {
						return generator->zeroWire;
					} else {
						return generator->oneWire;
					}
				} else {
					throw runtime_error("Trying to XOR two non-binary constants");
				}
			} else {
				if (constant.equals(BigInteger::ONE())) {
					return w->invAsBit(desc);
				} else {
					return w;
				}
			}
		}

		WireArray * getBitWires(size_t bitwidth, const string & desc = "" ) {
			UNUSEDPARAM(desc)
			
			if (constant.bitLength() > bitwidth) {
				stringstream err_msg ;
				err_msg << "Trying to split a constant of "
						  <<  constant.bitLength() << " bits into " << bitwidth << "bits" ;
				throw runtime_error ( err_msg.str().c_str() );
			} else {
				Wires bits(bitwidth) ; //= vec new ConstantWire[bitwidth];
				for (size_t i = 0; i < bitwidth; i++) {
					bits[i] = constant.testBit(i) ? generator->oneWire : generator->zeroWire;
				}
				return allocate<WireArray>(generator , bits );
			}
		}
		
		void restrictBitLength(size_t bitwidth, const string & desc = "" ) {
			getBitWires(bitwidth, desc);
		}
		

		protected : 
			void pack(const string & desc = ""){
				UNUSEDPARAM(desc)
			}

	};

}
