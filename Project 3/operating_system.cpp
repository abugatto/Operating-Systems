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
#include <thread>
#include <pthread.h>
#include<semaphore.h>
#include <chrono>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include "operating_system.h"

using namespace std;

////////////////////////////////////////////////////////////////
//                    Define Symbols                          //
////////////////////////////////////////////////////////////////

const char SPACE = ' ';
const char NEWLINE = '\n';
const char LEFT_PARENTHESE = '{';
const char RIGHT_PARENTHESE = '}';
const char HYPHEN = '-';

////////////////////////////////////////////////////////////////
//                 Process Control Block Functions            //
////////////////////////////////////////////////////////////////

Process_Control_Block::Process_Control_Block(const int& PID, const vector<Instruction>& instructions) {
   pid = PID;

   //iterate through instructions to find indices of processi
   string descriptor = instructions.at(0).descriptor; //Starts at first instruction
   int counter = 0;
   for(int p = 0; p < pid; p++) { //find beginning of target process
      while(descriptor != "begin") { //loop ends every time begin is detected
         counter++;
         descriptor = instructions.at(counter).descriptor;
      }
   }

   ix.start = counter;
   while(descriptor != "finish") { //loop ends every time finish is detected
      counter++;
      descriptor = instructions.at(counter).descriptor;
   }

   ix.end = counter;
}

int Process_Control_Block::get_pid() const {
   return pid;
}

void Process_Control_Block::set_state(const STATE& new_state) {
   state = new_state;
}

STATE Process_Control_Block::get_state() const {
   return state;
}

Indices Process_Control_Block::get_indices() const {
   return ix;
}

////////////////////////////////////////////////////////////////
//                 Operating System Functions                 //
////////////////////////////////////////////////////////////////

Operating_System::Operating_System() {
   log_type = MONITOR_AND_OUTPUT_FILE;
   threads = 1;
}

Operating_System::Operating_System(ifstream& fin, const char *argv[], sem_t& sem) {
   log_type = MONITOR_AND_OUTPUT_FILE;
   system_counter = 0;
   threads = 1;
   semaphore = sem;

   if(argv[0][0] == '\0') { //if cstring is empty 
      throw 0;
   }

   string config_filepath = argv[1]; //checks file extension
   string metadata_filepath;
   check_configuration_file(fin, config_filepath);
   read_configuration_file(fin, config_filepath, metadata_filepath);

   int pids = 0;
   check_metadata_file(fin, metadata_filepath, pids);
   read_metadata_file(fin, pids);

   fill_process_queue(pids);
}

void Operating_System::copy(const Operating_System& OS) {
   log_type = OS.log_type;
   sys_time = OS.sys_time;
   threads = OS.threads;
   log_filepath = OS.log_filepath;
   resources = OS.resources;
   semaphore = OS.semaphore;
   cycle_times = OS.cycle_times;
   instructions = OS.instructions;
   process_queue = OS.process_queue;
}

void Operating_System::add_new_program(ifstream& fin, const string& metadata_filepath) {
   instructions.clear();
   while(!process_queue.empty()) { 
      process_queue.pop(); 
   }

   int pids = 0;
   check_metadata_file(fin, metadata_filepath, pids);
   read_metadata_file(fin, pids);

   fill_process_queue(pids);
}

