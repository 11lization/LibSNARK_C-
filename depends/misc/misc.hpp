 


#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <vector>
#include <string>

#include <BigInteger.hpp>

namespace MISC {


	std::string byteArrayToHexString( std::vector<uint8_t> & bytes) ;

	std::vector<size_t> zeroPad( const std::vector<size_t> &a , size_t bitWidth ) ;

	std::vector<size_t> split( const BigInteger &x ) ; 



	template< class VectorType >
	size_t copy_of_range(VectorType &src , VectorType &dst , size_t from , size_t to ){
		
		size_t new_size = to - from ;
		
		dst.resize(new_size) ;

		size_t dst_i = 0 ;
		size_t src_i = from ;

		for ( ; src_i < min(src.size() , to ) 
			  ;	src_i++ , dst_i++ )
		{
			dst[dst_i] =  src[src_i] ;
		}

		return dst_i ;
	}


	template< class VectorType >
	void array_copy( const VectorType &src , size_t srcPos, VectorType &dst , size_t dstPos , size_t length ){
		
		if( src.size() < (srcPos + length) ){
			throw std::range_error("out of range in source array" ) ;
		}
		
		if ( dst.size() < (dstPos + length) ){
			throw std::range_error("out of range in destination array" ) ;
		}

		size_t src_i = srcPos ;
		size_t dst_i = dstPos ;
		size_t i = 0 ;
		for ( ; i < length ; i++ , src_i++ , dst_i++ ) {
			dst[dst_i] =  src[src_i] ;
		}

		return ;
	}


}