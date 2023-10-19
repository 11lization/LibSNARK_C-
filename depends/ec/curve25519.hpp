


#include <BigInteger.hpp>



namespace EC {

    class AffinePoint {
    public :
        
        BigInteger x;
        BigInteger y;

        AffinePoint() : x(0l) , y(0l) {}
        
        AffinePoint( const BigInteger &_x, const BigInteger &_y ) : x(_x) , y(_y) {}
        
        AffinePoint( const AffinePoint &p ) : x(p.x) , y(p.y) {}
        
        std::string toString(int radix) {
            return x.toString(radix) + " " + y.toString(radix);
        }

        bool equals( const AffinePoint &p){
            return (p.x.equals(x))&(p.y.equals(y));
        }

    };


    class Curve25519 {
    
    private :

        static size_t BIT_WIDTH ;
        static BigInteger FIELD_PRIME  ; 
        static BigInteger CURVE_ORDER ; 
        static BigInteger SUBGROUP_ORDER ;
        static BigInteger COEFF_A ;


        AffinePoint pt ;

    public :

        Curve25519 ( ) ;
        Curve25519 ( const AffinePoint &pt ) ;
        Curve25519 ( const BigInteger &x , const BigInteger &y ) ;
        Curve25519 ( const BigInteger &x , bool isMatchRessol ) ;

        AffinePoint getPoint() const { return pt ; }
        const BigInteger& PointX() const { return pt.x ; }
        const BigInteger& PointY() const { return pt.y ; }


        Curve25519 add ( const Curve25519 &other ) const ;
        Curve25519 sub ( const Curve25519 &other ) const ;
        Curve25519 mul( const BigInteger &exp ) const ;

        BigInteger FieldDivision( const BigInteger &a , const BigInteger &b) const ;

        std::string toString ( const std::string & label , size_t radix );
        std::string toString () { return toString( "" , 16 ) ; } 
        std::string toString ( const std::string &label ) { return toString( label , 16 ) ; } 
        std::string toString ( size_t radix ) { return toString( "" , radix ) ; } 

        int isMatchRessol();
        static int isMatchRessol( const BigInteger &x , const BigInteger &y );
        

    private :
        
        AffinePoint multiply( const vector<AffinePoint> & precomputedTable , const vector<size_t> & secretBits )  const ;
        
        AffinePoint addAffinePoints( const AffinePoint &p1 , const AffinePoint &p2 ) const ;

        AffinePoint subAffinePoints( const AffinePoint &p1, const AffinePoint &p2) const ;

        vector<size_t> makesecretbits( const BigInteger &input ) const ;

        AffinePoint doubleAffinePoint( const AffinePoint &p) const ;

        vector<AffinePoint> preprocess( const AffinePoint &p, const vector<size_t> &secretBits ) const ;

        static BigInteger computeYCoordinate( const BigInteger &x ) ;
        
    };


}