void Operating_System::log_system_data() const {
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
   //open the log file and log running program
   ofstream fout; //open file
   fout.open(log_filepath, ios::app); //ios::app goes to end of file

   //Start timer -> runtime
   sys_time = chrono::high_resolution_clock::now();
   chrono::high_resolution_clock::time_point time1 = sys_time;
   chrono::high_resolution_clock::time_point time2 = sys_time; 

   print_time(fout, time1, time2);

   //sorting algorithm after each process iteration
   //
   //

   //loop through processes and place on job queue until 
   Process_Control_Block* process = nullptr;
   int PID = 0;
   while(PID != -1) {
      //set current process
      process = &process_queue.front();
      PID = process->get_pid() + 1;
      Indices idx = process->get_indices();

      //loops through instructions
      int program_counter = idx.start;
      while(program_counter != idx.end) { //exits when process ends or when process is stopped
         sys_time = chrono::high_resolution_clock::now();
         time1 = sys_time;

         if(instructions.at(program_counter).metadata_code == 'S') {
            if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
               cout << "Simulator program ";
            }

            if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
               fout << "Simulator program ";
            }

            if(instructions.at(program_counter).descriptor == "begin") {
               process->set_state(START);

               if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                  cout << "starting" << endl;
               }

               if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                  fout << "starting" << endl;
               }
            } else if(instructions.at(program_counter).descriptor == "finish") {
               process->set_state(EXIT);

               if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                  cout << "ending" << endl;
               }

               if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                  fout << "ending";
               }
            }
         }

         // check metadata code and output data accordingly
         if(instructions.at(program_counter).metadata_code == 'A') {
            if(instructions.at(program_counter).descriptor == "begin") {
               process->set_state(START);

               for(int j = 0; j < 2; j++) {
                  if(j == 0) {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << "OS: preparing process " << PID << " " << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << "OS: preparing process " << PID << " " << endl;
                     }
                  } else if(j == 1) {
                     this_thread::sleep_for(chrono::milliseconds(calculate_sleep_time(program_counter)));

                     sys_time = chrono::high_resolution_clock::now();
                     time2 = sys_time;
                     printTime(fout, time1, time2);

                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << "OS: starting process " << PID << " " << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << "OS: starting process " << PID << " " << endl;
                     }
                  }
               }
            } else if(instructions.at(program_counter).descriptor == "finish") {
               process->set_state(EXIT);

               if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                  cout << "OS: removing process " << PID << " " << endl;
               }

               if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                  fout << "OS: removing process " << PID << " " << endl;
               }
            }
         }

         // check metadata code and output data accordingly
         if(instructions.at(program_counter).metadata_code == 'P') {
            process->set_state(RUNNING);

            for(int j = 0; j < 2; j++) {
               if(j == 0) {
                  if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                     cout << "Process " << PID << ": start processing action" << endl;
                  }

                  if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                     fout << "Process " << PID << ": start processing action" << endl;
                  }
               } else if(j == 1) {
                  this_thread::sleep_for(chrono::milliseconds(calculate_sleep_time(program_counter)));

                  sys_time = chrono::high_resolution_clock::now();
                  time2 = sys_time;
                  printTime(fout, time1, time2);

                  if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                     cout << "Process " << PID << ": end processing action" << endl;
                  }

                  if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                     fout << "Process " << PID << ": end processing action" << endl;
                  }
               }
            }
         }

         // check metadata code and output data accordingly
         if(instructions.at(program_counter).metadata_code == 'M') {
            process->set_state(WAITING);

            if(instructions.at(program_counter).descriptor == "allocate") {
               for(int j = 0; j < 2; j++) {
                  if(j == 0) {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << "Process " << PID << ": allocating memory" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << "Process " << PID << ": allocating memory" << endl;
                     }
                  } else if(j == 1) {
                     this_thread::sleep_for(chrono::milliseconds(calculate_sleep_time(program_counter)));

                     sys_time = chrono::high_resolution_clock::now();
                     time2 = sys_time;
                     printTime(fout, time1, time2);

                     int addr = allocate_memory();
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << "memory allocated at 0x" << setfill('0') << setw(8) << addr << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << "memory allocated at 0x" << setfill('0') << setw(8) << addr << endl;
                     }
                  }
               }
            } else if(instructions.at(program_counter).descriptor == "block") {
               for(int j = 0; j < 2; j++) {
                  if(j == 0) {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << "Process " << PID << ": start memory blocking" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << "Process " << PID << ": start memory blocking" << endl;
                     }
                  } else if(j == 1) {
                     this_thread::sleep_for(chrono::milliseconds(calculate_sleep_time(program_counter)));

                     sys_time = chrono::high_resolution_clock::now();
                     time2 = sys_time;
                     printTime(fout, time1, time2);

                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << "Process " << PID << ": end memory blocking" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << "Process " << PID << ": end memory blocking" << endl;
                     }
                  }
               }
            }
         }

         // check metadata code and output data accordingly
         if(instructions.at(program_counter).metadata_code == 'O' || instructions.at(program_counter).metadata_code == 'I') {
            // initialize threads for input / output only
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            //lock critical section
            sem_wait(&semaphore);

            process->set_state(WAITING);
            for(int j = 0; j < 2; j++) {
               if(j == 0) {
                  if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                     cout << "Process " << PID << ": start " << instructions.at(program_counter).descriptor;
                  }

                  if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                     fout << "Process " << PID << ": start " << instructions.at(program_counter).descriptor;
                  }

                  if(instructions.at(program_counter).metadata_code == 'O') {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << " output" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << " output" << endl;
                     }
                  } else if(instructions.at(program_counter).metadata_code == 'I') {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << " input" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << " input" << endl;
                     }
                  }
               } else if(j == 1) {
                  int waitTime = calculate_sleep_time(program_counter);
                  void* ptr = &waitTime;

                  //create and join threads
                  pthread_create(&tid, &attr, IO_thread, ptr);
                  pthread_join(tid, NULL);

                  //calculate and print time
                  sys_time = chrono::high_resolution_clock::now();
                  time2 = sys_time;
                  printTime(fout, time1, time2);

                  if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                     cout << "Process " << PID << ": end " << instructions.at(program_counter).descriptor;
                  }
                  if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                     fout << "Process " << PID << ": end " << instructions.at(program_counter).descriptor;
                  }

                  if(instructions.at(program_counter).metadata_code == 'O') {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << " output" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << " output" << endl;
                     }
                  } else if(instructions.at(program_counter).metadata_code == 'I') {
                     if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
                        cout << " input" << endl;
                     }

                     if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
                        fout << " input" << endl;
                     }
                  }
               }
            }

            //unlock semaphore after breaking out of critical section
            sem_post(&semaphore);
         }

         program_counter++;
         system_counter++;
      }

      //after each process
      process_queue.pop();
      if(process_queue.empty()) {
         PID = -1;
      }
   }

   fout.close();
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
      
   while(component != "System") {
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
      get system memory and block size
   */

   int system_memory = 0; 
   fin.ignore(256, ':');
   fin >> system_memory;

   resources.system_memory = 1000 * (int) system_memory; //convert from kilobytes to bytes

   int block_size = 0; 
   fin.ignore(256, ':');
   fin >> block_size;

   resources.memory_block_size = 1000 * (int) block_size; //convert from kilobytes to bytes

   /*
      get projector and hard drive quantity
   */

   int hard_drives = 0; 
   fin.ignore(256, ':');
   fin >> hard_drives;

   resources.hard_drives = hard_drives; //convert from kilobytes to bytes

   int projectors = 0; 
   fin.ignore(256, ':');
   fin >> projectors;

   resources.projectors = projectors; //convert from kilobytes to bytes

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
void Operating_System::read_metadata_file(ifstream& fin, int& pids) {
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

      if(code_temp == 'P') {
         pids++;
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

void Operating_System::fill_process_queue(const int& pids) { //from instruction vector to PCB's
   if(!process_queue.empty()) {
      process_queue.clear();
   }

   for(int pid = 0; pid < pids; pid++) {
      Process_Control_Block temp(pid, instructions);
      process_queue.push_back(temp);
   }
}

void Operating_System::print_time(ofstream& fout, high_resolution_clock::time_point& time1, high_resolution_clock::time_point& time2) {
   duration<double> timespan = duration_cast<duration<double>> (time2 - time1);
   instructions[system_counter].runtime = timespan;

   if(log_type == MONITOR || log_type == MONITOR_AND_OUTPUT_FILE) {
      cout << fixed << setprecision(6) << time_span.count() << SPACE << HYPHEN << SPACE;
   }

   if(log_type == OUTPUT_FILE || log_type == MONITOR_AND_OUTPUT_FILE) {
      fout << fixed << setprecision(6) << time_span.count() << SPACE << HYPHEN << SPACE;
   }
}

int Operating_System::allocate_memory() {
   unsigned int address = srand(time(NULL)); //for random numbers
   return rand() % resources.memory; //use std::hex in cout
}

void* Operating_System::IO_thread(void* instr_time) {
   int* time = (int*) instr_time;

   //Wait
   sleep(chrono::milliseconds(time));

   //Quit Thread
   pthread_exit(NULL);
}

double Operating_System::calculate_sleep_time(const int& index) {
   if(instructions.at(index).descriptor == "start" || instructions.at(index).descriptor == "finish") {
      return (double) 0;
   } else if(instructions.at(index).descriptor == "hard drive") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Hard drive");
   } else if(instructions.at(index).descriptor == "keyboard") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Keyboard");
   } else if(instructions.at(index).descriptor == "scanner") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Scanner");
   } else if(instructions.at(index).descriptor == "monitor") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Monitor");
   } else if(instructions.at(index).descriptor == "run") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Processor");
   } else if(instructions.at(index).descriptor == "allocate") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Memory");
   } else if(instructions.at(index).descriptor == "projector") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Projector");
   } else if(instructions.at(index).descriptor == "block") {
      return (double) instructions.at(index).run_cycles * (double) cycle_times.at("Memory");
   } else {
      throw -8;
   }
}

