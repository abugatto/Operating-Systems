/*


*/

 #ifndef OS_CPP
 #define OS_CPP

////////////////////////////////////////////////////////////////
//            Define Libraries and Namespace                  //
////////////////////////////////////////////////////////////////

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<map>
#include<vector>
#include"operating_system.h"

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
//                    Parsing Functions                       //
////////////////////////////////////////////////////////////////

/*
   Name: check_configuration_file
   Operation: Checks config file for errors
   In: fin, filepath
   Out: m/a
*/
void check_configuration_file(ifstream& fin, const char *argv[]) {
   string filepath = argv[1]; //checks file extension
   if(filepath.find(".conf") == -1) {
      throw -1;
   }

   if(fin.is_open() == false) { //checks for open file
      throw 0;
   }

   if(!(fin >> filepath)) { //checks if file is empty
      throw -3;
   }
}

/*
   Name: read_configuration_file
   Operation: Parses configuration file and extracts necessary information for OS
   In: fin, ...initialized vars for out...
   Out: component cycle times, metadata filepath, log filepath, log type
*/
void read_configuration_file(ifstream& fin, map<string,int>& cycle_times, string& filepath, string& log_filepath, int& log_type) {
   /*
      get metadata filepath
   */
   
   char c;
   fin.ignore(256, ':');
   fin.ignore(256, ':');
   fin.get(c); //go to third line and get data after ': '
   
   fin >> filepath; //reminder: fin gets next word

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
      log_type = 0;
   } else if(logtype == "Log to File") {
      log_type = 1;
   } else if(logtype == "Log to Both") {
      log_type = 2;
   }

   /*
      get filepath
   */

   fin.ignore(256, ':');
   fin >> log_filepath;
}

/*
   Name: check_metadata_file
   Operation: Checks metadata file for errors
   In: fin, filepath
   Out: m/a
*/
void check_metadata_file(ifstream& fin, string filepath) {
   if(filepath.find(".mdf") == -1) { //checks file extension
      throw -2;
   }

   if(fin.is_open() == false) { //checks for open file
      throw 0;
   }

   if(!(fin >> filepath)) { //checks if file is empty
      throw -4;
   }
}

/*
   Name: read_metadata_file
   Operation: Parses metadata file 
   In: fin
   Out: descriptors, codes, cycles, count
*/
void read_metadata_file(ifstream& fin, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, int& instr_count) {   
   fin.ignore(256, NEWLINE); //skip to second line of metadata file
   
   char c;
   instr_count = 0;
   while(c != '.') { //loops over every instruction
      /*
         get code
      */

      char code_temp, lbracket, rbracket;
      fin >> code_temp >> lbracket; //get metadata code
      if(code_temp < 'A' || code_temp > 'Z') { //if not uppercase char throw error
         throw -5;
      }
      
      metadata_codes.push_back(code_temp);

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

      cout << endl;
      cout << descriptor_temp << " ";

      rbracket = descriptor_append;
      bool valid_descriptor = descriptor_temp == "start" || descriptor_temp == "end" || descriptor_temp == "run" || 
                              descriptor_temp == "hard drive" || descriptor_temp == "keyboard" || descriptor_temp == "printer" || 
                              descriptor_temp == "monitor" || descriptor_temp == "allocate" || descriptor_temp == "block" || 
                              descriptor_temp == "mouse" || descriptor_temp == "speaker";
      
      if(!valid_descriptor) { //check for invalid descriptor
         throw -6;
      }
      
      descriptors.push_back(descriptor_temp); //add element to vector

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

      cycles.push_back(cycles_temp); //add element to vector
      instr_count++;
      fin >> c; //get space
   }
}

////////////////////////////////////////////////////////////////
//                          Logging Functions                 //
////////////////////////////////////////////////////////////////

