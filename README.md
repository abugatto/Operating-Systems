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
	Definition: ''' void check_configuration_file(ifstream& fin, const char *argv[]); '''

2.
	Name: read_configuration_file <br />
	Operation: Parses configuration file and extracts necessary information for OS <br />
	In: fin, ...initialized vars for out... <br />
	Out: component cycle times, metadata filepath, log filepath, log type <br />
	Definition: void read_configuration_file(ifstream& fin, map<string,int>& cycle_times, string& filepath, string& log_filepath, int& log_type); '''


## Project 2: Threads


