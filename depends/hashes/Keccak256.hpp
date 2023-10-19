
#pragma once

#include <global.hpp>
#include <vector>


namespace Hashes {

	std::vector<uint8_t> keccak256(std::vector<uint8_t> & _input) ;

	std::vector<uint8_t> keccak256( uint8_t *_input , size_t _input_size ) ;

}



