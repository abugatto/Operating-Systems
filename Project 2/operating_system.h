/*
   

*/

 #ifndef OS_H
 #define OS_H

////////////////////////////////////////////////////////////////
// 			  Define Libraries and Namespace				        //
////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/wait.h>
#include <pthread.h>
#include <chrono>
#include <string>
#include <map>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////
//                   Auxiliary Abstractions                   //
////////////////////////////////////////////////////////////////

/*
   
*/
class Instruction {
   private:
      char metadata_code;
      string descriptor;
      int run_cycles;

      friend class Operating_System;
};

/*
   Enumerated type for the process states
*/
enum STATE {START, READY, RUNNING, WAITING, EXIT};

/*
   Indices of OS process "memory" 
*/
struct Indices {
   int start;
   int end;
};

////////////////////////////////////////////////////////////////
//                Process Control Block Class                 //
////////////////////////////////////////////////////////////////

/*
   Name: Process_Control_Block
   Operation: Controls, tracks, and schedules the operating system processes
*/
class Process_Control_Block {
   public:
      Process_Control_Block(vector<Instruction>& process_instructions);
      

   private:
      int process_number;
      STATE state = START; //converts to int at compile time
      int program_counter;
      Indices ix;
};

////////////////////////////////////////////////////////////////
//                   Operating System Class                   //
////////////////////////////////////////////////////////////////

/*
   
*/
class Operating_System {
   public: //interface functions
      Operating_System();
      Operating_System(ifstream& fin, const char *argv[]);
      void copy(const Operating_System& OS);
      void add_new_program(ifstream& fin, const string& metadata_filepath);
      void log_system() const;
      void run();
      friend int process_errors(const int& error, Operating_System& OS);

   private: //internal functions
      void check_configuration_file(ifstream& fin, const string& config_filepath);
      void check_metadata_file(ifstream& fin, const string& config_filepath);
      void read_configuration_file(ifstream& fin, const string& config_filepath, string& metadata_filepath);
      void read_metadata_file(ifstream& fin);
      void fill_job_queue();
      int calculate_time(const int& index) const;
      void print_log(const bool& offset) const;
      void file_log(const bool& offset) const;
      void time_instruction() const;
      void IO_thread(const int& instr_time);

      int log_type;
      int sys_time = 0;
      int threads = 1;
      string log_filepath;
      int instr_count;
      map<string, int> cycle_times;
      vector<Instruction> instructions;
      vector<Process_Control_Block> job_queue;
      vector<Process_Control_Block> ready_queue;
};

////////////////////////////////////////////////////////////////

#endif

