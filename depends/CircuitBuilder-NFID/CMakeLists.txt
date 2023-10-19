
set ( NFIDSRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-NFID/ )

target_sources(
  ${LIB_NAME} PUBLIC
  
  ${NFIDSRC}generators/NFID.cpp
)

add_definitions(  -DCIRCUIT_BUILDER_NFID  ) 