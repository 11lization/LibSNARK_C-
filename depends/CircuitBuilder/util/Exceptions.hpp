

#pragma once 


#include <string>
#include <stdexcept>

#include <global.hpp>

using namespace std ;

 
namespace CircuitBuilder {


	class NumberFormatException : public std::exception {
    private :

        std::string what_p ;
    
    public :
        
        NumberFormatException(string what ) : what_p(what) {}
    
        const char* what() const noexcept{
            return what_p.c_str() ;
        }
    };

    class ArithmeticException : public std::exception {
    private :

        std::string what_p ;
    
    public :
        
        ArithmeticException(string what ) : what_p(what) {}
    
        const char* what() const noexcept{
            return what_p.c_str() ;
        }
    };


    class NullPointerException : public std::exception {
    private :

        std::string what_p ;
    
    public :
        
        NullPointerException(string what ) : what_p(what) {}
    
        const char* what() const noexcept{
            return what_p.c_str() ;
        }
    };

    

}