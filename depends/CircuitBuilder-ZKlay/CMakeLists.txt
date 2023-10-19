
set ( ZKLAYSRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-ZKlay/ )

target_sources(
  ${LIB_NAME} PUBLIC
  
  ${ZKLAYSRC}generators/ZKlay.cpp
  ${ZKLAYSRC}ECGroupExp.cpp
)

add_definitions( -DCIRCUIT_BUILDER_ZKLAY ) 