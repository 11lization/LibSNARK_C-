

#pragma once

#include <gmp.h>
#include <math.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <ostream>


using namespace std ;

#ifdef  INITIAL_BIT_ALLOCATION
#define MPZ_INIT_BITCNT INITIAL_BIT_ALLOCATION
#else
#define MPZ_INIT_BITCNT 256 
#endif


class BigInteger ;
typedef BigInteger* BigIntegerPtr ;
typedef std::vector<BigIntegerPtr> BigIntegers ;




class local_string_buffer {

private:
	#define local_string_buffer_stack_size 1024 
	char local [local_string_buffer_stack_size] ;
	char* allocated ;
	char* buf ;
	size_t m_size ;

public:

	local_string_buffer( size_t size , bool set = false , char with = '\0' ){
		
		m_size = size + 2 ;

		if ( m_size > local_string_buffer_stack_size  ){
			allocated = (char*) malloc (m_size) ;
			buf = allocated ;
		}else{
			allocated = NULL ;
			buf = local ;
		}

		if (  set ){
			memset( buf , with , m_size ) ;
		}
	
	}

	~local_string_buffer( ){
		if (allocated){ free(allocated) ; }
	}

	char* buffer() { return buf ; }

	size_t size() { return m_size ; }

};




class BigInteger {
	
private:
	
	mpz_t mpz ;
	
	int check_radix (int radix) const {
		if ( (radix == 2) || (radix == 10) || (radix == 16)){
			return radix ;
		}else{
			throw invalid_argument( " Working with base 2, 10, and 16  ");
		}
	}

	typedef struct { int unused ; } no_assignment ;

	BigInteger( const no_assignment & no_assign ) {
		(void)(no_assign);
		mpz_init2(mpz, MPZ_INIT_BITCNT );
	}

public :

	inline void assign ( const mpz_t __mpz ) {
		mpz_set(mpz , __mpz);
	}

	inline void assign( const BigInteger &b ){
		mpz_set(mpz , b.mpz);
	}

	inline void assign( const char * value_str , int radix = 10 ){
		int base = check_radix(radix);
		mpz_set_str(mpz , value_str , base );
	}

	inline void assign ( const string & value_str , int radix = 10 ){
		int base = check_radix(radix);
		mpz_set_str(mpz , value_str.c_str() , base );
	}

	inline void assign ( signed long int value){
		mpz_set_si(mpz , value );
	}

	inline void assign ( unsigned long int value){
		mpz_set_ui(mpz , value );
	}


	//
	// Constructors 
	//
	BigInteger() {
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign( 0ul );
	}

	BigInteger(const mpz_t src ) {
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign(src);
	}

	BigInteger( const BigInteger &src ) {
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign(src);
	}

	BigInteger( const char * value_str , int radix = 10 ){
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign ( value_str , radix );
	}

	BigInteger( const string & value_str , int radix = 10 ){
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign ( value_str , radix );
	}

	BigInteger( signed long int src){
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign ( src );
	}

	BigInteger( unsigned long int src){
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign ( src );
	}

	#if __SIZEOF_SIZE_T__ == 4
	BigInteger( ssize_t src){
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign ( (signed long int) src );
	}
	
	BigInteger( size_t src){
		mpz_init2(mpz, MPZ_INIT_BITCNT );
		assign ( (unsigned long int)src );
	}
	#endif
	
	~BigInteger(){
		mpz_clear(mpz);
	}



	//
	// arithmetic functions
	// 

	void __negate(){
		BigInteger temp(mpz) ;
		mpz_neg( mpz , temp.mpz );
	}

	BigInteger negate(){
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_neg( ret_val.mpz , mpz );
		return ret_val ;
	}


	void __add(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_add( mpz , temp.mpz , other.mpz );
	}

