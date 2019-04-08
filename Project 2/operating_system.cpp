/*


*/

 #ifndef OS_CPP
 #define OS_CPP

////////////////////////////////////////////////////////////////
//            Define Libraries and Namespace                  //
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
#include "operating_system.h"

using namespace std;

////////////////////////////////////////////////////////////////
//                    Define Symbols                          //
////////////////////////////////////////////////////////////////

const int MONITOR = 0;
const int OUTPUT_FILE = 1;
const int MONITOR_AND_OUTPUT_FILE = 2;
const char SPACE = ' ';
const char NEWLINE = '\n';
const char LEFT_PARENTHESE = '{';
const char RIGHT_PARENTHESE = '}';
const char HYPHEN = '-';

////////////////////////////////////////////////////////////////
//                 Process Control Block Functions            //
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//                 Operating System Functions                 //
////////////////////////////////////////////////////////////////

Operating_System::Operating_System() {
   log_type = MONITOR_AND_OUTPUT_FILE;
   sys_time = 0;
   threads = 1;
   instr_count = 0;
}

Operating_System::Operating_System(ifstream& fin, const char *argv[]) {
   log_type = MONITOR_AND_OUTPUT_FILE; 
   sys_time = 0;
   threads = 1;
   string metadata_filepath;
   string config_filepath = argv[1]; //checks file extension

   check_configuration_file(fin, config_filepath);
   read_configuration_file(fin, config_filepath, metadata_filepath);

   check_metadata_file(fin, metadata_filepath);
   read_metadata_file(fin);

   fill_job_queue();
}

void Operating_System::copy(const Operating_System& OS) {
   log_type = OS.log_type;
   sys_time = OS.sys_time;
   threads = OS.threads;
   instr_count = OS.instr_count;
   log_filepath = OS.log_filepath;
   cycle_times = OS.cycle_times;
   instructions = OS.instructions;
   job_queue = OS.job_queue;
   ready_queue = OS.ready_queue;
}

void Operating_System::add_new_program(ifstream& fin, const string& metadata_filepath) {
   check_metadata_file(fin, metadata_filepath);
   read_metadata_file(fin);

   fill_job_queue();
}

void Operating_System::log_system() const {
   if(log_type == MONITOR) {
      print_log(false);
   } else if(log_type == OUTPUT_FILE) {
      file_log(false);
   } else if(log_type == MONITOR_AND_OUTPUT_FILE) {
      print_log(false);         
      file_log(false);
   }
}

void Operating_System::run() {
   return;
}

/*
   Name: check_configuration_file
   Operation: Checks config file for errors
   In: fin, filepath
   Out: m/a
*/
void Operating_System::check_configuration_file(ifstream& fin, const string& config_filepath) {
   fin.clear();
   fin.open(config_filepath);
   if(config_filepath.find(".conf") == -1) {
      throw -1;
   }

   if(fin.is_open() == false) { //checks for open file
      throw 0;
   }

   string temp;
   if(!(fin >> temp)) { //checks if file is empty
      throw -3;
   }
}

/*
   Name: check_metadata_file
   Operation: Checks metadata file for errors
   In: fin, filepath
   Out: m/a
*/
void Operating_System::check_metadata_file(ifstream& fin, const string& metadata_filepath) {
   fin.clear();
   fin.open(metadata_filepath);
   if(metadata_filepath.find(".mdf") == -1) { //checks file extension
      throw -2;
   }

   if(fin.is_open() == false) { //checks for open file
      throw 0;
   }

   string temp;
   if(!(fin >> temp)) { //checks if file is empty
      throw -4;
   }
}

/*
   Name: read_configuration_file
   Operation: Parses configuration file and extracts necessary information for OS
   In: fin, ...initialized vars for out...
   Out: component cycle times, metadata filepath, log filepath, log type
*/
void Operating_System::read_configuration_file(ifstream& fin, const string& config_filepath, string& metadata_filepath) {
   /*
      get metadata filepath
   */
   
   char c;
   fin.ignore(256, ':');
   fin.ignore(256, ':');
   fin.get(c); //go to third line and get data after ': '
   
   fin >> metadata_filepath; //reminder: fin gets next word

   //cout << endl << filepath;

   /*
      get cycle times
   */

   int cycle_time;
   string component;
   fin >> component;
      
   while(component != "Log:") {
      if(component == "Hard") { //needed because "Hard drive" is two words
         string drive;
         fin >> drive;
         component.append(" "); 
         component.append(drive);
      }
      
      fin.ignore(256, ':'); //get cycle time for current component
      fin >> cycle_time;
      cycle_times.emplace(component, cycle_time); //add new cycle time to map
      fin >> component; //get next component
   }

   /*
      get log type
   */

   string logtype, temp;

   fin >> logtype;
   logtype.append(" ");
   fin >> temp;
   logtype.append(temp);
   logtype.append(" ");
   fin >> temp;
   logtype.append(temp); 
   
   if(logtype == "Log to Monitor") { //code log type options
      log_type = MONITOR;
   } else if(logtype == "Log to File") {
      log_type = OUTPUT_FILE;
   } else if(logtype == "Log to Both") {
      log_type = MONITOR_AND_OUTPUT_FILE;
   }

   /*
      get filepath
   */

   fin.ignore(256, ':');
   fin >> log_filepath;

   fin.close();
   fin.clear();
}

