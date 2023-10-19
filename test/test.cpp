 #include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>

#include <json_tree.hpp>
#include <api.hpp>

using namespace std;

string current_dir_path;
int proof_system;

void print_create_context_msg(int context_id) ;
void print_msg(int rtn, int context_id);

int run(const string &circuit_name, const string &cmdline_arg_2);

void circuit_arguments(int context_id, const string &circuit_name, const string &cmdline_arg_2);

void generate_sample_inputs(int context_id, const string &circuit_name, const string &cmdline_arg_2);

void write_pk_json(int context_id, const string &filename);
void load_pk_json(int context_id, const string &filename);

void write_vk_json(int context_id, const string &filename);
void load_vk_json(int context_id, const string &filename);

void write_proof_json(int context_id, const string &filename);
void load_proof_json(int context_id, const string &filename);

void write_text_to_file(const string &filename, const string &content);
void read_text_from_file(const string &filename, string &content);


int main(int argc, char **argv) {
    std::cout << "\n";

    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    current_dir_path.assign(cwd);
    current_dir_path.append("/");

    proof_system = R1CS_GG; // R1CS_ROM_SE ;

    if (argc < 2) { return -1; }
    string cmdline_arg_2 = "";
    if (argc == 3) { cmdline_arg_2 = string(argv[2]); }

    int ret_val = run(std::string( argv[1]), cmdline_arg_2);

    return ret_val;
}


int run(const string &circuit_name, const string &cmdline_arg_2) {

    int context_id, result;
    const bool use_cs_file = false ;
    const bool use_json_pk = false ;
    
    const string cs_file_path = current_dir_path + circuit_name + "_constraint_system.dat";
    const string cs_file_checksum = "test_app";
    const string pk_file_path = current_dir_path + circuit_name + "_crs_pk.dat";
    const string pk_file_json = current_dir_path + circuit_name + "_crs_pk.json";
    const string vk_file_path = current_dir_path + circuit_name + "_crs_vk.dat";
    const string vk_file_json = current_dir_path + circuit_name + "_crs_vk.json";
    const string proof_file_path = current_dir_path + circuit_name + "_proof.dat";
    const string proof_file_json = current_dir_path + circuit_name + "_proof.json";


    //
    // create embedded circuit and write
    //  -   constraint system
    //  -   proving key
    //  -   verify key
    //  to file
    context_id = createCircuitContext(circuit_name.c_str(), proof_system , EC_ALT_BN128 , NULL , NULL , NULL  );
    print_create_context_msg(context_id);
    circuit_arguments(context_id, circuit_name, cmdline_arg_2);
    buildCircuit(context_id);
    runSetup(context_id);
    writeConstraintSystem(context_id, cs_file_path.c_str(), 1, cs_file_checksum.c_str());
    writePK(context_id, pk_file_path.c_str());
    writeVK(context_id, vk_file_path.c_str());
    serializeFormat(context_id, serializeFormatZKlay);
    write_pk_json(context_id, pk_file_json);
    write_vk_json(context_id, vk_file_json);
    finalizeCircuit(context_id);


    // verify checksum (optional)
    context_id = createCircuitContext(circuit_name.c_str(), proof_system , EC_ALT_BN128, NULL , NULL , cs_file_path.c_str() );
    print_create_context_msg(context_id);
    result = verifyConstraintSystemFileChecksum(context_id, cs_file_path.c_str(), cs_file_checksum.c_str());
    print_msg(result, context_id);
    finalizeCircuit(context_id);


    //
    // run proof
    //
    context_id = createCircuitContext(circuit_name.c_str(), proof_system , EC_ALT_BN128 , NULL , NULL , (use_cs_file)?cs_file_path.c_str():NULL );
    print_create_context_msg(context_id);
    circuit_arguments(context_id, circuit_name, cmdline_arg_2);
    serializeFormat(context_id, serializeFormatZKlay);
    buildCircuit(context_id) ; 
    if(use_json_pk) { load_pk_json(context_id, pk_file_json.c_str()); }
    else            { readPK(context_id, pk_file_path.c_str()); }
    generate_sample_inputs(context_id, circuit_name, cmdline_arg_2);
    runProof(context_id);
    write_proof_json(context_id, proof_file_json.c_str());
    finalizeCircuit(context_id);


    //
    // verify proofs
    //
    context_id = createCircuitContext(circuit_name.c_str(), proof_system , EC_ALT_BN128 , NULL , NULL , (use_cs_file)?cs_file_path.c_str():NULL );
    print_create_context_msg(context_id);
    circuit_arguments(context_id, circuit_name, cmdline_arg_2);
    serializeFormat(context_id, serializeFormatZKlay);
    buildCircuit(context_id) ; 
    if(use_json_pk) { load_pk_json(context_id, pk_file_json.c_str()); }
    else            { readPK(context_id, pk_file_path.c_str()); }
    generate_sample_inputs(context_id, circuit_name + "_v", cmdline_arg_2);
    load_vk_json(context_id, vk_file_json.c_str());
    load_proof_json( context_id , proof_file_json.c_str() );
    int verify_result = runVerify(context_id);
    fprintf(stderr, "\n Verify 1 : ");
    print_msg(verify_result, context_id);
    finalizeCircuit(context_id);
 
    return (verify_result == 0) ? 0 : -1;
}


void write_pk_json(int context_id, const string &filename) {
    string pk_json = serializeProofKey(context_id);
    //fprintf(stderr, "<<<<  Proof Key Json  >>>>\n%s\n<<<<              >>>>\n", pk_json.c_str());
    write_text_to_file(filename, pk_json);
}

