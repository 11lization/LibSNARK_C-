/** @file
 *****************************************************************************
 Definition of Protoboard, a "memory manager" for building arithmetic constraints
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_PROTOBOARD_HPP_
#define LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_PROTOBOARD_HPP_

#include <string>

#include <libsnark/gadgetlib2/constraint.hpp>
#include <libsnark/gadgetlib2/variable.hpp>
#include <api/context_base.hpp>
#include <logging.hpp>

#define ASSERT_CONSTRAINTS_SATISFIED(pb) \
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED))

#define ASSERT_CONSTRAINTS_NOT_SATISFIED(pb) \
    ASSERT_FALSE(pb->isSatisfied(PrintOptions::NO_DBG_PRINT))


namespace gadgetlib2 {

class ProtoboardParams; // Forward declaration

typedef ::std::shared_ptr<const ProtoboardParams> ParamsCPtr;

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                       class Protoboard                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
template<typename FieldT>
class Protoboard {
private :
    VariableAssignment<FieldT> assignment_;

    // friend class libsnark::Context ;

protected:
    
    ConstraintSystem<FieldT> constraintSystem_;
    size_t numInputs_;
    ParamsCPtr pParams_;    // TODO try to refactor this out and use inheritance for different types
                                    // of protoboards, for instance TinyRAMProtoboard : public Protoboard
                                    // This may not be trivial because of Gadget multiple inheritance scheme

    Protoboard(libsnark::Context_base * , const FieldType& fieldType, ParamsCPtr pParams);
    
    libsnark::Context_base* g_cxt_ ;

public:
    const FieldType fieldType_;
    libsnark::Context_base* g_cxt(){ return g_cxt_ ; } 
    
    static ProtoboardPtr<FieldT> create(libsnark::Context_base* g_cxt_ , const FieldType& fieldType, ParamsCPtr pParams = NULL) {
        Protoboard * p = new Protoboard(g_cxt_ , fieldType, pParams ) ;
        return ProtoboardPtr<FieldT>(p);
    }

    size_t numVars() const {return assignment_.size();} // TODO change to take num from constraintSys_
    //size_t numVars() const {return constraintSystem_.getUsedVariables().size();} // TODO change to take num from constraintSys_

    void clear_value_mapping(){
        assignment_.clear();
    }
    
    size_t numInputs() const {return numInputs_;} // TODO Madars How do we book keep this?
    ParamsCPtr params() const {return pParams_;}
    FElem<FieldT>& val(const Variable<FieldT>& var);
    FElem<FieldT> val(const LinearCombination<FieldT>& lc) const;
    void setValuesAsBitArray(const VariableArray<FieldT>& varArray, const size_t srcValue);
    void setDualWordValue(const DualWord<FieldT>& dualWord, const size_t srcValue);
    void setMultipackedWordValue(const MultiPackedWord<FieldT>& multipackedWord, const size_t srcValue);

    // The following 3 methods are purposely not overloaded to the same name in order to reduce
    // programmer error. We want the programmer to explicitly code what type of constraint
    // she wants.
    void addRank1Constraint(const LinearCombination<FieldT>& a,
                            const LinearCombination<FieldT>& b,
                            const LinearCombination<FieldT>& c,
                            const ::std::string& name);
    void addGeneralConstraint(const Polynomial<FieldT>& a,
                              const Polynomial<FieldT>& b,
                              const ::std::string& name);
    /// adds a constraint of the form (a == 0)
    void addUnaryConstraint(const LinearCombination<FieldT>& a, const ::std::string& name);
    
    #ifdef DEBUG
        bool isSatisfied(const PrintOptions& printOnFail = PrintOptions::NO_DBG_PRINT);
    #else
        void clear_constraintSystem(){ constraintSystem_.constraintsPtrs_.clear(); }
    #endif
    
    bool flagIsSet(const FlagVariable<FieldT>& flag) const {return val(flag) == 1;}
    void setFlag(const FlagVariable<FieldT>& flag, bool newFlagState = true);
    void clearFlag(const FlagVariable<FieldT>& flag) {val(flag) = 0;}
    void flipFlag(const FlagVariable<FieldT>& flag) {val(flag) = 1 - val(flag);}
    void enforceBooleanity(const Variable<FieldT>& var);
    ::std::string annotation() const;
    ConstraintSystem<FieldT> constraintSystem() const {return constraintSystem_;}
    VariableAssignment<FieldT> assignment() const {return assignment_;}
    const VariableAssignment<FieldT> & const_assignment() const {return assignment_;}
    bool dualWordAssignmentEqualsValue(
            const DualWord<FieldT>& dualWord,
            const size_t expectedValue,
            const PrintOptions& printOption = PrintOptions::NO_DBG_PRINT) const;
    bool multipackedWordAssignmentEqualsValue(
            const MultiPackedWord<FieldT>& multipackedWord,
            const size_t expectedValue,
            const PrintOptions& printOption = PrintOptions::NO_DBG_PRINT) const;
    bool unpackedWordAssignmentEqualsValue(
            const UnpackedWord<FieldT>& unpackedWord,
            const size_t expectedValue,
            const PrintOptions& printOption = PrintOptions::NO_DBG_PRINT) const;
};
/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                     class ProtoboardParams                 ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
/*
    An abstract class to hold any additional information needed by a specific Protoboard. For
    example a Protoboard specific to TinyRAM will have a class ArchParams which will inherit from
    this class.
*/
class ProtoboardParams {
public:
    virtual ~ProtoboardParams() = 0;
};

} // namespace gadgetlib2

#endif // LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_PROTOBOARD_HPP_