/*
   Name: read_metadata_file
   Operation: Parses metadata file 
   In: fin
   Out: descriptors, codes, cycles, count
*/
void Operating_System::read_metadata_file(ifstream& fin) {
   if(!instructions.empty()) {
      instructions.clear();
   }

   fin.ignore(256, NEWLINE); //skip to second line of metadata file
   
   char c = ' ';
   instr_count = 0;
   Instruction new_instr;
   while(c != '.') { //loops over every instruction
      /*
         get code
      */

      char code_temp, lbracket, rbracket;
      fin >> code_temp >> lbracket; //get metadata code
      if(code_temp < 'A' || code_temp > 'Z') { //if not uppercase char throw error
         throw -5;
      }
      
      new_instr.metadata_code = code_temp;

      /*
         get descriptor
      */
      
      string descriptor_temp;
      char descriptor_append; 
      fin >> descriptor_append;
      while(descriptor_append != '}') { //make descriptor string with chars
         descriptor_temp += descriptor_append;
         if(descriptor_temp == "hard") { //because hard drive is two words
            descriptor_temp += SPACE;
         }

         fin >> descriptor_append; //get next char
      }

      rbracket = descriptor_append;
      bool valid_descriptor = descriptor_temp == "begin" || descriptor_temp == "finish" || descriptor_temp == "hard drive" || 
                              descriptor_temp == "keyboard" || descriptor_temp == "scanner" || descriptor_temp == "monitor" || 
                              descriptor_temp == "run" || descriptor_temp == "allocate" || descriptor_temp == "projector" || 
                              descriptor_temp == "block";
      
      if(!valid_descriptor) { //check for invalid descriptor
         throw -6;
      }
      
      new_instr.descriptor = descriptor_temp; //add element to vector

      /*
         get cycle
      */
      
      if(fin.peek() == ';') { //check next char for cycle time syntax
         throw -7;
      }
      
      int cycles_temp = -999;
      fin >> cycles_temp;
      if(cycles_temp < 0) { //if negative
         throw -7;
      }

      new_instr.run_cycles = cycles_temp; //add element to vector
      instr_count++;
      fin >> c; //get space

      /*
         push new instruction into end of vector
      */

      instructions.push_back(new_instr);
   }

   fin.close();
   fin.clear();
}

void Operating_System::fill_job_queue() { //from instruction vector to PCB's
   if(!job_queue.empty()) {
      job_queue.clear();
   }

   return;
}

/*
   Name: calculate_time
   Operation: This function calculates metadata metrics by mapping then to their corresponding calculations
   In: cycle times, descriptors, metadata codes, and index
   Out: time calculation
*/
int Operating_System::calculate_time(const int& index) const {
   if(instructions.at(index).descriptor == "hard drive") {
      return instructions.at(index).run_cycles * cycle_times.at("Hard drive");
   } else if(instructions.at(index).descriptor == "keyboard") {
      return instructions.at(index).run_cycles * cycle_times.at("Keyboard");
   } else if(instructions.at(index).descriptor == "scanner") {
      return instructions.at(index).run_cycles * cycle_times.at("Scanner");
   } else if(instructions.at(index).descriptor == "monitor") {
      return instructions.at(index).run_cycles * cycle_times.at("Monitor");
   } else if(instructions.at(index).descriptor == "run") {
      return instructions.at(index).run_cycles * cycle_times.at("Processor");
   } else if(instructions.at(index).descriptor == "allocate") {
      return instructions.at(index).run_cycles * cycle_times.at("Memory");
   } else if(instructions.at(index).descriptor == "projector") {
      return instructions.at(index).run_cycles * cycle_times.at("Projector");
   } else if(instructions.at(index).descriptor == "block") {
      return instructions.at(index).run_cycles * cycle_times.at("Memory");
   } else {
      throw -8;
   }
}

