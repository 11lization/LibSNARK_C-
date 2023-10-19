 
 

#include <misc.hpp>
 
namespace MISC {


	std::string byteArrayToHexString( std::vector<uint8_t> & bytes) {
        size_t size = bytes.size() ;
        char buff [ ( size * 2)  + 2 ] ;
        int next_buff_pos = 0 ;
        for ( size_t i = 0 ; i < size ; i++ ) {
            next_buff_pos += sprintf( &(buff[next_buff_pos]) , "%02x" , (bytes[i] & 0xFF) );
        }	
        return std::string(buff) ;
    }



    std::vector<size_t> zeroPad( const std::vector<size_t> &a , size_t bitWidth ) {
        
        if( a.size() >= bitWidth ){
            
            return a ; 
        
        } else {
        
            std::vector<size_t> output = std::vector<size_t>( bitWidth );
            
            array_copy < std::vector<size_t> > ( a , 0, output, 0, a.size() );
            
            for(size_t k = a.size() ; k < bitWidth; k++){
                output[k] = 0  ;
            }
            
            return output;
        }

    }


    std::vector<size_t> split( const BigInteger &x ) {
        
        int numChunks = ceil( x.bitLength() );
        
        std::vector<size_t> chunks = std::vector<size_t>( numChunks );
        
        for (int i = 0; i < numChunks; i++) {
            chunks[i] = x.testBit(i);
        }
        
        return chunks;
    }

}