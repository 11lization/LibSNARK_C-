/** @file
 *****************************************************************************
 Implementation of an adapter for interfacing to SNARKs.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <libsnark/gadgetlib2/adapters.hpp>

using gadgetlib2::Variable;
using gadgetlib2::Rank1Constraint;

namespace gadgetlib2 {

// typedef GadgetLibAdapter<FieldT> GLA;

template<typename FieldT>
typename GadgetLibAdapter<FieldT>::linear_term_t GadgetLibAdapter<FieldT>::convert(const LinearTerm<FieldT>& lt) const {
    const variable_index_t var = lt.variable_index() ;
    const Fp_elem_t coeff = convert(lt.coeff());
    return{ var, coeff };
}

template<typename FieldT>
typename GadgetLibAdapter<FieldT>::linear_combination_t GadgetLibAdapter<FieldT>::convert(const LinearCombination<FieldT>& lc) const {
    sparse_vec_t sparse_vec;
    sparse_vec.reserve(lc.linearTerms().size());
    for ( const LinearTerm<FieldT> & lt : lc.linearTerms() ) {
        sparse_vec.emplace_back(convert(lt));
    }
    const Fp_elem_t offset = convert(lc.constant());
    return{ sparse_vec, offset };
}

template<typename FieldT>
typename GadgetLibAdapter<FieldT>::constraint_t GadgetLibAdapter<FieldT>::convert(const Constraint<FieldT>& constraint) const {
    const auto rank1_constraint = dynamic_cast<const Rank1Constraint<FieldT>&>(constraint);
    return constraint_t(convert(rank1_constraint.a()),
        convert(rank1_constraint.b()),
        convert(rank1_constraint.c()));
}

template<typename FieldT>
typename GadgetLibAdapter<FieldT>::constraint_sys_t GadgetLibAdapter<FieldT>::convert(const ConstraintSystem<FieldT>& constraint_sys) const {
    constraint_sys_t retval;
    retval.reserve(constraint_sys.constraintsPtrs().size());
    for ( const typename ConstraintSystem<FieldT>::ConstraintPtr & constraintPtr : constraint_sys.constraintsPtrs()) {
        retval.emplace_back(convert(*constraintPtr));
    }
    return retval;
}

template<typename FieldT>
typename GadgetLibAdapter<FieldT>::assignment_t GadgetLibAdapter<FieldT>::convert(const VariableAssignment<FieldT>& assignment) const {
    assignment_t retval;
    for (const auto & assignmentPair : assignment) {
        const variable_index_t var = assignmentPair.first.index();
        const Fp_elem_t elem = convert(assignmentPair.second);
        retval[var] = elem;
    }
    return retval;
}

// void GLA::resetVariableIndex() { // This is a hack, used for testing
//     Variable::nextFreeIndex_ = 0;
// }

/***TODO: Remove reliance of GadgetLibAdapter conversion on global variable indices, and the resulting limit of single protoboard instance at a time.
This limitation is to prevent a logic bug that may occur if the variables used are given different indices in different generations of the same constraint system.
The indices are assigned on the Variable constructor, using the global variable nextFreeIndex. Thus, creating two protoboards in the same program may cause
unexpected behavior when converting.
Moreover, the bug will create more variables than needed in the converted system, e.g. if variables 0,1,3,4 were used in the gadgetlib2
generated system, then the conversion will create a new r1cs system with variables 0,1,2,3,4 and assign variable 2 the value zero
(when converting the assignment).
Everything should be fixed soon.
If you are sure you know what you are doing, you can comment out the ASSERT line.
*/
template<typename FieldT>
typename GadgetLibAdapter<FieldT>::protoboard_t GadgetLibAdapter<FieldT>::convert(const Protoboard<FieldT>& pb) const {
	//GADGETLIB_ASSERT(pb.numVars()==getNextFreeIndex(), "Some Variables were created and not used, or, more than one protoboard was used.");
    return protoboard_t(convert(pb.constraintSystem()), convert(pb.assignment()));
}

template<typename FieldT>
typename GadgetLibAdapter<FieldT>::Fp_elem_t GadgetLibAdapter<FieldT>::convert(FElem<FieldT> fElem) const {
    using gadgetlib2::R1P_Elem;
    fElem.promoteToFieldType(gadgetlib2::R1P); // convert fElem from FConst to R1P_Elem
    const R1P_Elem<FieldT>* pR1P = dynamic_cast<R1P_Elem<FieldT>*>(fElem.elem().get());
    return pR1P->elem();
}

// bool operator==(const GLA::linear_combination_t& lhs,
//     const GLA::linear_term_t& rhs) {
//     return lhs.first.size() == 1 &&
//         lhs.first.at(0) == rhs &&
//         lhs.second == Fp(0);
// }

}
