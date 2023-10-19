

#pragma once

#include <global.hpp>
#include <BaseClass.hpp>
#include <utilities.hpp>

namespace CircuitBuilder {

	class Wire : public Object {

	protected : 

		wireID_t wireId ;
		WireArray * bitWires;
		CircuitGenerator * generator;
		
		WireArray * forceSplit(size_t bitwidth, const string & desc = "" ) ;
		virtual void pack(const string & desc = "" ) ;
	
	public : 

		Wire(CircuitGenerator * generator, int wireId = -1 ) ;
		Wire(CircuitGenerator * generator, WireArray * bits , int wireId = -1 );
		virtual ~Wire(){}

		void packIfNeeded(const string & desc = "" ) const ;
		string toString() const ;
		wireID_t getWireId() const ;

		virtual WireArray * getBitWires() ;
		virtual WireArray * getBitWires(size_t bitwidth, const string & desc = "" ) ;
		WireArray * getBitWiresIfExistAlready();

		virtual void setBits(WireArray * bits) ;
		WirePtr checkNonZero(const string & desc = "" ) ;
		virtual hashCode_t hashCode() const ;
		virtual bool equals( const Object * obj) const ;
		

		// TODO , not all functions below need to be virtual 

		virtual WirePtr power( const WirePtr exp, const string & desc = "" ) ;

		virtual WirePtr mul(long l, const string & desc = "" ) ;
		virtual WirePtr mul(long base, int exp, const string & desc = "" ) ;
		virtual WirePtr mul(const WirePtr w, const string & desc = "" ) ;
		virtual WirePtr mul(const BigInteger &b, const string & desc = "" ) ;
		
		virtual WirePtr add(long v, const string & desc = "" ) ;
		virtual WirePtr add(const BigInteger &b, const string & desc = "" ) ;
		virtual WirePtr add(const WirePtr w, const string & desc = "" ) ;

		virtual WirePtr sub(long v, const string & desc = "" ) ;
		virtual WirePtr sub(const BigInteger &b, const string & desc = "" ) ;
		virtual WirePtr sub(const WirePtr w, const string & desc = "" ) ;
		
		virtual WirePtr OR( const WirePtr w, const string & desc = "" ) ;
		virtual WirePtr XOR( const WirePtr w, const string & desc = "" ) ;
		virtual WirePtr AND( const WirePtr w, const string & desc = "" ) ;

		virtual WirePtr xorBitwise(long v, size_t numBits, const string & desc = "" ) ;
		virtual WirePtr xorBitwise(const  BigInteger &b, size_t numBits, const string & desc = "" ) ;
		virtual WirePtr xorBitwise(const  WirePtr w, size_t numBits, const string & desc = "" ) ;

		virtual WirePtr andBitwise( long v, size_t numBits, const string & desc = "" ) ;
		virtual WirePtr andBitwise( const BigInteger &b, size_t numBits, const string & desc = "" ) ;
		virtual WirePtr andBitwise( const WirePtr w, size_t numBits, const string & desc = "" ) ;
		
		virtual WirePtr orBitwise( long v, size_t numBits, const string & desc = "" ) ;
		virtual WirePtr orBitwise( const BigInteger &b, size_t numBits, const string & desc = "" ) ;
		virtual WirePtr orBitwise( const WirePtr w, size_t numBits, const string & desc = "" ) ;
				
		virtual WirePtr isEqualTo( long v, const string & desc = "" ) ;
		virtual WirePtr isEqualTo( const BigInteger &b, const string & desc = "" ) ;
		virtual WirePtr isEqualTo( const WirePtr w, const string & desc = "" ) ;
		
		virtual WirePtr isLessThanOrEqual( long v, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isLessThanOrEqual( const BigInteger &b, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isLessThanOrEqual( const WirePtr w, size_t bitwidth, const string & desc = "" ) ;

		virtual WirePtr isLessThan( long v, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isLessThan( const BigInteger &b, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isLessThan( const WirePtr w, size_t bitwidth, const string & desc = "" ) ;

		virtual WirePtr isGreaterThanOrEqual( long v, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isGreaterThanOrEqual( const BigInteger &b, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isGreaterThanOrEqual( const WirePtr w, size_t bitwidth, const string & desc = "" ) ;
		
		virtual WirePtr isGreaterThan( long v, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isGreaterThan( const BigInteger &b, size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr isGreaterThan( const WirePtr w, size_t bitwidth, const string & desc = "" ) ;
		
		virtual WirePtr rotateLeft(size_t numBits, size_t s, const string & desc = "" ) ;
		virtual WirePtr rotateRight(size_t numBits, size_t s, const string & desc = "" ) ;
		virtual WirePtr shiftLeft(size_t numBits, size_t s, const string & desc = "" ) ;
		virtual WirePtr shiftRight(size_t numBits, size_t s, const string & desc = "" ) ;

		virtual WirePtr invAsBit(const string & desc = "" ) ;		
		virtual WirePtr invBits(size_t bitwidth, const string & desc = "" ) ;
		virtual WirePtr trimBits(size_t currentNumOfBits, size_t desiredNumofBits, const string & desc = "" ) ;
		virtual void restrictBitLength(size_t bitWidth, const string & desc = "" ) ;

	};
}
