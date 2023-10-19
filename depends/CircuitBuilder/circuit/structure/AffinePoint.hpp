

#pragma once


namespace CircuitBuilder { 

    class AffinePoint {
    
    public:

        WirePtr x;
        WirePtr y;

        AffinePoint(WirePtr __x) {
            x = __x;
        }

        AffinePoint(WirePtr __x, WirePtr __y) {
            x = __x;
            y = __y;
        }

        AffinePoint(const AffinePoint &p) {
            x = p.x;
            y = p.y;
        }

        AffinePoint& operator= (const AffinePoint& rhs){
            x = rhs.x ;
            y = rhs.y ;
            return *this ;
        } 
    };

}