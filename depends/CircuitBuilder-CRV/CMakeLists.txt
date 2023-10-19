
set ( CRVSRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-CRV/ )

target_sources(
  ${LIB_NAME} PUBLIC
   
  ${CRVSRC}/generators/Register.cpp
  ${CRVSRC}/generators/Tally.cpp
  ${CRVSRC}/generators/Vote.cpp
)

add_definitions( -DCIRCUIT_BUILDER_CRV ) 