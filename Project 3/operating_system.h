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
#include <pthread.h>
#include<semaphore.h>
#include <chrono>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <queue>

using namespace std;

////////////////////////////////////////////////////////////////
//                   Auxiliary Abstractions                   //
////////////////////////////////////////////////////////////////

/*
   Enumerated type for the log states
*/
enum LOG_STATE {MONITOR, OUTPUT_FILE, MONITOR_AND_OUTPUT_FILE};

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

/*
   Instruction wrapper class
*/
class Instruction {
   private:
      char metadata_code;
      string descriptor;
      int run_cycles; //if > 1 it iterates until == 1 and updates runtime accordingly
      chrono::duration<double> runtime; 

      friend class Process_Control_Block;
      friend class Operating_System;
};

/*
   Resource management wrapper class
*/
class Hardware {
   private:
      int system_memory;
      int memory_block_size;
      int memory_used;
      int projectors;
      int hard_drives;

      friend class Operating_System;
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
      Process_Control_Block(const int& PID, const vector<Instruction>& instructions);
      int get_pid() const;
      void set_program_counter(const int& count);
      void increment_program_counter();
      int get_program_counter() const;
      void set_state(const STATE& state);
      STATE get_state() const;
      Indices get_indices() const;

   private:
      int pid;
      STATE state = START; //converts to int at compile time
      Indices ix;
};

////////////////////////////////////////////////////////////////
//                   Operating System Class                   //
////////////////////////////////////////////////////////////////

/*
   Name: Operating_System
   Operation: Configures system from file, logs system, controls processing, and controls multiprocessing
*/
class Operating_System {
   public: //interface functions
      Operating_System();
      Operating_System(ifstream& fin, const char *argv[], sem_t& semaphore);
      void copy(const Operating_System& OS);
      void add_new_program(ifstream& fin, const string& metadata_filepath);
      void log_system_data() const;
      void run();

      friend int process_errors(const int& error, Operating_System& OS);

   private: //internal functions
      void check_configuration_file(ifstream& fin, const string& config_filepath);
      void check_metadata_file(ifstream& fin, const string& config_filepath, int& pids);
      void read_configuration_file(ifstream& fin, const string& config_filepath, string& metadata_filepath);
      void read_metadata_file(ifstream& fin, int& pids);
      void fill_process_queue(const int& pids);
      void print_time(ofstream& fout, chrono::high_resolution_clock::time_point& time1, chrono::high_resolution_clock::time_point& time2);
      int allocate_memory() const;
      void* IO_thread();
      double calculate_sleep_time(const int& index) const;
      int calculate_instruction_time(const int& index) const;
      void print_log(const bool& offset) const;
      void file_log(const bool& offset) const;

      LOG_STATE log_type; 
      chrono::high_resolution_clock::time_point sys_time;
      int system_counter;
      int threads;
      string log_filepath;
      Hardware resources;
      sem_t semaphore;
      map<string, int> cycle_times;
      vector<Instruction> instructions;
      queue<Process_Control_Block> process_queue;
};

////////////////////////////////////////////////////////////////

#endif

