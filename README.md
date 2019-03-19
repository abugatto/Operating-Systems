# Operating-Systems

This repository holds my CS 446: Operating Systems projects.

## How to Compile and Run				  

Use the shell to compile and run with commands in folder "Project <'project-num'>":
1. compile: <br />
	> make
2. run: <br />
	> ./P<'project-num'> config_<'file-num'>.config

## Project 1: File I/O 		

Files:
1. 

Functions

1.	
	Name: check_configuration_file <br />
	Operation: Checks config file for errors <br />
	In: fin, filename <br />
	Out: m/a <br />

2.
	Name: read_configuration_file <br />
	Operation: Parses configuration file and extracts necessary information for OS <br />
	In: fin, ...initialized vars for out... <br />
	Out: component cycle times, metadata filepath, log filepath, log type <br />
	Definition: void read_configuration_file(ifstream& fin, map<string,int>& cycle_times, string& filepath, string& log_filepath, int& log_type); '''

3.
   Name: check_metadata_file
   Operation: Checks metadata file for errors
   In: fin, filepath
   Out: m/a

4.
   Name: read_metadata_file
   Operation: Parses metadata file 
   In: fin
   Out: descriptors, codes, cycles, count

4.
   Name: calculate_time
   Operation: This function calculates metadata metrics by mapping then to their corresponding calculations
   In: cycle times, descriptors, metadata codes, and index
   Out: time calculation

5.
   Name: print_log
   Operation: This function prints the log
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A

6.
   Name: file_log
   Operation: This function saves the log to file
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A

7.
   Name: log
   Operation: This function logs the data
   In: cycle_times, descriptors, metadata_codes, cycles. log_filepath
   Out: N/A

8.
   Name: process_errors
   Operation: This function processes thrown errors in the code
   In: error, cycle_times, descriptors, metadata_codes, cycles. log_filepath, log_type, count
   Out: EXIT_FAILURE
   

## Project 2: Threads


