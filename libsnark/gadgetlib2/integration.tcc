/** @file
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <thread>
#include <mutex>
#include <atomic>
#include <libsnark/gadgetlib2/adapters.hpp>
#include <libsnark/gadgetlib2/integration.hpp>

#include <logging.hpp>

namespace libsnark {

typedef struct {
    size_t min_size, max_size , avg_size , call_cc ;
}debug_lc_convert_sizes_t ;


template<typename FieldT >  
void do_counts ( 
    const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc ,
    debug_lc_convert_sizes_t &counts )
{
    return ;
    if (lc.first.size() < counts.min_size ) { counts.min_size = lc.first.size() ;} ;
    if (lc.first.size() > counts.max_size ) { counts.max_size = lc.first.size() ;} ;
    counts.avg_size += lc.first.size() ;
    counts.call_cc ++ ;
}


template<typename FieldT >  
linear_combination<FieldT> 
    convert_gadgetlib2_linear_combination(
        const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc ,
        debug_lc_convert_sizes_t &counts )
{
    
    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    linear_combination<FieldT> result = lc.second * variable<FieldT>(0);

    for (const typename GLA::linear_term_t &lt : lc.first){
        
        result = result + lt.second * variable<FieldT>(lt.first+1);

    }

    do_counts<FieldT>(lc , counts );
    
    return result;
}

template<typename FieldT >
int get_constraint_system_from_gadgetlib2(
    const gadgetlib2::Protoboard<FieldT> &pb ,
    r1cs_constraint_system<FieldT> &result )
{

    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    debug_lc_convert_sizes_t counts = {1000000000, 0 , 0, 0} ;

    const GLA adapter;

    typename GLA::protoboard_t converted_pb = adapter.convert(pb);
    for (const typename GLA::constraint_t &constr : converted_pb.first)
    {

        result.constraints.emplace_back(r1cs_constraint<FieldT>(convert_gadgetlib2_linear_combination<FieldT>(std::get<0>(constr), counts),
                                                                convert_gadgetlib2_linear_combination<FieldT>(std::get<1>(constr), counts),
                                                                convert_gadgetlib2_linear_combination<FieldT>(std::get<2>(constr), counts)));

    }
    
    LOGD ( "lc convert calls=%lu , lc.first.size() [ min=%lu, max=%lu, avg=%lu ]\n" , 
            counts.call_cc , counts.min_size, counts.max_size , counts.avg_size / counts.call_cc );

    //The number of variables is the highest index created.
    //TODO: If there are multiple protoboards, or variables not assigned to a protoboard, then getNextFreeIndex() is *not* the number of variables! See also in get_variable_assignment_from_gadgetlib2.
    const size_t num_variables = GLA::getNextFreeIndex();
    result.primary_input_size = pb.numInputs();
    result.auxiliary_input_size = num_variables - pb.numInputs();
    return 0 ;
}


template<typename FieldT > 
r1cs_variable_assignment<FieldT > get_variable_assignment_from_gadgetlib2(const gadgetlib2::Protoboard<FieldT> &pb)
{
    // typedef libff::Fr<EC_PP_t> FieldT;
    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    //The number of variables is the highest index created. This is also the required size for the assignment vector.
    //TODO: If there are multiple protoboards, or variables not assigned to a protoboard, then getNextFreeIndex() is *not* the number of variables! See also in get_constraint_system_from_gadgetlib2.
    const size_t num_vars = GLA::getNextFreeIndex();
    const GLA adapter;
    r1cs_variable_assignment<FieldT> result(num_vars, FieldT::zero());
    VariableAssignment<FieldT> assignment = pb.assignment();

    //Go over all assigned values of the protoboard, from every variable-value pair, put the value in the variable.index place of the new assignment.
    for(typename VariableAssignment<FieldT>::iterator iter = assignment.begin(); iter != assignment.end(); ++iter){
        result[GLA::getVariableIndex(iter->first)] = adapter.convert(iter->second);
    }

    return result;
}


template<typename FieldT >
r1cs_variable_assignment<FieldT > get_variable_assignment_from_gadgetlib2_2(const gadgetlib2::Protoboard<FieldT> &pb , size_t num_vars )
{

    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    //The number of variables is the highest index created. This is also the required size for the assignment vector.
    //TODO: If there are multiple protoboards, or variables not assigned to a protoboard, then getNextFreeIndex() is *not* the number of variables! See also in get_constraint_system_from_gadgetlib2.
    const GLA adapter;
    r1cs_variable_assignment<FieldT> result(num_vars, FieldT::zero());
    VariableAssignment<FieldT> assignment = pb.assignment();

    //Go over all assigned values of the protoboard, from every variable-value pair, put the value in the variable.index place of the new assignment.
    for(typename VariableAssignment<FieldT>::iterator iter = assignment.begin(); iter != assignment.end(); ++iter){
        result[iter->first.index()] = adapter.convert(iter->second);
    }

    return result;
}





//
//  Optimized convert_gadgetlib2_linear_combination
//


template<typename FieldT >
class l_term_t {
public:   
    var_index_t index;
    FieldT coeff;
    l_term_t( var_index_t index__ , FieldT coeff__ ) 
        : index ( index__ ) , coeff (coeff__ )
    {}
};

template<typename FieldT >
bool compare_l_term(l_term_t<FieldT> &a , l_term_t<FieldT> &b ){
    return (a.index < b.index ) ;
}

std::mutex log_print_mtx ;

template<typename FieldT >
void convert_gadgetlib2_linear_combination_2(
    const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc,
    linear_combination<FieldT> &result ,
    debug_lc_convert_sizes_t &counts )
{
    // typedef libff::Fr<EC_PP_t> FieldT;
    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    std::list<l_term_t<FieldT>> terms ; 
    terms.get_allocator().allocate(lc.first.size() + 1 ) ;

    terms.emplace_back( variable<FieldT>(0).index , lc.second ) ;

    for (const typename GLA::linear_term_t &lt : lc.first ){
        
        terms.emplace_back( variable<FieldT>(lt.first+1).index , lt.second ) ;

    }

    terms.sort( compare_l_term<FieldT> );


    // merge any term with equal index
    auto lt = terms.begin();
    auto next_lt = std::next(lt);
    
    while ( next_lt != terms.end() ){
        
        if ( lt->index == next_lt->index  ){ 
            assert(false);
            lt->coeff += next_lt->coeff ;
            next_lt = terms.erase(next_lt);
        }else{
            lt = next_lt ; 
            next_lt = std::next(next_lt) ;    
        }

    }

    result.terms.get_allocator().allocate( terms.size() ) ; 
    for ( auto &lt : terms ){ 
        result.terms.emplace_back( lt.index , lt.coeff );
    } 
    
    do_counts<FieldT>(lc , counts );
} 


template<typename FieldT >
void bg_routine (typename gadgetlib2::GadgetLibAdapter<FieldT>::protoboard_t* converted_pb_ptr ,
                r1cs_constraint_system<FieldT> *cs ,
                unsigned int vector_offset ,
                unsigned int vector_step ,
                unsigned int thrd_id ,
                std::atomic<bool> *thrd_complete_status )
{
    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    debug_lc_convert_sizes_t counts = {1000000000, 0 , 0, 0} ;

    for ( unsigned int vector_index = vector_offset ; 
          vector_index < converted_pb_ptr->first.size() ; 
          vector_index += vector_step  )
    {    
        const typename GLA::constraint_t &constr = converted_pb_ptr->first.at(vector_index) ;

        const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc_a = std::get<0>(constr) ;
        const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc_b = std::get<1>(constr) ;
        const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc_c = std::get<2>(constr) ;

        convert_gadgetlib2_linear_combination_2(lc_a , cs->constraints[vector_index].a , counts );
        convert_gadgetlib2_linear_combination_2(lc_b , cs->constraints[vector_index].b , counts );
        convert_gadgetlib2_linear_combination_2(lc_c , cs->constraints[vector_index].c , counts );
    
    }

    thrd_complete_status->store(true) ;

    log_print_mtx.lock(); //TODO skip execution is silent build
    LOGD ( "Thread %u Done :: lc convert calls=%zu , lc.first.size() [ min=%zu, max=%zu, avg=%zu ]\n" , 
            thrd_id , counts.call_cc , counts.min_size, counts.max_size , counts.avg_size / counts.call_cc ) ;
    log_print_mtx.unlock();

}


template<typename FieldT >
int get_constraint_system_from_gadgetlib2_2(
    const gadgetlib2::Protoboard<FieldT> &pb,
    r1cs_constraint_system<FieldT> &cs ,
    bool use_multi_thread ,
    libff::profiling & profile , 
    libsnark::Context_base * context )
{

    libff::UNUSED ( profile ) ;

    typedef gadgetlib2::GadgetLibAdapter<FieldT> GLA;

    debug_lc_convert_sizes_t counts = {1000000000, 0 , 0, 0} ;

    const GLA adapter;

    typename GLA::protoboard_t converted_pb = adapter.convert(pb);

    cs.constraints.get_allocator().allocate(converted_pb.first.size()) ;
    
    // construct empty constraints objects 
    for ( unsigned long ix = 0 ; ix < converted_pb.first.size() ; ix ++){
        cs.constraints.emplace_back();
    }

    if ( use_multi_thread ){

        #define thread_count 4 
        
        std::thread* worker_threads[thread_count] ;
        std::atomic<bool> thrd_complete_status [thread_count] ;

        // start all thread_count threads        
        for (unsigned int i = 0 ; i < thread_count ; i++){
            
            thrd_complete_status[i].store(false) ;
            
            worker_threads[i] = 
                new std::thread ( bg_routine<FieldT> ,  &converted_pb , &cs ,
                                  i , thread_count , 
                                  i+1 , &thrd_complete_status[i]) ;
        }

        // wait for all threads to complete
        while ( ! ( thrd_complete_status[0] && 
                    thrd_complete_status[1] &&
                    thrd_complete_status[2] &&
                    thrd_complete_status[3] ) )
        {
            std::this_thread::yield();
        }
        

    }else{

        // build linear combinations
        int cs_itr = 0 ;
        for (const typename GLA::constraint_t &constr : converted_pb.first)
        {   
            const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc_a = std::get<0>(constr) ;
            const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc_b = std::get<1>(constr) ;
            const typename gadgetlib2::GadgetLibAdapter<FieldT>::linear_combination_t &lc_c = std::get<2>(constr) ;

            convert_gadgetlib2_linear_combination_2<FieldT>(lc_a , cs.constraints[cs_itr].a , counts );
            convert_gadgetlib2_linear_combination_2<FieldT>(lc_b , cs.constraints[cs_itr].b , counts );
            convert_gadgetlib2_linear_combination_2<FieldT>(lc_c , cs.constraints[cs_itr].c , counts );

            cs_itr ++ ;
        }

        // LOGD ( "lc convert calls=%lu , lc.first.size() [ min=%lu, max=%lu, avg=%lu ]\n" , 
                // counts.call_cc , counts.min_size, counts.max_size , counts.avg_size / counts.call_cc );
    }


    const size_t num_variables = context->getLastVariableIndex();
    cs.primary_input_size = pb.numInputs();
    cs.auxiliary_input_size = num_variables - pb.numInputs();
    return 0 ;
}

}




