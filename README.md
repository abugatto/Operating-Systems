# Operating-Systems

This repository holds my CS 446: Operating Systems projects.

## How to Compile and Run				  

Use the shell to compile and run with commands in folder "Project <'project-num'>":
1. compile: <br />
	> make
2. run: <br />
	> ./P<'project-num'> config_<'file-num'>.config

## Project 1: File I/O 	

Simulates the file I/O of an operating system, complete with a configuration file and metadata file. The code parses through these files for future use. <br />

**I/O files:** 

1.
	Name: makefile <br />
	Purpose: Links libraries and code for compilation <br />

2. 
	Name: config_<'project-num'>.config <br />
	Purpose: Contains configuration information of the OS simulation <br />

3. 
	Name: Test_<'project-num'>.mdf <br />
	Purpose: Contains metadata information of the OS simulation <br />

4. 
	Name: Logfile_<'project-num'>.lgf <br />
	Purpose: Contains logged file I/O data <br />


**Code Files:**

1. 	
	Name: project1.cpp <br />
	Operation: Runs file I/O functions and logs the data <br />

2.
	Name: operating_system.h <br />
	Operation: Declares file I/O functions <br />

3.
	Name: operating_system.cpp <br />
	Operation: Defines and details file I/O functions <br />


**Functions:**

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

3.
	Name: check_metadata_file <br />
	Operation: Checks metadata file for errors <br />
	In: fin, filepath <br />
	Out: m/a <br />

4.
	Name: read_metadata_file <br />
	Operation: Parses metadata file <br /> 
	In: fin <br />
	Out: descriptors, codes, cycles, count <br />

4.
	Name: calculate_time <br />
	Operation: This function calculates metadata metrics by mapping then to their corresponding calculations <br />
	In: cycle times, descriptors, metadata codes, and index <br />
	Out: time calculation <br />

5.
	Name: print_log <br />
	Operation: This function prints the log <br />
	In: cycle_times, descriptors, metadata_codes, cycles. log_filepath <br />
	Out: N/A <br />

6.
	Name: file_log <br />
	Operation: This function saves the log to file <br />
	In: cycle_times, descriptors, metadata_codes, cycles. log_filepath <br />
	Out: N/A <br />

7.
	Name: log <br />
	Operation: This function logs the data <br />
	In: cycle_times, descriptors, metadata_codes, cycles. log_filepath <br />
	Out: N/A <br />

8.
	Name: process_errors <br />
	Operation: This function processes thrown errors in the code <br />
	In: error, cycle_times, descriptors, metadata_codes, cycles. log_filepath, log_type, count <br />
	Out: EXIT_FAILURE <br />


**Results:**

Configuration File: <br /> 
<img src="https://github.com/abugatto/Operating-Systems/blob/master/Project%201/config.png" height="250">

<br /> Metadata File: <br /> 
<img src="https://github.com/abugatto/Operating-Systems/blob/master/Project%201/metadata.png" height="250">

<br /> Log to Monitor: <br /> 
<img src="https://github.com/abugatto/Operating-Systems/blob/master/Project%201/output.png" height="250">

## Project 2: Threads