void load_pk_json(int context_id, const string &filename) {
    string pk_json;
    read_text_from_file(filename, pk_json);
    //fprintf(stderr, "<<<<  Load Proof Key Json [%s] >>>>\n%s\n<<<<              >>>>\n", filename.c_str(), pk_json.c_str());
    deSerializeProofKey(context_id, pk_json.c_str());
}

void write_vk_json(int context_id, const string &filename) {
    string vk_json = serializeVerifyKey(context_id);
    fprintf(stderr, "<<<<  Verify Key Json  >>>>\n%s\n<<<<              >>>>\n", vk_json.c_str());
    write_text_to_file(filename, vk_json);
}

void load_vk_json(int context_id, const string &filename) {
    string vk_json;
    read_text_from_file(filename, vk_json);
    //fprintf(stderr, "<<<<  Load Verify Key Json [%s] >>>>\n%s\n<<<<              >>>>\n", filename.c_str(), vk_json.c_str());
    deSerializeVerifyKey(context_id, vk_json.c_str());
}


void write_proof_json(int context_id, const string &filename) {
    string proof_json = serializeProof(context_id);
    write_text_to_file(filename, proof_json);
    fprintf(stderr, "<<<<  Proof Json  >>>>\n%s\n<<<<              >>>>\n", proof_json.c_str());
}

void load_proof_json(int context_id, const string &filename) {
    string proof_json;
    read_text_from_file(filename, proof_json);
    //fprintf(stderr, "<<<<  Load Proof Json [%s] >>>>\n%s\n<<<<              >>>>\n", filename.c_str(), proof_json.c_str());
    deSerializeProof(context_id, proof_json.c_str());
}


void write_text_to_file(const string &filename, const string &content) {
    std::ofstream out(filename.c_str(), ios::out);
    out << content;
    out.flush();
    out.close();
}

void read_text_from_file(const string &filename, string &content) {
    string line;
    std::ifstream in(filename.c_str(), ios::in);
    while (in.good()) {
        getline(in, line);
        content.append(line);
        content.append("\n");
    }
    in.close();
}


void generate_sample_inputs(int context_id, const string &circuit_name, const string &cmdline_arg_2) {

    string sample_input_selection;
    string json_str;

    read_text_from_file("sample_input.json", json_str);
    JsonTree::Root json_root(json_str);

    if (circuit_name.compare("ZKlay") == 0) {
        string treeHeight = (cmdline_arg_2.size()) ? cmdline_arg_2 : to_string(32);
        string hashType = "Poseidon";
        sample_input_selection = string("ZKlay_") + treeHeight + string("_") + hashType;
    } else if (circuit_name.compare("zkzkRollup") == 0) {
        string treeHeight = (cmdline_arg_2.size()) ? cmdline_arg_2 : to_string(21);
        string hashType = "Poseidon";
        sample_input_selection = circuit_name;
    } else if (circuit_name.compare("RealEstate") == 0) {
        string treeHeight = (cmdline_arg_2.size()) ? cmdline_arg_2 : to_string(21);
        string hashType = "MiMC7";
        sample_input_selection = circuit_name;
    } else {
        sample_input_selection = circuit_name;
    }

    json_str = json_root[sample_input_selection][0].get_json();

    updatePrimaryInputFromJson(context_id, json_str.c_str());

    if (circuit_name.compare("zkzkRollup") == 0) {
        resetPrimaryInputArray(context_id, "leafNodes", 0);
    }

}


void circuit_arguments(int context_id, const string &circuit_name, const string &cmdline_arg_2) {
    if (circuit_name.compare("ZKlay") == 0) {
        string treeHeight = (cmdline_arg_2.size()) ? cmdline_arg_2 : to_string(32);
        string hashType = "Poseidon";
        assignCircuitArgument(context_id, "treeHeight", treeHeight.c_str());
        assignCircuitArgument(context_id, "hashType", hashType.c_str());
    } else if (circuit_name.compare("zkzkRollup") == 0) {
        string treeHeight = (cmdline_arg_2.size()) ? cmdline_arg_2 : to_string(21);
        string hashType = "Poseidon";
        assignCircuitArgument(context_id, "treeHeight", treeHeight.c_str());
        assignCircuitArgument(context_id, "hashType", hashType.c_str());
    } else if (circuit_name.compare("RealEstate") == 0) {
        string treeHeight = (cmdline_arg_2.size()) ? cmdline_arg_2 : to_string(21);
        string hashType = "MiMC7";
        assignCircuitArgument(context_id, "treeHeight", treeHeight.c_str());
        assignCircuitArgument(context_id, "hashType", hashType.c_str());
    }
}



void print_create_context_msg(int context_id) {
    
    if (context_id >= 1 ) { 
        
        const char *msg = getLastFunctionMsg(context_id);
        
        if ( msg ){
            fprintf(stderr, "\n  <<<< %d , %s >>>> \n\n", context_id, msg);
        }else{
            fprintf(stderr, "\n  <<<< %d >>>> \n\n", context_id );
        }

    }else{
        fprintf(stderr, "\n***  Error creating context : ErrorCode : %d ***\n", context_id); 
        exit(-1);
    }

    fflush(stderr);
    
    return;
}

void print_msg(int rtn, int context_id) {
    if (rtn == -1) { fprintf(stderr, "Invalid context id : %d \n", context_id); }
    const char *msg = getLastFunctionMsg(context_id);
    fprintf(stderr, "  <<<< %d , %s >>>> \n\n", rtn, msg);
    return;
}

