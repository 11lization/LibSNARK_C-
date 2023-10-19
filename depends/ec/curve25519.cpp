
#include <sstream>

#include <misc.hpp>
#include <integer_functions.hpp>
#include <curve25519.hpp>

#include <logging.hpp>



namespace EC {

    size_t Curve25519::BIT_WIDTH = 254;
    BigInteger Curve25519::FIELD_PRIME = BigInteger ("21888242871839275222246405745257275088548364400416034343698204186575808495617" , 10 ) ; 
    BigInteger Curve25519::CURVE_ORDER = BigInteger ("21888242871839275222246405745257275088597270486034011716802747351550446453784" , 10 ) ;
    BigInteger Curve25519::SUBGROUP_ORDER = BigInteger ("2736030358979909402780800718157159386074658810754251464600343418943805806723" , 10 );
    BigInteger Curve25519::COEFF_A = BigInteger("126932" , 10 ) ;

    Curve25519::Curve25519 () : pt () {}

    Curve25519::Curve25519 ( const AffinePoint &_pt ) : pt ( _pt ) {}
    
    Curve25519::Curve25519 ( const BigInteger &x , const BigInteger &y ) : pt(x , y) {}
    
    Curve25519::Curve25519 ( const BigInteger &x , bool isMatchRessol ) : pt() {
        pt.x = x ;
        pt.y = isMatchRessol ? Curve25519::computeYCoordinate(x): 
                               (Curve25519::computeYCoordinate(x).negate()).mod(FIELD_PRIME);
    }

    Curve25519 Curve25519::add ( const Curve25519 &other ) const {
        AffinePoint new_p = addAffinePoints( pt, other.getPoint());
        return Curve25519(new_p);
    }


    Curve25519 Curve25519::sub ( const Curve25519 &other ) const {
        AffinePoint new_p = subAffinePoints( pt, other.getPoint() );
        return Curve25519(new_p);
    }

    Curve25519 Curve25519::mul( const BigInteger &exp ) const {
        vector<size_t> secretBits = makesecretbits(exp) ;
        vector<AffinePoint> baseTable = preprocess( pt , secretBits );
        AffinePoint new_p = multiply(baseTable, secretBits);
        return Curve25519(new_p);
    }


    BigInteger Curve25519::FieldDivision( const BigInteger &a, const BigInteger &b ) const {
        BigInteger b_ = b ;
        return (a.multiply(b_.modInverse(FIELD_PRIME)).mod(FIELD_PRIME));
    }

    

    AffinePoint Curve25519::multiply( const vector<AffinePoint> & precomputedTable , const vector<size_t> & secretBits )  const {
        
        AffinePoint result = AffinePoint(precomputedTable[secretBits.size()]);
        
        for (int j = ((int)secretBits.size()) - 1; j >= 0; j--) {
            
            AffinePoint tmp = addAffinePoints(result, precomputedTable[j]);
            BigInteger isOne = secretBits[j];
            result.x = (result.x.add(isOne.multiply(tmp.x.subtract(result.x)))).mod(FIELD_PRIME);
            result.y = (result.y.add(isOne.multiply(tmp.y.subtract(result.y)))).mod(FIELD_PRIME);
        
        }
        
        result = subAffinePoints(result, precomputedTable[ secretBits.size() ]);

        return result;
    }


    AffinePoint Curve25519::addAffinePoints( const AffinePoint &p1 , const AffinePoint &p2 ) const {
        
        BigInteger two = BigInteger(2l);
        BigInteger diffY = p1.y.subtract(p2.y).mod(FIELD_PRIME);
        BigInteger diffX = p1.x.subtract(p2.x).mod(FIELD_PRIME);
        BigInteger q = FieldDivision(diffY, diffX);

        BigInteger q2 = q.multiply(q);
        BigInteger q3 = q2.multiply(q);
        BigInteger newX = q2.subtract(COEFF_A).subtract(p1.x).subtract(p2.x).mod(FIELD_PRIME);
        BigInteger newY = p1.x.multiply(two).add(p2.x).add(COEFF_A).multiply(q).subtract(q3).subtract(p1.y).mod(FIELD_PRIME);
        return AffinePoint(newX, newY);
    
    }


    AffinePoint Curve25519::subAffinePoints( const AffinePoint &p1, const AffinePoint &p2) const {
        
        BigInteger two = BigInteger("2");
        
        BigInteger diffY = p1.y.add(p2.y).mod( FIELD_PRIME );
        BigInteger diffX = p1.x.subtract(p2.x).mod(FIELD_PRIME);
        
        BigInteger q = FieldDivision(diffY, diffX);

        BigInteger q2 = q.multiply(q);
        BigInteger q3 = q2.multiply(q);
        BigInteger newX = q2.subtract(COEFF_A).subtract(p1.x).subtract(p2.x).mod(FIELD_PRIME);
        BigInteger newY = p1.x.multiply(two).add(p2.x).add(COEFF_A).multiply(q).subtract(q3).subtract(p1.y).mod(FIELD_PRIME);

        return AffinePoint(newX, newY);
    }


