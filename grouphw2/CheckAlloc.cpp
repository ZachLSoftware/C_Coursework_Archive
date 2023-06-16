#include "Alloc.h"

class CheckAlloc : public Alloc{
    public:

        // call Super Constructor and add allocation input file
        CheckAlloc(string staffFile, string projectsFile, string studentsFile, string allocFile)
            :Alloc(staffFile, projectsFile, studentsFile){
                allocFile_=allocFile;
            }

        // initialize object by reading in files
       int init() final{
            ifstream allocInput(allocFile_); // open ouptut file
            int superInit = Alloc::init(); // open all other files

            // check files opened and super init completed successfully
            if(!allocInput || superInit){
                throw invalid_argument("Error: Cannot Read Allocation Files.\n");
                return 1;
            }
            else{
                setAllocations(allocInput);
                return 0;
            }
        }

        // reads standard input for allocations and writes it into a map
        void setAllocations(ifstream& alloc){
            string allocLine;
            int lines = 0;
            int count = 1;
            map<string, int> data;

            // get number of lines
            while(getline(alloc, allocLine)) lines+=1;

            // reset ifstream to start
            alloc.clear();
            alloc.seekg(0);

            // read each line of allocation file
            while (getline(alloc, allocLine))
            {
                stringstream allocStream(allocLine);

                // if we are on the last line, read in the allocation score
                if (count==lines){
                    allocStream >> score_;
                    break;
                }

                // get student and allocation
                string studentId;
                int project;
                allocStream >> studentId >> project;

                // if student is already in the allocation map, generate error message
                if(data[studentId]){
                    string err = "Student " + studentId + " allocated more than one project";
                    errs.push_back(err);
                }
                
                data[studentId] = project;
                count+=1;
            }

            // copy input data into allocation map
            allocations_=data;
        }

        // prints contents of files including allocations
        void printInputs() final{
            Alloc::printInputs();
            cout << "----------------------- Allocation -----------------------" << endl;
            cout << "------------------StudnetName:Allocation------------------" << endl;
            for (auto z : allocations_)
            {
                cout << z.first << ":" << to_string(z.second) << endl;
            }
            cout << "Allocation Score: " << to_string(score_) << endl;
        }

        // checks for legality
        void checkLegalAndOptimal(){
            // gets load and multiplicity of allocated projects
            for(auto &alloc : allocations_){
                // skip check if allocation is 0
                if(alloc.second==0) continue;
                // check that project exists
                if(projects_.find(alloc.second)==projects_.end()){
                    string err = "Student " + alloc.first + " is allocated project " + to_string(alloc.second) + " which is not a valid project.";
                    errs.push_back(err);
                    continue;
                }
                projectsMult_[alloc.second] += 1;
                staffLoad_[get<0>(projects_[alloc.second])] += 1;
            }
            
            // legal and optimal checks broken down
            checkStaffLoad();
            checkProjectMult();
            checkStudentAllocations();
            checkSwap();
        }

        // ensure legal staff loads in allocation
        void checkStaffLoad(){

            for(auto &staff : staff_ ){
                // check if allocated staff load is greater than staff max load
                if(staffLoad_[staff.first] > staff.second){
                    string err = "Supervisor " + staff.first + " has load " + to_string(staff.second) + " but is allocated " 
                                + to_string(staffLoad_[staff.first]) + " projects.";

                    errs.push_back(err);
                
                }
            }
        }

        // ensure legal project multiplicity
        void checkProjectMult(){
            for(auto &project : projects_){

                // check if allocated project ,ultiplicity is greater than requested
                if(projectsMult_[project.first] > get<1>(project.second)){
                    string err = "Project " + to_string(project.first) + " \"" + get<2>(project.second) + 
                                                "\" has multiplicity " + to_string(get<1>(project.second)) + " but is allocated to " 
                                                + to_string(projectsMult_[project.first]) + " students.";
                    errs.push_back(err);
                } 
            }
        }