/*
   Name: print_log
   Operation: This function prints the log
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A
*/
void Operating_System::print_log(const bool& offset) const {
   /*
      print configuration data
   */

   cout << endl << "Configuration File Data" << endl;
   for(auto& x : cycle_times) { //loop using auto type for map -> c++11
      cout << x.first << " = " << x.second << " ms / cycle" << endl;
   }

   /*
      print log status
   */
   
   cout << "Logged to: ";
   if(log_type == MONITOR) {
      cout << "monitor" << endl << endl;
   } else if(log_type == OUTPUT_FILE) {
      cout << log_filepath << endl << endl;
   } else if(log_type == MONITOR_AND_OUTPUT_FILE) {
      cout << "monitor and " << log_filepath << endl << endl;
   }

   /*
      print metadata metrics
   */
   
   cout << "Meta-Data Metrics" << endl;

   int offst = 2;
   if(offset) {
      offst = 0;
   }

   for(int index = 2; index < instr_count - offst; index++) { //start at index=2 to skip start
      cout << instructions.at(index).metadata_code << LEFT_PARENTHESE
           << instructions.at(index).descriptor << RIGHT_PARENTHESE
           << instructions.at(index).run_cycles << SPACE << HYPHEN << SPACE
           << calculate_time(index) << " ms"
           << endl;
   }

   cout << endl;
}

/*
   Name: file_log
   Operation: This function saves the log to file
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A
*/
void Operating_System::file_log(const bool& offset) const {
   /*
      save configuration data
   */

   ofstream fout; //open file
   fout.clear();
   fout.open(log_filepath);
   
   fout << "Configuration File Data" << endl;   
   for(auto& x : cycle_times) { //loop using auto type for map -> c++11
      fout << x.first << " = " << x.second << " ms / cycle" << endl;
   }

   /*
      save log status
   */
   
   fout << "Logged to: ";
   if(log_type == MONITOR) {
      fout << "monitor" << endl << endl;
   } else if(log_type == OUTPUT_FILE) {
      fout << log_filepath << endl << endl;
   } else if(log_type == MONITOR_AND_OUTPUT_FILE) {
      fout << "monitor and " << log_filepath << endl << endl;
   }

   /*
      save metadata metrics
   */
   
   fout << "Meta-Data Metrics" << endl;

   int offst = 2;
   if(offset) {
      offst = 0;
   }

   for(int index = 2; index < instr_count - offst; index++) { //start at index=2 to skip start
      fout << instructions.at(index).metadata_code << LEFT_PARENTHESE
           << instructions.at(index).descriptor << RIGHT_PARENTHESE
           << instructions.at(index).run_cycles << SPACE << HYPHEN << SPACE
           << calculate_time(index) << " ms"
           << endl;
   }
}

void Operating_System::time_instruction() const {
   return;
}

void Operating_System::IO_thread(const int& instr_time) {        
   return;
}

////////////////////////////////////////////////////////////////
//                      Friend Functions                      //
//////////////////////////////////////////////////////////////// 

/*
   Name: process_errors
   Operation: This function processes thrown errors in the code
   In: error, cycle_times, descriptors, metadata_codes, cycles. log_filepath, log_type, count
   Out: EXIT_FAILURE
*/
int process_errors(const int& error, Operating_System& OS) {
   if(error == 0) {
      cout << "ERROR CODE 0: FILE NOT FOUND" << endl;
      return EXIT_FAILURE;
   }

   if(error == -1) {
      cout << "ERROR CODE -1: INVALID CONFIGURATION FILE EXTENSION" << endl;
      return EXIT_FAILURE;
   }

   if(error == -2) {
      cout << "ERROR CODE -2: INVALID METADATA FILE EXTENSION" << endl;
      return EXIT_FAILURE;
   }

   if(error == -3) {      
      cout << "ERROR CODE -3: EMPTY CONFIGURATION FILE" << endl;
      return EXIT_FAILURE;
   }

   if(error == -4) {
      OS.print_log(true);         
      OS.file_log(true); //count+2 to offset skip
      cout << "ERROR CODE -4: EMPTY METADATA FILE" << endl;
      return EXIT_FAILURE;
   }

   if(error == -5) {
      OS.print_log(true);         
      OS.file_log(true);
      cout << "ERROR CODE -5: INVALID[LOWERCASE] OR MISSING METADATA CODE" << endl;
      return EXIT_FAILURE;
   }

   if(error == -6) {
      OS.print_log(true);         
      OS.file_log(true);
      cout << "ERROR CODE -6: INVALID[TYPO] OR MISSING METADATA DESCRIPTOR" << endl;
      return EXIT_FAILURE;
   }

   if(error == -7) {
      OS.print_log(true);         
      OS.file_log(true);
      cout << "ERROR CODE -7: INVALID[NEGATIVE] OR MISSING METADATA CYCLES" << endl;      
      return EXIT_FAILURE;
   }

   return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////

#endif