	BigInteger add(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_add( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger add(signed long other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other);
		mpz_add( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}

	BigInteger add(unsigned long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_add_ui( ret_val.mpz , mpz , other );
		return ret_val ;
	}



	void __subtract(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_sub( mpz , temp.mpz , other.mpz );
	}

	BigInteger subtract(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_sub( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger subtract(signed long other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other);
		mpz_sub( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}

	BigInteger subtract(unsigned long other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_sub_ui( ret_val.mpz , mpz , other );
		return ret_val ;
	}



	void __multiply(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_mul( mpz , temp.mpz , other.mpz );
	}

	BigInteger multiply(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_mul( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger multiply(signed long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_mul_si( ret_val.mpz , mpz , other );
		return ret_val ;
	}

	BigInteger multiply(unsigned long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_mul_ui( ret_val.mpz , mpz , other );
		return ret_val ;
	}



	void __fdivide(const BigInteger &d ) {
		BigInteger temp(mpz) ;
		mpz_fdiv_q( mpz , temp.mpz , d.mpz );
	}

	BigInteger fdivide(const BigInteger &d ) const {
		no_assignment no_assign ;
		BigInteger quotient(no_assign) ;
		mpz_fdiv_q( quotient.mpz , mpz , d.mpz );
		return quotient ;
	}

	BigInteger fdivide(signed long int d ) const {
		no_assignment no_assign ;
		BigInteger quotient(no_assign) ;
		BigInteger __d(d);
		mpz_fdiv_q( quotient.mpz , mpz , __d.mpz );
		return quotient ;
	}

	BigInteger fdivide(unsigned long int d ) const {
		no_assignment no_assign ;
		BigInteger quotient(no_assign) ;
		mpz_fdiv_q_ui( quotient.mpz , mpz , d );
		return quotient ;
	}



	void __remainder(const BigInteger &d) {
		BigInteger temp(mpz) ;
		mpz_fdiv_r( mpz , temp.mpz , d.mpz );
	}

	BigInteger remainder(const BigInteger & d ) const {
		no_assignment no_assign ;
		BigInteger r(no_assign) ;
		mpz_fdiv_r( r.mpz , mpz , d.mpz );
		return r ;
	}



	void __mod(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_mod( mpz , temp.mpz , other.mpz );
	}

	BigInteger mod(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_mod( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger mod(signed long int d ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger __d(d);
		mpz_mod( ret_val.mpz , mpz , __d.mpz );
		return ret_val ;
	}

	BigInteger mod(unsigned long int d ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_mod_ui( ret_val.mpz , mpz , d );
		return ret_val;
	}


	BigInteger modInverse(const BigInteger &mod ) {
		if( this->intValue() == 0) {
			// LOGD("Error in modInverse --- assigned value is zero, operation is ignored ");
			return *this;
		}
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger exp(-1l) ;
		mpz_powm( ret_val.mpz , mpz , exp.mpz , mod.mpz );
		return ret_val ;
	}


	void __pow( unsigned long exp ) {
		BigInteger base(mpz) ;
		mpz_pow_ui(mpz , base.mpz , exp );
	}

	BigInteger pow(unsigned long exp ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_pow_ui(ret_val.mpz , mpz , exp );
		return ret_val ;
	}

	

	void __modPow(const BigInteger &exp , const BigInteger &mod ) {
		BigInteger base(mpz) ;
		mpz_powm( mpz , base.mpz , exp.mpz , mod.mpz );
	}

	BigInteger modPow(const BigInteger &exp , const BigInteger &mod ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_powm( ret_val.mpz , mpz , exp.mpz , mod.mpz );
		return ret_val ;
	}

	BigInteger modPow( signed long exp , unsigned long int mod)  const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger __exp(exp) ;
		BigInteger __mod(mod) ;
		mpz_powm( ret_val.mpz , mpz , __exp.mpz , __mod.mpz );
		return ret_val ;
	}

	BigInteger modPow(unsigned long int exp , unsigned long int mod) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger __mod(mod) ;
		mpz_powm_ui( ret_val.mpz , mpz , exp , __mod.mpz );
		return ret_val ;
	}




	void __OR(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_ior( mpz , temp.mpz , other.mpz );
	}

	BigInteger OR(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_ior( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger OR(signed long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other) ;
		mpz_ior( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}

	BigInteger OR(unsigned long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other) ;
		mpz_ior( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}



	void __XOR(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_xor( mpz , temp.mpz , other.mpz );
	}

	BigInteger XOR(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_xor( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger XOR(signed long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other) ;
		mpz_xor( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}

	BigInteger XOR(unsigned long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other) ;
		mpz_xor( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}



	void __AND(const BigInteger &other ) {
		BigInteger temp(mpz) ;
		mpz_and( mpz , temp.mpz , other.mpz );
	}

	BigInteger AND(const BigInteger &other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		mpz_and( ret_val.mpz , mpz , other.mpz );
		return ret_val ;
	}

	BigInteger AND(signed long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other) ;
		mpz_and( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}

	BigInteger AND(unsigned long int other ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		BigInteger op2(other) ;
		mpz_and( ret_val.mpz , mpz , op2.mpz );
		return ret_val ;
	}
	
 


private:

	void __shiftLeft( const BigInteger & src , size_t i ) {

		if ( i == 0 ) { 
			mpz_set( mpz , src.mpz );
			return ; 
		}
		
		const size_t src_bit_count = mpz_sizeinbase(src.mpz , 2 );
		const size_t dst_bit_count = src_bit_count + i ;
		
		//
		//  Converting to base 2 string , extend lsb , then back to big int 
		//
		local_string_buffer local_buf(dst_bit_count, true , '0') ;
		char * buf = local_buf.buffer() ;
		
		mpz_get_str ( buf , 2 , src.mpz ) ;
		
		// extent lsb
		buf[src_bit_count] = '0' ;
		buf[dst_bit_count] = '\0' ; 
		
		mpz_set_str( mpz , buf , 2 );

		return ; 
	}

	void __shiftRight( const BigInteger & src , size_t i ) {
		
		if ( i == 0 ) { 
			mpz_set( mpz , src.mpz );
			return ; 
		}
		
		size_t bit_count = mpz_sizeinbase(src.mpz , 2 );
		
		if ( i >= bit_count ) {
			mpz_set_ui( mpz , 0 );		
			return ; 
		}

		//
		//  Converting to base 2 string , truncate lsb , then back to big int 
		//
		local_string_buffer local_buf(bit_count , true ) ;
		char * buf = local_buf.buffer() ;

		mpz_get_str ( buf , 2 , src.mpz ) ;
		buf[(bit_count - i)] = '\0' ; // truncate lsb 
		mpz_set_str( mpz , buf , 2 );

	}


public:

	void __shiftLeft(size_t i ) {
		BigInteger src(mpz) ;
		__shiftLeft( src , i ) ;
	}

	BigInteger shiftLeft(size_t i ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		ret_val.__shiftLeft( *this , i ) ;
		return ret_val ;
	}

	void __shiftRight(size_t i ) {
		BigInteger src(mpz) ;
		__shiftRight( src , i ) ;
	}

	BigInteger shiftRight(size_t i ) const {
		no_assignment no_assign ;
		BigInteger ret_val(no_assign) ;
		ret_val.__shiftRight( *this , i ) ;
		return ret_val ;
	}



	


	//
	// Member functions
	//
	
	int signum() const {
		return mpz_sgn(mpz) ;
	}


	int intValue() const {
		return (int) mpz_get_ui(mpz);
	}

	const mpz_t & get_mpz() const {
		return mpz ;
	}

	bool testBit( int i ) const {
		return mpz_tstbit(mpz , i ) ;
	}
		
	
	bool isBinary() const {
		return equals(BigInteger::ZERO() ) || equals(BigInteger::ONE() ) ;
	}

	bool equals(const BigInteger &other) const {
		return (mpz_cmp(mpz , other.mpz ) == 0 );
	}
	
	bool equals(const BigIntegerPtr other) const {
		return (mpz_cmp(mpz , other->mpz ) == 0 );
	}

	bool equals( signed long other) const {
		return (mpz_cmp(mpz , BigInteger(other).mpz ) == 0 );
	}

	bool equals( unsigned long other) const {
		return (mpz_cmp(mpz , BigInteger(other).mpz ) == 0 );
	}
	


private:
	
	int __adjust_compareTo_rtn__(int i ) const {
		if(i < 0 ) { return -1 ; }
		else if ( i > 0 ) { return 1 ;}
		else { return 0 ;} 
	}

public:

	int compareTo( const BigInteger &rhs ) const {
		return __adjust_compareTo_rtn__( mpz_cmp(mpz , rhs.mpz) ) ;
	}

	int compareTo( signed long int rhs ) const {
		return __adjust_compareTo_rtn__( mpz_cmp_si (mpz , rhs ) ) ;
	}

	int compareTo( unsigned long int rhs ) const {
		return __adjust_compareTo_rtn__( mpz_cmp_ui(mpz , rhs ) ) ;
	}

	
	size_t size_in_base(int radix){
		return mpz_sizeinbase(mpz , radix ) ;
	}


	size_t bitLength() const {			
		
		//
		// 	basing on Java BigInteger reference manual 
		//   " Computes (ceil(log2(this < 0 ? -this : this+1))). "
		//   @ { https://docs.oracle.com/javase/8/docs/api/java/math/BigInteger.html#bitLength-- }
		//
		BigInteger temp (mpz) ;
		if ( mpz_sgn(mpz) < 0 ){
			temp.__negate() ;
		}else{
			temp.__add(1l) ;
		}

		int log2_temp = mpz_sizeinbase(temp.mpz , 2 ) ;

		return (size_t)(ceil(log2_temp)) ;
	}


	vector<uint8_t> toByteArray() const {
		
		if (signum() == -1){
			std::string errMsg;
			errMsg.append("Negative integers not supported [");
			errMsg.append(toString(10));
			errMsg.append("]");
			throw invalid_argument(errMsg );
		}

		size_t word_size = 8 ;
		size_t count = (mpz_sizeinbase (mpz , 2) + word_size-1) / word_size;
		vector<uint8_t> ret_val(count);
		mpz_export(ret_val.data(), NULL , 1 , 1 , 0 , 0, mpz );
		return ret_val ;
	}


	

	//
	// formated outputs
	// 

	void print (FILE *stream){
		mpz_out_str(stream , 10 , mpz);
	}

	void print_hex (FILE *stream){
		mpz_out_str(stream , 16 , mpz);
	}

	string toString(int radix = 10 ) const {
		
		check_radix(radix);

		size_t temp_size = mpz_sizeinbase(mpz , radix ) ;
		
		local_string_buffer local_buf( temp_size ) ;
		char * buf = local_buf.buffer() ;
 
		mpz_get_str ( buf , radix , mpz ) ;

		return string (buf) ;
	}

	string toHexString() const { return toString(16) ; }
		

	

	//
	// operators
	//

	BigInteger& operator= ( const BigInteger & rhs )	{ assign ( rhs ); return *this ; }
	BigInteger& operator= ( const BigIntegerPtr rhs)	{ assign ( *rhs); return *this ; }
	BigInteger& operator= ( signed long int rhs)		{ assign ( rhs ); return *this ; }
	BigInteger& operator= ( unsigned long int rhs)		{ assign ( rhs ); return *this ; }
	
	BigInteger& operator++(){ __add( 1ul ) ; return *this ; }
	BigInteger& operator--(){ __subtract( 1ul ) ; return *this ; }

	bool operator<( const BigInteger & rhs ) const 		{ return (compareTo(rhs) == -1 ) ; }
	bool operator<( const BigIntegerPtr rhs) const 		{ return (compareTo(*rhs) == -1 ) ; }
	bool operator<( signed long int rhs )    const 		{ return (compareTo(rhs) == -1 ) ; }
	bool operator<( unsigned long int rhs )  const 		{ return (compareTo(rhs) == -1 ) ; }
	
	bool operator>( const BigInteger & rhs ) const 		{ return (compareTo(rhs) == 1 ) ; }
	bool operator>( const BigIntegerPtr rhs) const 		{ return (compareTo(*rhs) == 1 ) ; }
	bool operator>( signed long int rhs )    const 		{ return (compareTo(rhs) == 1 ) ; }
	bool operator>( unsigned long int rhs )  const 		{ return (compareTo(rhs) == 1 ) ; }
	
	BigInteger operator+ ( const BigInteger & rhs )	const { return add( rhs ) ; }
	BigInteger operator+ ( const BigIntegerPtr rhs) const { return add( * rhs ) ; }
	BigInteger operator+ ( signed long int rhs )	const { return add( rhs ) ; }
	BigInteger operator+ ( unsigned long int rhs )	const { return add( rhs ) ; }

	BigInteger operator- ( const BigInteger & rhs )	const { return subtract( rhs ) ;  }
	BigInteger operator- ( const BigIntegerPtr rhs) const { return subtract( * rhs ) ;  }
	BigInteger operator- ( signed long int rhs )	const { return subtract( rhs ) ;  }
	BigInteger operator- ( unsigned long int rhs )	const { return subtract( rhs ) ;  }

	BigInteger operator* ( const BigInteger & rhs )	const { return multiply( rhs ) ;  }
	BigInteger operator* ( const BigIntegerPtr rhs) const { return multiply( * rhs ) ;  }
	BigInteger operator* ( signed long int rhs )	const { return multiply( rhs ) ;  }
	BigInteger operator* ( unsigned long int rhs )	const { return multiply( rhs ) ;  }




	//
	// 	random number generation
	//
	class RandState{
	private:
		gmp_randstate_t state ;
		friend class BigInteger ;
	public:
		RandState(){}
		void reset() { gmp_randinit_default (state) ; }
		~RandState() { gmp_randclear(state) ; }
	};


	BigInteger( size_t bitcnt , RandState & rand ){
		mpz_init2(mpz, bitcnt );
		mpz_urandomb( mpz , rand.state, bitcnt);
	}


	//
	// Static members
	//
	static const BigInteger ZERO() { return BigInteger(0ul) ;}
	static const BigInteger ONE()  { return BigInteger(1ul) ;}
	static const BigInteger TWO()  { return BigInteger(2ul) ;}
	static const BigInteger FOUR() { return BigInteger(4ul) ;}

	static void set_mpz( mpz_t dst , const BigInteger & src ) {
		mpz_set ( dst , src.mpz ) ;
	}

	static void set_mpz( mpz_t dst , mpz_srcptr src ) {
		mpz_set ( dst , src ) ;
	}

};


static inline std::ostream& operator<<(std::ostream &out, const BigInteger  &b){
	out << b.toString();
	return out ;
}
