
set ( REAL_ESTATE_SRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-RealEstate/ )

target_sources(
  ${LIB_NAME} PUBLIC
  
  ${REAL_ESTATE_SRC}generators/Repay.cpp
)

add_definitions( -DCIRCUIT_BUILDER_REAL_ESTATE ) 