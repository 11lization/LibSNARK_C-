


namespace libsnark {

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::build_circuit(){
        
        if ( create_circuit_ftn && !cs_file_path.size() ){
        
            return build_circuit_with_generator();
        
        }else if ( create_circuit_ftn && cs_file_path.size() )  {
        
            return build_circuit_with_cs();
        
        }else{
        
            return build_circuit_with_arith();
        
        }
    
    }



    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::build_circuit_with_generator(){
        
        LOGD("\n\n" );
        LOGD("BuildCircuit  :\n" );
        LOGD("Context_ID    : %d\n", id );
        LOGD("Circuit Name  : %s\n", circuit_name.c_str() );

        libff::profiling profile ;

        LOGD("Circuit Args  : \n")
        for( auto Itr : circuit_arguments ){
            LOGD ( "    %s : %s \n" , Itr.first.c_str() , Itr.second.c_str()  );
        }
        
        profile.enter_block("Build Circuit" );

        profile.enter_block("Create Embedded Circuit" ); 
        generator = create_circuit_ftn ( circuit_arguments , config ) ; 
        profile.leave_block("Create Embedded Circuit" ); 

        if (! generator ){
            LOGD("BuildCircuit  : Error\n" );
            print_profile_logs("Build Circuit" , profile );
            strncpy (last_function_msg , "Error : could not build circuit" , last_function_msg_size ); 
            return 1 ;
        }
        
        pb = gadgetlib2::Protoboard<FieldT>::create( this , gadgetlib2::R1P); 

        LOGD("create circuit reader \n");
        embedded_generator_reader = new EmbeddedGeneratorCircuitReader<FieldT>( 
                                        generator, 
                                        pb , 
                                        & wire_variable_map , 
                                        & wire_variable_map_count , 
                                        & zero_variables_idx , 
                                        & zero_variables_count ,
                                        profile , this );
        
        LOGD("get constraint system \n");
        profile.enter_block("Get ConstraintSystem from Gadgetlib2" ); 
        get_constraint_system_from_gadgetlib2_2<FieldT> (* pb, cs , false , profile , this ) ; 
        profile.leave_block("Get ConstraintSystem from Gadgetlib2" ); 

        
        profile.leave_block("Build Circuit" );
            
        
        full_assignment_size =  getLastVariableIndex() ;
        cs.primary_input_size = embedded_generator_reader->getNumInputs() + embedded_generator_reader->getNumOutputs();
        cs.auxiliary_input_size = full_assignment_size - cs.num_inputs();
        
        #ifndef DEBUG
        pb->clear_constraintSystem();
        #endif 

        LOGD("Evaluate Inputs Done : Full Assignments:%u ,  Primary Inputs:%zu , Auxiliary Inputs:%zu \n", 
                full_assignment_size , cs.primary_input_size , cs.auxiliary_input_size ) ;

        inputs_evaluated = false ;
        evaluate_with = Reader ;

        LOGD("BuildCircuit  : Done\n" );

        print_profile_logs("Build Circuit", profile );
        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0 ;
    }



    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::build_circuit_with_cs(){

        LOGD("\n\n" );
        LOGD("BuildCircuit           :\n" );
        LOGD("Context_ID             : %d\n", id );
        LOGD("Circuit Name           : %s\n", circuit_name.c_str() );
        LOGD("Constraint System File : %s\n", cs_file_path.c_str() );

        libff::profiling profile ;

        profile.enter_block("Build Circuit" );

        profile.enter_block("Create Embedded Circuit" ); 
        generator = create_circuit_ftn ( circuit_arguments , config ) ; 
        profile.leave_block("Create Embedded Circuit" ); 

        if (! generator ){
            LOGD("BuildCircuit  : Error\n" );
            print_profile_logs("Build Circuit" , profile );
            strncpy (last_function_msg , "Error : could not build circuit" , last_function_msg_size ); 
            return 1 ;
        }
        
        
        if ( read_cs( profile) != 0 ){
            strncpy (last_function_msg , "Error : could not read constraint system file " , last_function_msg_size ); 
            return 1 ;
        }

        
        profile.leave_block("Build Circuit" );
            
        LOGD("Evaluate Inputs Done : Full Assignments:%u ,  Primary Inputs:%zu , Auxiliary Inputs:%zu \n", 
                full_assignment_size , cs.primary_input_size , cs.auxiliary_input_size ) ;

        inputs_evaluated = false ;
        evaluate_with = Generator ;

        LOGD("BuildCircuit  : Done\n" );

        print_profile_logs("Build Circuit" , profile );
        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0 ;

    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::build_circuit_with_arith(){   
        
        LOGD("\n\n" );
        LOGD("BuildCircuit  :\n" );
        LOGD("Context_ID    : %d\n", id );
        LOGD("Arith File    : %s\n", arith_text_path.c_str() );
        LOGD("Inputs File   : %s\n", inputs_text_path.c_str() );

        libff::profiling profile ;

        pb = gadgetlib2::Protoboard<FieldT>
        ::create( this , gadgetlib2::R1P); 

        LOGD("create circuit reader \n");
        arith_file_reader = new ArithFileCircuitReader<FieldT>(arith_text_path, inputs_text_path , pb , this );
        
        LOGD("get constraint system \n");
        profile.enter_block("Get ConstraintSystem from Gadgetlib2" ); 
        get_constraint_system_from_gadgetlib2_2<FieldT> (* pb, cs , false , profile , this ) ; 
        profile.leave_block("Get ConstraintSystem from Gadgetlib2" ); 


        // extract primary and auxiliary input
        primary_input.clear() ; 
        auxiliary_input.clear() ;
        const r1cs_variable_assignment<FieldT> full_assignment =
            get_variable_assignment_from_gadgetlib2_2<FieldT>(* pb , getLastVariableIndex() );
        cs.primary_input_size = arith_file_reader->getNumInputs() + arith_file_reader->getNumOutputs() ;
        cs.auxiliary_input_size = full_assignment.size() - cs.num_inputs();
        primary_input = r1cs_primary_input<FieldT> ( full_assignment.begin(), full_assignment.begin() + cs.num_inputs() );
        auxiliary_input = r1cs_auxiliary_input<FieldT> ( full_assignment.begin() + cs.num_inputs(), full_assignment.end() );
        LOGD("primary_input_size:%zu , auxiliary_input_size:%zu\n", cs.primary_input_size, cs.auxiliary_input_size);

        // A follow-up will be added.
        if(! cs.is_satisfied(primary_input, auxiliary_input)){
            LOGD("The constraint system is  not satisifed by the value assignment - Terminating.\n");
            LOGD("1194\n");
        }

        evaluate_with = NoPostEvaluation ;

        print_profile_logs("Build Circuit" , profile );
        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0 ;
    }

}
