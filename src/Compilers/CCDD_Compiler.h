#ifndef _CCDD_Compiler_h_
#define _CCDD_Compiler_h_
/// NOTE: unfinished

#include "../KC_Languages/OBDD[AND].h"
#include "../KC_Languages/CCDD.h"
#include "CDD_Compiler.h"


namespace KCBox {


class CCDD_Compiler: public CDD_Compiler
{
protected:
public:
	CCDD_Compiler();
	~CCDD_Compiler();
	void Reset();
protected:
	void Allocate_and_Init_Auxiliary_Memory( Variable max_var );
	void Free_Auxiliary_Memory();
public:
	CDD Compile( CCDD_Manager & manager, CNF_Formula & cnf, Heuristic heur = AutomaticalHeur, Chain & vorder = Chain::default_empty_chain );  // Reset outside
protected:
	NodeID Make_Root_Node( CCDD_Manager & manager, NodeID node );
	NodeID Make_Kernelized_Conjunction_Node( CCDD_Manager & manager, NodeID node );
	void Choose_Running_Options( Heuristic heur, Chain & vorder );
	void Compute_Var_Order_Automatical();
	void Choose_Implicate_Computing_Strategy();
	void Reorder_Manager( CCDD_Manager & manager );
protected:
	void Compile_With_Implicite_BCP( CCDD_Manager & manager );
	NodeID Make_Node_With_Imp( CCDD_Manager & manager, NodeID node );
	NodeID Make_Decision_Node( CCDD_Manager & manager, NodeID low, NodeID high );
	NodeID Make_Node_With_Imp( CCDD_Manager & manager, NodeID * nodes, unsigned num );
protected:
	void Verify_Result_Component( Component & comp, CCDD_Manager & manager, NodeID result );
protected:
	void Compile_With_SAT_Imp_Computing( CCDD_Manager & manager );  // employ SAT engine to compute implied literals
	bool Try_Shift_To_Implicite_BCP( CCDD_Manager & manager );
	bool Estimate_Hardness( Component & comp );
	lbool Try_Final_Kernelization( CCDD_Manager & manager );  // return whether solved by this function
	bool Estimate_Final_Kernelization_Effect();
	void Leave_Final_Kernelization( CCDD_Manager & manager );
	void Compute_Second_Var_Order_Automatical( Component & comp );
	lbool Try_Kernelization( CCDD_Manager & manager );  // return whether solved by this function
	bool Estimate_Kernelization_Effect();
	void Leave_Kernelization( CCDD_Manager & manager );
public:
	static void Debug() { Debug_File(); }
	static void Debug_Rand()
	{
		Random_Generator rand_gen;
		CCDD_Compiler compiler;
		compiler.running_options.max_kdepth = 2;
		compiler.debug_options.verify_compilation = true;
		compiler.debug_options.verify_component_compilation = false;
		for ( unsigned i = 0; i < 100; i++ ) {
			rand_gen.Reset( i );
			cout << "========== " << i << " ==========" << endl;
			unsigned nv = 20, nc = 40;
			CCDD_Manager manager( Variable( Variable::start + nv - 1 ) );
			CNF_Formula cnf( rand_gen, nv, nc, 3, 3 );
			cnf.Sort_Clauses();
			cout << cnf;
			compiler.Compile( manager, cnf, AutomaticalHeur );
//			ofstream fout( "result.cdd" );
			manager.Display( cout );
//			fout.close();
//			system( "./pause" );
		}
	}
	static void Debug_File()
	{
		CCDD_Compiler compiler;
		compiler.running_options.max_kdepth = 2;
		compiler.debug_options.verify_compilation = true;
		compiler.debug_options.verify_component_compilation = false;
		ifstream fin( "instances/s27.cnf" );
//		ifstream fin( "../benchmarks/kc-domain-benchmarks-pmc/circuit_s13207.txt.cnf" );
//		ifstream fin( "../benchmarks/kc-domain-benchmarks/ModelChecking_bmc-ibm-10.cnf" );
//		ifstream fin( "../benchmarks/kc-domain-benchmarks/QIF_min-2s.cnf.gz.no_w.cnf" );
//		ifstream fin( "../benchmarks/kc-domain-benchmarks/BlastedSMT_blasted_squaring1.cnf" );
//		ifstream fin( "../benchmarks/kc-domain-benchmarks-pmc/Planning_logistics.a.cnf" );
		CNF_Formula cnf( fin );
		fin.close();
		CCDD_Manager manager( cnf.Max_Var() );
		NodeID root = compiler.Compile( manager, cnf, AutomaticalHeur );
		ofstream fout( "result.cdd" );
		manager.Display( fout );
		fout.close();
		Random_Generator rand_gen( 0 );
		vector<vector<bool>> samples( 10 );
		manager.Uniformly_Sample( rand_gen, root, samples );
		for ( vector<bool> & sample: samples ) {
			for ( Variable i = Variable::start; i <= manager.Max_Var(); i++ ) {
				if ( sample[i] ) cout << i << " ";
				else cout << '-' << i << " ";
			}
			cout << endl;
		}
	}
	static void Test_CCDD_Compiler( const char * infile, Compiler_Parameters & parameters, bool quiet )
	{
		CCDD_Compiler compiler;
		compiler.debug_options.verify_compilation = false;
		compiler.debug_options.verify_component_compilation = false;
//		compiler.running_options.lit_equivalence_detecting_strategy = Literal_Equivalence_Detection_IBCP;
		compiler.running_options.max_kdepth = parameters.kdepth;
		compiler.running_options.mixed_imp_computing = true;
		compiler.running_options.trivial_variable_bound = 128;
		compiler.running_options.display_kernelizing_process = false;
		compiler.running_options.max_memory = parameters.memo;
		compiler.running_options.removing_redundant_nodes_trigger *= parameters.memo / 4;
		Heuristic heur = Parse_Heuristic( parameters.heur );
		if ( heur != AutomaticalHeur && heur != minfill && heur != LinearLRW && heur != DLCP && heur != dynamic_minfill ) {
			cerr << "ERROR: the heuristic is not supported yet!" << endl;
			exit( 0 );
		}
		if ( quiet ) {
			compiler.running_options.profile_solving = Profiling_Close;
			compiler.running_options.profile_preprocessing = Profiling_Close;
			compiler.running_options.profile_compiling = Profiling_Close;
		}
		ifstream fin( infile );
		CNF_Formula cnf( fin );
		fin.close();
		if ( cnf.Max_Var() == Variable::undef ) {
			if ( compiler.running_options.display_compiling_process ) cout << compiler.running_options.display_prefix << "Number of edges: 0" << endl;
			if ( parameters.CT ) cout << compiler.running_options.display_prefix << "Number of models: " << cnf.Known_Count() << endl;
			return;
		}
		CCDD_Manager manager( cnf.Max_Var() );
		CDD root = compiler.Compile( manager, cnf, heur );
		if ( parameters.CT || parameters.US.Exists() ) {
			compiler._component_cache.Shrink_To_Fit();
			manager.Remove_Redundant_Nodes( root );
		}
		if ( parameters.CT ) {
			BigInt count = manager.Count_Models( root );
			cout << compiler.running_options.display_prefix << compiler.running_options.display_prefix << "Number of models: " << count << endl;
		}
		if ( parameters.US.Exists() ) {
			Random_Generator rand_gen;
			vector<vector<bool>> samples( parameters.US );
			manager.Uniformly_Sample( rand_gen, root, samples );
			const char * sample_file = "samples.txt";
			ofstream fout( sample_file );
			for ( vector<bool> & sample: samples ) {
				for ( Variable i = Variable::start; i <= manager.Max_Var(); i++ ) {
					if ( sample[i] ) fout << i << " ";
					else fout << '-' << i << " ";
				}
				fout << endl;
			}
			fout.close();
			cout << compiler.running_options.display_prefix << "Samples saved to " << sample_file << endl;
		}
		if ( parameters.out_file != nullptr ) {
            ofstream fout( parameters.out_file );
            manager.Display( fout );
            fout.close();
		}
	}
	static void Test_Sampler( const char * infile, Sampler_Parameters & parameters, bool quiet )
	{
		CCDD_Compiler compiler;
		compiler.debug_options.verify_compilation = false;
		compiler.debug_options.verify_component_compilation = false;
//		compiler.running_options.lit_equivalence_detecting_strategy = Literal_Equivalence_Detection_IBCP;
		compiler.running_options.max_kdepth = 128;
		compiler.running_options.mixed_imp_computing = true;
		compiler.running_options.trivial_variable_bound = 128;
		compiler.running_options.display_kernelizing_process = false;
		compiler.running_options.max_memory = parameters.memo;
		compiler.running_options.removing_redundant_nodes_trigger *= parameters.memo / 4;
		if ( quiet ) {
			compiler.running_options.profile_solving = Profiling_Close;
			compiler.running_options.profile_preprocessing = Profiling_Close;
			compiler.running_options.profile_compiling = Profiling_Close;
		}
		ifstream fin( infile );
		CNF_Formula cnf( fin );
		fin.close();
		cerr << "here" << endl;
		if ( cnf.Max_Var() == Variable::undef ) {
			cerr << "ERROR: empty instance!" << endl;
			return;
		}
		CCDD_Manager manager( cnf.Max_Var() );
		CDD root = compiler.Compile( manager, cnf, AutomaticalHeur );
		compiler._component_cache.Shrink_To_Fit();
		Random_Generator rand_gen;
		vector<vector<bool>> samples( parameters.nsamples );
		manager.Uniformly_Sample( rand_gen, root, samples );
		ofstream fout( parameters.out_file );
		for ( vector<bool> & sample: samples ) {
			for ( Variable i = Variable::start; i <= manager.Max_Var(); i++ ) {
				if ( sample[i] ) fout << i << " ";
				else fout << '-' << i << " ";
			}
			fout << endl;
		}
		fout.close();
		cout << compiler.running_options.display_prefix << "Samples saved to " << parameters.out_file << endl;
	}
};


}


#endif  // _CCDD_Compiler_h_