/*
   Name: calculate_time
   Operation: This function calculates metadata metrics by mapping then to their corresponding calculations
   In: cycle times, descriptors, metadata codes, and index
   Out: time calculation
*/
int calculate_time(map<string,int>& cycle_times, vector<string>& descriptors, vector<int>& metadata_codes, int index) {
   if(descriptors.at(index) == "run") {
      return metadata_codes.at(index) * cycle_times.at("Processor");
   } else if(descriptors.at(index) == "hard drive") {
      return metadata_codes.at(index) * cycle_times.at("Hard drive");
   } else if(descriptors.at(index) == "keyboard") {
      return metadata_codes.at(index) * cycle_times.at("Keyboard");
   } else if(descriptors.at(index) == "mouse") {
      return metadata_codes.at(index) * cycle_times.at("Mouse");
   } else if(descriptors.at(index) == "monitor") {
      return metadata_codes.at(index) * cycle_times.at("Monitor");
   } else if(descriptors.at(index) == "speaker") {
      return metadata_codes.at(index) * cycle_times.at("Speaker");
   } else if(descriptors.at(index) == "block") {
      return metadata_codes.at(index) * cycle_times.at("Memory");
   } else if(descriptors.at(index) == "allocate") {
      return metadata_codes.at(index) * cycle_times.at("Memory");
   } else if(descriptors.at(index) == "printer") {
      return metadata_codes.at(index) * cycle_times.at("Printer");
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
void print_log(map<string,int>& cycle_times, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, string log_filepath, int log_type, int count) {
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
   for(int index = 2; index < count - 2; index++) { //start at index=2 to skip start
      cout << metadata_codes.at(index) << LEFT_PARENTHESE
           << descriptors.at(index) << RIGHT_PARENTHESE
           << cycles.at(index) << SPACE << HYPHEN << SPACE
           << calculate_time(cycle_times, descriptors, cycles, index)
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
void file_log(map<string, int>& cycle_times, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, string log_filepath, int log_type, int count) {
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
   for(int index = 2; index < count - 2; index++) { //start at index=2 to skip start
      fout << metadata_codes.at(index) << LEFT_PARENTHESE
           << descriptors.at(index) << RIGHT_PARENTHESE
           << cycles.at(index) << SPACE << HYPHEN << SPACE
           << calculate_time(cycle_times, descriptors, cycles, index)
           << endl;
   }
}

/*
   Name: log
   Operation: This function logs the data
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A
*/
void log(map<string, int>& cycle_times, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, string log_filepath, int log_type, int count) {
   if(log_type == MONITOR) {
      print_log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count);
   } else if(log_type == OUTPUT_FILE) {
      file_log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count);
   } else if(log_type == MONITOR_AND_OUTPUT_FILE) {
      print_log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count);         
      file_log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count);
   }
}

////////////////////////////////////////////////////////////////
//                  Error Handling Function                   //
////////////////////////////////////////////////////////////////

/*
   Name: process_errors
   Operation: This function processes thrown errors in the code
   In: error, cycle_times, descriptors, metadata_codes, cycles. log_filepath, log_type, count
   Out: EXIT_FAILURE
*/
int process_errors(int error, map<string, int>& cycle_times, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, string log_filepath, int log_type, int count) {
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
      log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count + 2); //count+2 to offset skip
      cout << "ERROR CODE -4: EMPTY METADATA FILE" << endl;
      return EXIT_FAILURE;
   }

   if(error == -5) {
      log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count + 2);
      cout << "ERROR CODE -5: INVALID[LOWERCASE] OR MISSING METADATA CODE" << endl;
      return EXIT_FAILURE;
   }

   if(error == -6) {
      log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count + 2);
      cout << "ERROR CODE -6: INVALID[TYPO] OR MISSING METADATA DESCRIPTOR" << endl;
      return EXIT_FAILURE;
   }

   if(error == -7) {
      log(cycle_times, descriptors, metadata_codes, cycles, log_filepath, log_type, count + 2);
      cout << "ERROR CODE -7: INVALID[NEGATIVE] OR MISSING METADATA CYCLES" << endl;      
      return EXIT_FAILURE;
   }

   return EXIT_FAILURE;
}

/////////////////////////////////////////////////////////////////\

#endif
