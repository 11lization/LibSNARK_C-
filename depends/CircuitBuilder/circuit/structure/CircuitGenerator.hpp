


#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <BaseClass.hpp>
#include <utilities.hpp>
#include <WireArray.hpp>


typedef std::map<std::string , std::string> CircuitArguments ;


namespace CircuitBuilder {

    class EvaluationQueue {
    
    private:
        vector<InstructionPtr> base_insertion_order ;
        std::map<hashCode_t , vector<size_t> > base_hash_map ;

    public :
        size_t put_order ;
        size_t vector_alloc_size ;

        EvaluationQueue( size_t estimated_queue_size = 1 );
        
        InstructionPtr find_Instruction_by_Inputs ( InstructionPtr instr );
        size_t size(){ return put_order ; }
        size_t length(){ return put_order ; }
        void put(InstructionPtr instr  );
        
        void clear_quick_search_map(){ base_hash_map.clear() ; }        

        class const_iterator {
        private:
            size_t index ;
            const vector<InstructionPtr> * base ;
        public:
            const_iterator(size_t __index , const vector<InstructionPtr> * __base ) 
                : index(__index) , base(__base ) { }
            
            const_iterator operator++() { const_iterator i = *this; index++; return i; }
            const InstructionPtr & operator*() { return ((*base)[index]) ; }
            bool operator==(const const_iterator& rhs) { return index == rhs.index ; }
            bool operator!=(const const_iterator& rhs) { return index != rhs.index ; }
        };

        const_iterator begin() const { return const_iterator(0 , & base_insertion_order ); }
        const_iterator end() const { return const_iterator( put_order , & base_insertion_order ); }
    };





    class CircuitGenerator {

        private : 
            
            int numOfConstraints;
            CircuitEvaluator *circuitEvaluator;

        protected : 

            EvaluationQueue  evaluationQueue;
            vector<WirePtr> inWires;
            vector<WirePtr> outWires;
            vector<WirePtr> proverWitnessWires;
            string circuitName;

            std::map<string , std::pair<WirePtr , BigInteger>> primary_inputs ;
            std::map<string , std::pair<WiresPtr , std::vector<BigInteger>>> primary_array_inputs ;
            
        public : 
            
            Config config ;
            ConstantWirePtr zeroWire;
            ConstantWirePtr oneWire;
            int currentWireId;
            map<BigInteger, WirePtr > knownConstantWires;
            bool ignore_failed_assertion ;
            
            CircuitGenerator(string circuitName, Config config) ; 
            virtual ~CircuitGenerator(){}

            virtual void finalize();

        private : 

            void initCircuitConstruction() ;

        protected : 

            virtual void buildCircuit() = 0;

            WirePtr makeVariable(WirePtr wire, const string & desc = ""  );

            int assign_inputs(CircuitEvaluator &evaluator) ;

        public : 

            void generateCircuit() ;

            string getName();

            vector<WirePtr> & getInWires() ;

            vector<WirePtr> & getOutWires() ;

            vector<WirePtr> & getProverWitnessWires() ;

            bool In_inWires(WirePtr wire );
            
            bool In_outWires(WirePtr wire );
            
            bool In_ProverWitnessWires(WirePtr wire );


            virtual void assignInputs(CircuitEvaluator &evaluator) = 0;

            WirePtr create_input_wire(const string & desc = "" ) ;

            WirePtr createInputWire(const string & desc = "" ) ;

            WiresPtr createInputWireArray(int n, const string & desc = "" ) ;

            WirePtr create_prover_witness_wire(const string & desc = ""  );

            WirePtr createProverWitnessWire(const string & desc = ""  );

            WiresPtr createProverWitnessWireArray(int n, const string & desc = ""  );

            WiresPtr generateZeroWireArray(int n) ;

            WiresPtr generateOneWireArray(int n) ;

            WirePtr makeOutput(WirePtr wire, const string & desc = "" ) ;
            
            WiresPtr makeOutputArray(Wires & wires, const string & desc = ""  );

            void addDebugInstruction(WirePtr w, const string & desc = ""  ) ;

            void addDebugInstruction(Wires & wires, const string & desc = "" ) ;

            WirePtr createConstantWire(const BigInteger &x, const string & desc = ""  ) ;

            WiresPtr createConstantWireArray(vector<BigInteger> &a, const string & desc = ""  ) ;

            WirePtr createConstantWire(long x, const string & desc = "" ) ;

            WiresPtr createConstantWireArray(vector<long> a, const string & desc = ""  ) ;

            WirePtr createNegConstantWire(BigInteger &x, const string & desc = ""  ) ;

            WirePtr createNegConstantWire(long x, const string & desc = ""  ) ;

            void specifyProverWitnessComputation(InstructionPtr instruction) ;

            EvaluationQueue & getEvaluationQueue() ;

            int getNumWires();

            Wires * addToEvaluationQueue(InstructionPtr e) ;

            int getNumOfConstraints() ;

            void addAssertion(WirePtr w1, WirePtr w2, WirePtr w3, const string & desc = ""  );

            void addZeroAssertion(WirePtr w, const string & desc = ""  ) ;

            void addOneAssertion(WirePtr w, const string & desc = ""  ) ;

            void addBinaryAssertion(WirePtr w, const string & desc = ""  ) ;

            void addEqualityAssertion(WirePtr w1, WirePtr w2, const string & desc = "" ) ;

            void addEqualityAssertion(WirePtr w1, BigInteger &b, const string & desc = ""  ) ;

            void evalCircuit() ;

            void prepFiles() ;

            void pFiles();

            CircuitEvaluator* getCircuitEvaluator() ;

            void DumpCircuit();

            void printCircuit( std::ostream & printWriter , string endline = "\n") ;

            void DumpInputs();
            void dumpAssignments();

            void printInputs( std::ostream & printWriter , string endline = "\n" );

        public:
            static CircuitGenerator* create_generator() ;

            int update_primary_input(const char* input_name , const BigInteger &value );
            int update_primary_input(const char* input_name , int value );
            int update_primary_input(const char* input_name , const char* value_str );
            int update_primary_input_array(const char* input_name , size_t array_index , const BigInteger &value );
            int update_primary_input_array(const char* input_name , int array_index , int value );
            int update_primary_input_array(const char* input_name , int array_index , const char* value_str );
            int reset_primary_input_array(const char* input_name , const BigInteger &value );
            int reset_primary_input_array(const char* input_name , int value );
            int reset_primary_input_array(const char* input_name , const char* value_str );



        private :
            uint32_t last_desc_id ;
            vector<string> op_desc_list ;
        
        public :
            uint32_t set_desc( const string & desc );
            string get_desc( uint32_t desc_id ) ;



        private :
            map<BasicOp*, BasicOp*> AllocatedOperators ;
            map<WirePtr , WirePtr> AllocatedWires ;
            map<WireLabelInstruction*, WireLabelInstruction*> AllocatedWireLabelInstructions ;
            map<WireArray* , WireArray*> AllocatedWireArrays ;
            map<Gadget* , Gadget* > AllocatedGadgets ;
            vector<BasicOp*> toDeallocate;

            void clean_deallocated();

        public:
            void add_allocation( Object* new_allocation );

            //
            // deallocate operator and its output wires
            //
            void deallocate( BasicOp * op );

    };

}
