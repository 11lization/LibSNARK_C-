/** @file
 *****************************************************************************
 Implementation of the Constraint class.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <set>

#include <libsnark/gadgetlib2/constraint.hpp>
#include <libsnark/gadgetlib2/variable.hpp>

using ::std::string;
using ::std::vector;
using ::std::set;
using ::std::cout;
using ::std::cerr;
using ::std::endl;
using ::std::shared_ptr;

namespace gadgetlib2 {

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    class Constraint                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

#ifdef DEBUG
template<typename FieldT> Constraint<FieldT>::Constraint(const string& name) : name_(name) {}
#else
template<typename FieldT> Constraint<FieldT>::Constraint(const string& name) { libff::UNUSED(name); }
#endif

template<typename FieldT>
string Constraint<FieldT>::name() const {
#   ifdef DEBUG
        return name_;
#   else
        return "";
#   endif
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 class Rank1Constraint                       ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT>
Rank1Constraint<FieldT>::Rank1Constraint(const LinearCombination<FieldT> &a,
                               const LinearCombination<FieldT> &b,
                               const LinearCombination<FieldT> &c,
                               const string& name)
    : Constraint<FieldT>(name), a_(a), b_(b), c_(c) 
{}

template<typename FieldT> LinearCombination<FieldT> Rank1Constraint<FieldT>::a() const {return a_;}
template<typename FieldT> LinearCombination<FieldT> Rank1Constraint<FieldT>::b() const {return b_;}
template<typename FieldT> LinearCombination<FieldT> Rank1Constraint<FieldT>::c() const {return c_;}


template<typename FieldT>
bool Rank1Constraint<FieldT>::isSatisfied(const VariableAssignment<FieldT>& assignment,
                                  const PrintOptions& printOnFail) const {
    const FElem<FieldT> ares = a_.eval(assignment);
    const FElem<FieldT> bres = b_.eval(assignment);
    const FElem<FieldT> cres = c_.eval(assignment);
    if (ares*bres != cres) {
#       ifdef DEBUG
        if (printOnFail == PrintOptions::DBG_PRINT_IF_NOT_SATISFIED) {
            cerr << GADGETLIB2_FMT("Constraint named \"%s\" not satisfied. Constraint is:", Constraint<FieldT>::name().c_str()) << endl;
            cerr << annotation() << endl;
            cerr << "Variable assignments are:" << endl;
            const VariableSet<FieldT> varSet = getUsedVariables();
            for(const Variable<FieldT>& var : varSet) {
                cerr <<  var.name() << ": " << assignment.at(var).asString() << endl;
            }
            cerr << "a:   " << ares.asString() << endl;
            cerr << "b:   " << bres.asString() << endl;
            cerr << "a*b: " << (ares*bres).asString() << endl;
            cerr << "c:   " << cres.asString() << endl;
        }
#       else
        libff::UNUSED(printOnFail);
#       endif
        return false;
    }
    return true;
}

template<typename FieldT> 
string Rank1Constraint<FieldT>::annotation() const {
#   ifndef DEBUG
        return "";
#   endif
    return string("( ") + a_.asString() + " ) * ( " + b_.asString() + " ) = "+ c_.asString();
}

template<typename FieldT> 
const VariableSet<FieldT> Rank1Constraint<FieldT>::getUsedVariables() const {
    VariableSet<FieldT> retSet;
    const VariableSet<FieldT> aSet = a_.getUsedVariables();
    retSet.insert(aSet.begin(), aSet.end());
    const VariableSet<FieldT> bSet = b_.getUsedVariables();
    retSet.insert(bSet.begin(), bSet.end());
    const VariableSet<FieldT> cSet = c_.getUsedVariables();
    retSet.insert(cSet.begin(), cSet.end());
    return retSet;
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 class PolynomialConstraint                 ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT> 
PolynomialConstraint<FieldT>::PolynomialConstraint(const Polynomial<FieldT>& a, const Polynomial<FieldT>& b,
        const string& name) : Constraint<FieldT>(name), a_(a), b_(b) {}

template<typename FieldT> 
bool PolynomialConstraint<FieldT>::isSatisfied(const VariableAssignment<FieldT>& assignment,
                                       const PrintOptions& printOnFail) const {
    const FElem<FieldT> aEval = a_.eval(assignment);
    const FElem<FieldT> bEval = b_.eval(assignment);
    if (aEval != bEval) {
#       ifdef DEBUG
            if(printOnFail == PrintOptions::DBG_PRINT_IF_NOT_SATISFIED) {
                cerr << GADGETLIB2_FMT("Constraint named \"%s\" not satisfied. Constraint is:", Constraint<FieldT>::name().c_str()) << endl;
                cerr << annotation() << endl;
				cerr << "Expecting: " << aEval << " == " << bEval << endl;
                cerr << "Variable assignments are:" << endl;
                const VariableSet<FieldT> varSet = getUsedVariables();
                for(const Variable<FieldT>& var : varSet) {
                    cerr <<  var.name() << ": " << assignment.at(var).asString() << endl;
                }
            }
#       else
            libff::UNUSED(printOnFail);
#       endif

        return false;
    }
    return true;
}

template<typename FieldT> 
string PolynomialConstraint<FieldT>::annotation() const {
#   ifndef DEBUG
        return "";
#   endif
    return a_.asString() + " == " + b_.asString();
}

template<typename FieldT> 
const VariableSet<FieldT> PolynomialConstraint<FieldT>::getUsedVariables() const {
    VariableSet<FieldT> retSet;
    const VariableSet<FieldT> aSet = a_.getUsedVariables();
    retSet.insert(aSet.begin(), aSet.end());
    const VariableSet<FieldT> bSet = b_.getUsedVariables();
    retSet.insert(bSet.begin(), bSet.end());
    return retSet;
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

template<typename FieldT> 
void ConstraintSystem<FieldT>::addConstraint(const Rank1Constraint<FieldT>& c) {
    constraintsPtrs_.emplace_back(::std::shared_ptr<Constraint<FieldT>>(new Rank1Constraint<FieldT>(c)));
}

template<typename FieldT> 
void ConstraintSystem<FieldT>::addConstraint(const PolynomialConstraint<FieldT>& c) {
    constraintsPtrs_.emplace_back(::std::shared_ptr<Constraint<FieldT>>(new PolynomialConstraint<FieldT>(c)));
}

template<typename FieldT> 
bool ConstraintSystem<FieldT>::isSatisfied(const VariableAssignment<FieldT>& assignment,
                                   const PrintOptions& printOnFail) const {
    for(size_t i = 0; i < constraintsPtrs_.size(); ++i) {
        if (!constraintsPtrs_[i]->isSatisfied(assignment, printOnFail)){
            return false;
        }
    }
    return true;
}

template<typename FieldT> 
string ConstraintSystem<FieldT>::annotation() const {
    string retVal("\n");
    for(auto i = constraintsPtrs_.begin(); i != constraintsPtrs_.end(); ++i) {
        retVal += (*i)->annotation() + '\n';
    }
    return retVal;
}

template<typename FieldT> 
VariableSet<FieldT> ConstraintSystem<FieldT>::getUsedVariables() const {
    VariableSet<FieldT> retSet;
    for(auto& pConstraint : constraintsPtrs_) {
        const VariableSet<FieldT> curSet = pConstraint->getUsedVariables();
        retSet.insert(curSet.begin(), curSet.end());
    }
    return retSet;
}

} // namespace gadgetlib2