        // do multiple checks on student allocations
        void checkStudentAllocations(){

            for(auto &student:students_){

                // ensure that each student in students list is in the allocation list and has a non-zero allocation
                if(!allocations_[student.first] || allocations_[student.first]==0){

                    for(auto &project:projects_){
                        // if a student is not allocated a project, check if any are available
                        if(checkLegal(project.first)){
                            string err = "Student " + student.first + " is not allocated any project but project " "\"" + get<2>(project.second) + "\" is available";
                            errs.push_back(err);
                            break;
                            }
                        }
                }                     
                    
                int allocPref = getPreference(student, allocations_[student.first]); // get students allocation preference
                
                if(allocPref==0) continue; // skip check if student already has top priority project

                else{studLowPref_[student.first]=student.second;} // add student to vector of students who did not get top priority for swap test


                for(int i=0; i<student.second.size(); i++){
                    // check if any of the students other preferences are available
                    if(allocations_[student.first]!=0 && checkLegal(student.second[i])){
                        // check if the preference is higher on their preference list
                        if(i<allocPref){
                            string err = "Student " + student.first + " is allocated project \"" + get<2>(projects_[allocations_[student.first]]) 
                            + "\" but project " "\"" + get<2>(projects_[student.second[i]]) + "\" is higher on their preference list and is available.";

                            errs.push_back(err);
                            }
                        }
                    }
                }
            }
        

        void checkSwap(){
            map<string, vector<int>> copy = studLowPref_; // copy list to compare preferences
            
            // loop through students
            for(auto &student : studLowPref_){

                copy.erase(student.first); // remove current student (Once they compare to each other student, they do not need to be compared again)

                int allocPref = getPreference(student, allocations_[student.first]); // get the pref of students allocation

                // loop through each pref
                for(int i=0; i<student.second.size(); i++){

                    int proj = student.second[i]; // get project at pref index i

                    // loop through the copy list
                    for(auto &other : copy){

                        // check if the other student has been allocated the project
                        if(proj==allocations_[other.first]){

                            int otherPref=getPreference(other, allocations_[other.first]); // get the preference of the allocation in the others list

                            // check if other students preference for project is lower than current student
                            if(otherPref>i){

                                int otherAllocPref = getPreference(other,allocations_[student.first]); // check if current students allocation is in others preference

                                // if other user also prefers current students project, swap error
                                if(allocPref>otherAllocPref){

                                    string err = "Student " + student.first + " is allocated project \"" + get<2>(projects_[allocations_[student.first]]) 
                                    + "\" but project " "\"" + get<2>(projects_[allocations_[other.first]]) + "\" is higher on their preference list, while "+
                                    "Student " + other.first + " is allocated project \"" + get<2>(projects_[allocations_[other.first]]) 
                                    + "\" but project " "\"" + get<2>(projects_[allocations_[student.first]]) + "\" is higher on their preference list.";
                                    
                                    errs.push_back(err);                                
                                }
                            }
                        }
                    }
                }
            }
        }

        // find the preference of a project
        int getPreference(pair<string, vector<int>> student, int project){
            int pref = find(student.second.begin(), student.second.end(), project)-student.second.begin(); // find the index value of a specific project.

            if(pref==student.second.end()-student.second.begin()) pref = 5; // if preference is the same as the end (meaning not found) auto set to 5
            return pref;
        }

        // print ok or display error messages
        void printResult(){
            if(errs.empty()){
                cout << "OK!" << endl;
            }else{
                for(string err : errs){
                    cout << err << endl << endl;
                }
            }
        }
        
    protected:
        string allocFile_;
        map<string, vector<int>> studLowPref_;
        vector<string> errs;
};

int main(int argc, char *argv[])
{
   try{
        // throw error if 4 arguments are not given
        if (argc != 5)
        {
            throw invalid_argument("Error: Incorrect number of arguments. Expecting 4.\n");
        }
        CheckAlloc check = CheckAlloc(argv[1], argv[2], argv[3], argv[4]);
        check.init();
        check.checkLegalAndOptimal();
        check.printResult();
   }
   catch(const exception& e){
        cout << e.what();
   }

}