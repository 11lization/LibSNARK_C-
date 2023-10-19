/** @file
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef INTEGRATION_HPP_
#define INTEGRATION_HPP_

#include <libsnark/gadgetlib2/protoboard.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>

namespace libsnark {

template<typename FieldT >
int get_constraint_system_from_gadgetlib2(
	const gadgetlib2::Protoboard<FieldT> &pb,
	r1cs_constraint_system<FieldT> &cs );

template<typename FieldT >
int get_constraint_system_from_gadgetlib2_2(
    const gadgetlib2::Protoboard<FieldT> &pb,
    r1cs_constraint_system<FieldT> &cs ,
    bool use_multi_thread ,
    libff::profiling & profile ,
    libsnark::Context_base * context );

template<typename FieldT >
r1cs_variable_assignment<FieldT> get_variable_assignment_from_gadgetlib2(const gadgetlib2::Protoboard<FieldT> &pb);

template<typename FieldT >
r1cs_variable_assignment<FieldT> get_variable_assignment_from_gadgetlib2_2(const gadgetlib2::Protoboard<FieldT> &pb , size_t num_vars);

} // libsnark


#endif // INTEGRATION_HPP_
