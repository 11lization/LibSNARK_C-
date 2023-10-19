
set ( DIDSRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-DID/ )

target_sources(
  ${LIB_NAME} PUBLIC
   
  ${DIDSRC}/generators/DID.cpp
)

add_definitions( -DCIRCUIT_BUILDER_DID ) 