
set ( ZKZKROLLUPSRC ${LIBSNARK_SRC_DIR}/../depends/CircuitBuilder-zkzkRollup/ )

target_sources(
  ${LIB_NAME} PUBLIC
  
  ${ZKZKROLLUPSRC}generators/zkzkRollup.cpp
)

add_definitions( -DCIRCUIT_BUILDER_ZKZKROLLUP ) 