/*
   Name: calculate_time
   Operation: This function calculates metadata metrics by mapping then to their corresponding calculations
   In: cycle times, descriptors, metadata codes, and index
   Out: time calculation
*/
int Operating_System::calculate_cycle_time(const int& index) const {
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
      print resource data
   */

   cout << endl << "Resource Data" << endl;
   cout << "System Memory = " << resources.system_memory << endl;

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
           << calculate_cycle_time(index) << " ms"
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
   ofstream fout; //open file
   fout.clear();
   fout.open(log_filepath);

   /*
      save resource data
   */

   fout << endl << "Resource Data" << endl;
   fout << "System Memory = " << resources.system_memory << endl;

   /*
      save configuration data
   */
   
   fout << "Configuration File Data" << endl;   
   for(auto& x : cycle_times) { //loop using auto type for map -> c++11
      fout << x.first << " = " << x.second << " ms / cycle" << endl;
   }

   /*
      save log status
   */
   
   fout << "Logged to: ";
   if(log_type == OUTPUT_FILE) {
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
           << calculate_cycle_time(index) << " ms"
           << endl;
   }

   fout << endl;

   fout.close();
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

      if(error == -8) {
      OS.print_log(true);         
      OS.file_log(true);
      cout << "ERROR CODE -8: INVALID INSTRUCTION DATA" << endl;      
      return EXIT_FAILURE;
   }

   return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////

#endif
