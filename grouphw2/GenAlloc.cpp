
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "Alloc.h"

class GenAlloc : public Alloc{
   public:

   // call super constructor and add output file
   GenAlloc(string staffFile, string projectsFile, string studentsFile, string outputFile)
      :Alloc(staffFile, projectsFile, studentsFile){
         outputFile_=outputFile;
      }

   ~GenAlloc(){}

   // set a new allocation
   void setAllocations(string student, int choice){

      allocations_[student]=choice; // set allocation map key as student and value as project

      projectsMult_[choice] += 1; // increment projects multiplicity
      staffLoad_[get<0>(projects_[choice])] += 1; // increment staff load
   }

   // allocation algorithm
   void allocateByPref(){

      // create a set for students that do not get an allocation by preference
      set<string> unallocated;

      // begin loop, runs through each student by preference
      for (int i=0; i<4; i++)
      {
         for(const auto &student : students_){

            // skip if student has already been allocated or has already exhausted preferences without allocation
            if(allocations_[student.first] || unallocated.find(student.first)!=unallocated.end()) continue;

            // check student still has a preference at this level and check if project avalible
            if (student.second.size() > i && checkLegal(student.second[i])){

                  // allocate the student to that chosen project and add to the score
                  setAllocations(student.first, student.second[i]);
                  setScore(student.second, student.second[i]);
                  continue;
            }

            // if student wasn't allocated a project yet, check if we have exhausted their preferences
            if((int)student.second.size()-1 <= i){
               unallocated.insert(student.first); // add student to unallocated set
               continue;
            }
         }
      }
      // if unallocated has students, allocate remaining students
      if(!unallocated.empty()) allocateRemaining(unallocated);
   }

   // allocate first come first serve
   void allocateRemaining(set<string> una){

      // loop through unallocated list
      for(string student : una){

         // double check they have not been allocated a project
         if(!allocations_[student]){

      
            for(auto &project : projects_){
               // check for first legal project
               if(checkLegal(project.first)){
                  setAllocations(student, project.first); // allocate student to project
                  break;
               }
            }
            // check if no projects are available
            if(!allocations_[student]){
               allocations_[student]=0; // set allocation to 0
            }
         }
      }
   }

   // write allocation to file
   void writeOutput(){
      cout << "------------------output to text file-------------------" << endl;

      ofstream allocOutput(outputFile_);

      for (auto &sa : allocations_)
      {
         allocOutput << sa.first << " " << sa.second << "\n";
         cout << sa.first << ":" << sa.second << endl;
      }
      allocOutput << score_;
      cout << score_ << endl;

      allocOutput.close();
   }

   private:
      string outputFile_; // store output file name

};

int main(int argc, char *argv[])
{
   try{
       // throw error if 4 arguments are not given
      if (argc != 5)
      {
         throw invalid_argument("Error: Incorrect number of arguments. Expecting 4.\n");
      }
      GenAlloc genAlloc(argv[1], argv[2], argv[3], argv[4]);
      genAlloc.init();
      genAlloc.printInputs();

   cout << "--------------------beign allocation--------------------" << endl;
   
   // try allocate the student to one of their prefered choices
   genAlloc.allocateByPref();
   genAlloc.writeOutput();
   }catch(const exception& e){
        cout << e.what();
   }

}