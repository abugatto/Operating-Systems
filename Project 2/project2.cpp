/*


*/

////////////////////////////////////////////////////////////////
//            Define Libraries and Namespace                  //
////////////////////////////////////////////////////////////////

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<map>
#include<vector>
#include "operating_system.h"

using namespace std;


////////////////////////////////////////////////////////////////
//                          Test File                         //
////////////////////////////////////////////////////////////////

int main(int argc, const char *argv[]) {
   // declare variables, initialize count and set default logtype (Log to Both)
   ifstream fin;
   string metadata_filepath, log_filepath;
   int instr_count = 0, log_type = 2, sys_time = 0, threads = 1;

   map<string,int> cycle_times;
   map<string, int> cycle_times;
   vector<Instruction> instructions;
   vector<Process_Control_Block> job_queue;
   vector<Process_Control_Block> ready_queue;
   
   try {
      /*
         Parse configuration file
      */

      //clear input filestream and open configuration file
      fin.clear();
      fin.open(argv[1]);
      
      //check for valid configuration file
      check_configuration_file(fin, argv);
      
      //process configuration file
      read_configuration_file(fin, cycle_times, metadata_filepath, log_filepath, log_type);

      /*
         Parse metadata file
      */ 
      
      //close configuration file
      fin.close();
      
      //clear input filestream and open metadata file
      fin.clear();
      fin.open(metadata_filepath);
      
      //check for valid metadata file
      check_metadata_file(fin, metadata_filepath);
      
      //process configuration file
      read_metadata_file(fin, metadata_descriptors, metadata_code, metadata_cycles, count);
      
      //close metadata file
      fin.close();

      /*
         Log data
      */
      
      //log results
      log(cycle_times, metadata_descriptors, metadata_code, metadata_cycles, log_filepath, log_type, count);
      
   } catch(int errors) { //if error exit program with error code
      return process_errors(errors, cycle_times, metadata_descriptors, metadata_code, metadata_cycles, log_filepath, log_type, count);
   }

   // otherwise exit program successfully
   return EXIT_SUCCESS;
}