    vector<size_t> Curve25519::makesecretbits( const BigInteger &input ) const {
        BigInteger qr = input.remainder(SUBGROUP_ORDER);
        return MISC::zeroPad( MISC::split( qr ) , BIT_WIDTH );
    }


    AffinePoint Curve25519::doubleAffinePoint( const AffinePoint &p) const {
       
        BigInteger three = BigInteger(3l);
        BigInteger two = BigInteger(2l);
        BigInteger x_2 = p.x.multiply(p.x).mod(FIELD_PRIME);
        BigInteger l1 = FieldDivision(x_2.multiply(three).add(p.x.multiply(COEFF_A).multiply(two)).add(BigInteger::ONE()), p.y.multiply(two));
        BigInteger l2 = l1.multiply(l1);
        BigInteger newX = l2.subtract(COEFF_A).subtract(p.x).subtract(p.x);
        BigInteger newY = p.x.multiply(three).add(COEFF_A).subtract(l2).multiply(l1).subtract(p.y);

        return AffinePoint(newX.mod(FIELD_PRIME), newY.mod(FIELD_PRIME));
    }


    vector<AffinePoint> Curve25519::preprocess( const AffinePoint &p, const vector<size_t> &secretBits ) const {
        
        vector<AffinePoint> precomputedTable = vector<AffinePoint>( secretBits.size() + 1 );

        precomputedTable[0] = p;

        for (size_t j = 1; j <= secretBits.size() ; j += 1) {
            precomputedTable[j] = doubleAffinePoint(precomputedTable[j - 1]);
        }

        return precomputedTable;
    }


    BigInteger Curve25519::computeYCoordinate( const BigInteger &x ) {
        BigInteger xSqred = (x.multiply(x)).mod(FIELD_PRIME);
        BigInteger xCubed = (xSqred.multiply(x)).mod(FIELD_PRIME);
        BigInteger ySqred = (xCubed.add(COEFF_A.multiply(xSqred)).add(x)).mod(FIELD_PRIME);
        return (  IntegerFunctions::ressol(ySqred, FIELD_PRIME)).mod(FIELD_PRIME);
    }
    

    int Curve25519::isMatchRessol( ){
        return Curve25519::isMatchRessol ( pt.x , pt.y ) ;
    }

    int Curve25519::isMatchRessol( const BigInteger &x , const BigInteger &y ){
        return (computeYCoordinate(x)).equals((y)) ? 1 : 0 ;
    }


    std::string Curve25519::toString ( const std::string & label , size_t radix ){
        stringstream sstr ;
        sstr << label << " = x : " << pt.x.toString(radix) << " , y : " << pt.y.toString(radix) ;
        return sstr.str() ;    
    }


}








 

#ifdef __cplusplus
extern "C" {
#endif


    void Curve25519_init ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , int isMatch_ressol  ){
        EC::Curve25519 self = EC::Curve25519( BigInteger(self_x) , (isMatch_ressol == 1 ) ) ;
        BigInteger::set_mpz ( dst_x , self.PointX() ) ;
        BigInteger::set_mpz ( dst_y , self.PointY() ) ;
    }


    void Curve25519_add ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , mpz_ptr self_y , mpz_ptr other_x , mpz_ptr other_y) {
        
        EC::Curve25519 self = EC::Curve25519( BigInteger(self_x) ,  BigInteger(self_y) ) ;
        EC::AffinePoint other = EC::AffinePoint( BigInteger ( other_x ) , BigInteger ( other_y ) ) ;
        EC::Curve25519 ret_val ;
        
        ret_val = self.add( other ) ; 

        BigInteger::set_mpz ( dst_x , ret_val.PointX() ) ;
        BigInteger::set_mpz ( dst_y , ret_val.PointY() ) ;
        return ;
    }


    void Curve25519_sub ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , mpz_ptr self_y , mpz_ptr other_x , mpz_ptr other_y ) {
        
        EC::Curve25519 self = EC::Curve25519( BigInteger(self_x) ,  BigInteger(self_y) ) ;
        EC::AffinePoint other = EC::AffinePoint( BigInteger ( other_x ) , BigInteger ( other_y ) ) ;
        EC::Curve25519 ret_val ;
        
        ret_val = self.sub( other ) ; 
        
        BigInteger::set_mpz ( dst_x , ret_val.PointX() ) ;
        BigInteger::set_mpz ( dst_y , ret_val.PointY() ) ;
        return ;
    }


    void Curve25519_mul ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , mpz_ptr self_y , mpz_srcptr exp_mpz ) {
        
        EC::Curve25519 self = EC::Curve25519( BigInteger(self_x) ,  BigInteger(self_y) ) ;
        BigInteger exp = BigInteger ( exp_mpz ) ;
        
        EC::Curve25519 ret_val = self.mul(exp) ;

        BigInteger::set_mpz ( dst_x , ret_val.PointX() ) ;
        BigInteger::set_mpz ( dst_y , ret_val.PointY() ) ;
    }


    int Curve25519_isMatchRessol ( mpz_srcptr self_x , mpz_srcptr self_y ) {
        BigInteger x = BigInteger ( self_x ) ;
        BigInteger y = BigInteger ( self_y ) ;
        return EC::Curve25519::isMatchRessol( x , y );
    }

#ifdef __cplusplus
}
#endif    







