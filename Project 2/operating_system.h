/*
   

*/

 #ifndef OS_H
 #define OS_H

////////////////////////////////////////////////////////////////
// 			  Define Libraries and Namespace				  //
////////////////////////////////////////////////////////////////

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<map>
#include<vector>

using namespace std;

////////////////////////////////////////////////////////////////
//                    Process Control Block                   //
////////////////////////////////////////////////////////////////

class Process_Control_Block {
   public:

   private:
      
};

////////////////////////////////////////////////////////////////
// 					    Parsing Functions					  //
////////////////////////////////////////////////////////////////

/*
   Name: check_configuration_file
   Operation: Checks config file for errors
   In: fin, filename
   Out: m/a
*/
void check_configuration_file(ifstream& fin, const char *argv[]);

/*
	Name: read_configuration_file
	Operation: Parses configuration file and extracts necessary information for OS
	In: fin, ...initialized vars for out...
	Out: component cycle times, metadata filepath, log filepath, log type
*/
void read_configuration_file(ifstream& fin, map<string,int>& cycle_times, string& filepath, string& log_filepath, int& log_type);

/*
   Name: check_metadata_file
   Operation: Checks metadata file for errors
   In: fin, filepath
   Out: m/a
*/
void check_metadata_file(ifstream& fin, string filepath);

/*
   Name: read_metadata_file
   Operation: Parses metadata file 
   In: fin
   Out: descriptors, codes, cycles, count
*/
void read_metadata_file(ifstream& fin, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, int& instr_count);


////////////////////////////////////////////////////////////////
// 					    Logging Functions					  //
////////////////////////////////////////////////////////////////

/*
   Name: calculate_time
   Operation: This function calculates metadata metrics by mapping then to their corresponding calculations
   In: cycle times, descriptors, metadata codes, and index
   Out: time calculation
*/
int calculate_time(map<string,int>& cycle_times, vector<string>& descriptors, vector<int>& metadata_codes, int index);

/*
   Name: print_log
   Operation: This function prints the log
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A
*/
void print_log(map<string,int>& cycle_times, vector<string>& descriptors, vector<char>& mdco, vector<int>& mdcy, string logFilepath, int logType, int count);

/*
   Name: file_log
   Operation: This function saves the log to file
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A
*/
void file_log(map<string,int>& cycle_times, vector<string>& descriptors, vector<char>& mdco, vector<int>& mdcy, string logFilepath, int logType, int count);

/*
   Name: log
   Operation: This function logs the data
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A
*/
void log(map<string,int>& cycle_times, vector<string>& descriptors, vector<char>& mdco, vector<int>& mdcy, string logFilepath, int logType, int count);


////////////////////////////////////////////////////////////////
// 					  Error Handling Function				        //
////////////////////////////////////////////////////////////////

/*
   Name: process_errors
   Operation: This function processes thrown errors in the code
   In: error, cycle_times, descriptors, metadata_codes, cycles. log_filepath, log_type, count
   Out: EXIT_FAILURE
*/
int process_errors(int error, map<string,int>& cycle_times, vector<string>& descriptors, vector<char>& metadata_codes, vector<int>& cycles, string log_filepath, int log_type, int count);


/////////////////////////////////////////////////////////////////

#endif

