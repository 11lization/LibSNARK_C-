cmake_minimum_required(VERSION 3.16)

message(STATUS "${SDK_DIR}")
message(STATUS "${CXX_FLAGS}")
message(STATUS "${LIBSNARK_SRC_DIR}")
message(STATUS "${INSTALL_DIR}")
message(STATUS "${TARGET}")


project (AppLibraries)


# Target cmake option
include ( ${LIBSNARK_SRC_DIR}/../build_scripts/${TARGET}.cmake )

add_definitions( -DNO_PROCPS )
add_definitions( -DNDEBUG )
add_definitions( -DNO_PT_COMPRESSION=1 )
add_definitions( -DBINARY_OUTPUT)             # "In serialization, output raw binary data (instead of decimal), which is smaller and faster." 


include_directories( ${INSTALL_DIR}/../include )
include_directories( ${LIBSNARK_SRC_DIR} )
include_directories( ${LIBSNARK_SRC_DIR}/../ )
include_directories( ${LIBSNARK_SRC_DIR}/../depends )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/libff )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/libfqfft )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/gmp_bigInt )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/hashes )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/ec )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/include )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/integer_functions )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/misc )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/json_tree )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/google_snappy )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/circuit )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/circuit/operations )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/circuit/operations/primitive )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/circuit/structure )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/circuit/eval )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/circuit/config )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/util )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-Gadgets )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-Gadgets/hash )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-Gadgets/math )
include_directories( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-Gadgets/diffieHellmanKeyExchange )


set ( FFSRC ${LIBSNARK_SRC_DIR}/../depends/libff/ )
set ( SNARKSRC ${LIBSNARK_SRC_DIR}/ )
set ( CCBDSRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder/ )
set ( GADGETS ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-Gadgets/ )


if(${PRODUCE_SHARED_LIB})
  add_library(  ${LIB_NAME}   SHARED )
else()
  add_library(  ${LIB_NAME}   STATIC )    
endif()

