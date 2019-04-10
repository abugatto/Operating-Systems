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
   Operating_System OS;
   ifstream fin;
   bool metrics = false;

   try {
      //declares OS object, parses, and stores config / metadata files
      Operating_System* temp = new Operating_System(fin, argv);
      OS.copy(*temp);
      delete temp;

      //prints configuration log
      if(metrics) {
         OS.log_system_data();
      }

      OS.run();
   
   } catch(int error) { //if error exit program with error code
      return process_errors(error, OS);
   }

   // otherwise exit program successfully
   return EXIT_SUCCESS;
}