target_sources(

    ${LIB_NAME} PUBLIC


    #
    #   Simplified API to libsnark
    #
    ${LIBSNARK_SRC_DIR}/../api/context_base.cpp
    ${LIBSNARK_SRC_DIR}/../api/api.cpp
    ${LIBSNARK_SRC_DIR}/../api/p_input_update.cpp
    ${LIBSNARK_SRC_DIR}/../api/jni_functions.cpp
    
    
    #
    #   libff 
    #
    ${FFSRC}libff/common/profiling.cpp
    ${FFSRC}libff/common/utils.cpp

    #
    #   libff.alt_bn128
    #
    ${FFSRC}libff/algebra/curves/alt_bn128/alt_bn128_g1.cpp
    ${FFSRC}libff/algebra/curves/alt_bn128/alt_bn128_g2.cpp
    ${FFSRC}libff/algebra/curves/alt_bn128/alt_bn128_init.cpp
    ${FFSRC}libff/algebra/curves/alt_bn128/alt_bn128_pairing.cpp
    ${FFSRC}libff/algebra/curves/alt_bn128/alt_bn128_pp.cpp

    #
    #   libff.bls12_381
    #
    ${FFSRC}libff/algebra/curves/bls12_381/bls12_381_fields.cpp
    ${FFSRC}libff/algebra/curves/bls12_381/bls12_381_g1.cpp
    ${FFSRC}libff/algebra/curves/bls12_381/bls12_381_g2.cpp
    ${FFSRC}libff/algebra/curves/bls12_381/bls12_381_init.cpp
    ${FFSRC}libff/algebra/curves/bls12_381/bls12_381_pairing.cpp
    ${FFSRC}libff/algebra/curves/bls12_381/bls12_381_pp.cpp



    #
    #   libsnark 
    #
    ${SNARKSRC}gadgetlib2/infrastructure.cpp


    
    #
    #   cpp port of jsnark for building circuits 
    #   and generating input variables 
    #
    ${CCBDSRC}util/utilities.cpp
    ${CCBDSRC}circuit/structure/Wire.cpp
    ${CCBDSRC}circuit/structure/WireArray.cpp
    ${CCBDSRC}circuit/structure/BitWire.cpp
    ${CCBDSRC}circuit/operations/primitive/BasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/AddBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/MulBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/SquareOp.cpp
    ${CCBDSRC}circuit/operations/primitive/SquareAndMultiplyOp.cpp
    ${CCBDSRC}circuit/operations/primitive/ConstMulBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/ConstantSquareOp.cpp
    ${CCBDSRC}circuit/operations/primitive/ORBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/XorBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/SplitBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/PackBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/AssertBasicOp.cpp
    ${CCBDSRC}circuit/operations/primitive/NonZeroCheckBasicOp.cpp
    ${CCBDSRC}circuit/structure/CircuitGenerator/CircuitGenerator.cpp
    ${CCBDSRC}circuit/structure/CircuitGenerator/AllocationManagement.cpp
    ${CCBDSRC}circuit/structure/CircuitGenerator/EvaluationQueue.cpp
    ${CCBDSRC}circuit/eval/CircuitEvaluator.cpp
    

    #
    #   Gadgets
    #
    ${GADGETS}diffieHellmanKeyExchange/ECGroupOperationGadget.cpp
    ${GADGETS}diffieHellmanKeyExchange/ECGroupGeneratorGadget.cpp
    ${GADGETS}hash/HashGadget.cpp
    ${GADGETS}hash/MerkleTreePathGadget.cpp
    ${GADGETS}hash/MerkleTreePathGadget_MiMC7.cpp
    ${GADGETS}hash/MiMC7Gadget.cpp
    ${GADGETS}hash/PoseidonGadget.cpp
    ${GADGETS}hash/SHA256Gadget.cpp
    ${GADGETS}hash/SubsetSumHashGadget.cpp
    ${GADGETS}MerkleTreeGadget.cpp
    ${GADGETS}DecryptionGadget.cpp
    


    
    #
    #   Miscellaneous Utility and Application Support Functions
    #
    ${LIBSNARK_SRC_DIR}/../depends/hashes/Keccak256.cpp
    ${LIBSNARK_SRC_DIR}/../depends/hashes/mimc7_hash.cpp
    ${LIBSNARK_SRC_DIR}/../depends/hashes/merkle_tree_path.cpp
    ${LIBSNARK_SRC_DIR}/../depends/hashes/Poseidon.cpp
    ${LIBSNARK_SRC_DIR}/../depends/ec/curve25519.cpp
    ${LIBSNARK_SRC_DIR}/../depends/integer_functions/ressol.cpp
    ${LIBSNARK_SRC_DIR}/../depends/misc/misc.cpp
    ${LIBSNARK_SRC_DIR}/../depends/json_tree/json_tree.cpp
    ${LIBSNARK_SRC_DIR}/../depends/google_snappy/snappy-c.cc
    ${LIBSNARK_SRC_DIR}/../depends/google_snappy/snappy-sinksource.cc
    ${LIBSNARK_SRC_DIR}/../depends/google_snappy/snappy-stubs-internal.cc
    ${LIBSNARK_SRC_DIR}/../depends/google_snappy/snappy.cc

)

install(
  TARGETS ${LIB_NAME}
  DESTINATION ${INSTALL_DIR}
)




#
#   Embedded CRV Circuit Generators : 
#
include ( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-CRV/CMakeLists.txt )


#
#   Embedded ZKlay Circuit Generators : 
#
include ( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-ZKlay/CMakeLists.txt )


#
#   Embedded zkzkRollup Circuit Generators : 
#
include ( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-zkzkRollup/CMakeLists.txt )


#
#   Embedded NFID Circuit Generators : 
#
include ( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-NFID/CMakeLists.txt )

#
#   Embedded DID Circuit Generators : 
#
include ( ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-DID/CMakeLists.txt )

#
# Embedded RealEstate Circuit Generators :
#
include(${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-RealEstate/CMakeLists.txt)