#include "Alloc.h"

Alloc::Alloc(string staffFile, string projectsFile, string studentsFile){
    staffFile_=staffFile; projectsFile_=projectsFile; studentsFile_=studentsFile; // stores input file names
    score_=0; // sets score to 0
}

int Alloc::init(){

    // open files
    ifstream staffInput(staffFile_);
    ifstream projectsInput(projectsFile_);
    ifstream studentsInput(studentsFile_);
    
    // check that the files were successfully opened
    if (!staffInput || !projectsInput || !studentsInput)
    {
        throw invalid_argument("Error: One or more input files could not be opened.\n");
    }else{
        // read in file data into maps
        setProjects(projectsInput);
        setStaff(staffInput);
        setStudents(studentsInput);
        
        // close files
        staffInput.close();
        projectsInput.close();
        studentsInput.close();
        return 0;
    }
}

Alloc::~Alloc(){}

void Alloc::setStaff(ifstream& ifs){
   string staffLine;
   map<string, int> data;
   // reads each line of staff file
   while (getline(ifs, staffLine))
   {
      stringstream staffStream(staffLine);
      string staffId;
      int staffLoad;
      staffStream >> staffId >> staffLoad;
      data[staffId] = staffLoad; // sets map key to staff ID and the value to the staff load
   }
   // copy input data into staff map
   staff_=data;
}

void Alloc::setProjects(ifstream& ifs){
    string projectLine;
    map<int, tuple<string, int, string>> data;
    // reads each line of project file
    while (getline(ifs, projectLine))
    {
        stringstream projectsStream(projectLine);
        int projectId;
        string supervisorId, projectTitle;
        int multiplicity;
        projectsStream >> projectId >> supervisorId >> multiplicity;
        getline(projectsStream, projectTitle, '\n');
        projectTitle.erase(0,1);
        data[projectId] = make_tuple(supervisorId, multiplicity, projectTitle); // sets map key to the project id and the value to a tuple of project information
    }
    // copy input data into project map
    projects_=data;

}

void Alloc::setStudents(ifstream& ifs){
   string studentLine;
   map<string, vector<int>> data;
   // reads each line of student file
   while (getline(ifs, studentLine))
   {
      stringstream studentsStream(studentLine);
      string studentId;
      vector<int> choices;
      int choice;
      studentsStream >> studentId;
      while (studentsStream >> choice)
      {
         choices.push_back(choice);
      }
      data[studentId] = choices; // sets map key to the student ID and the value to the chosen projects
   }
   // copy input data into student map
    students_=data;
}

void Alloc::setScore (vector<int> choices, int choice){
   auto it = find(choices.begin(), choices.end(), choice); // finds the chosen project in the project choices
   if(it != choices.end()){
      int index = it - choices.begin(); // sets the index as the chosen projects position in the project choices
      int score = 4 - index; // sets the score based on choice priority
      score_+= score;
   }
}

map<int, tuple<string,int,string>> Alloc::getProjects() const{
    return projects_;

}


map<string,vector<int>> Alloc::getStudents() const{
    return students_;

}

map<string, int> Alloc::getStaff() const{
    return staff_;
}

map<string, int> Alloc::getAllocations() const{
    return allocations_;
}


bool Alloc::checkLegal(int choice){
    // conditions that have to be satisfied for the allocation to be legal
     if ((get<1>(projects_[choice]) > projectsMult_[choice]) && (staff_[get<0>(projects_[choice])] > staffLoad_[get<0>(projects_[choice])]))
         {
            return true;
         }
    return false;
}

void Alloc::printInputs(){
    cout << "------------------------ staff ------------------------" << endl;
    cout << "--------------------StaffID:MaxLoad--------------------" << endl;

    for (auto x : staff_)
    {
        cout << x.first << ":" << x.second << endl;
    }
    cout << "----------------------- projects ----------------------" << endl;
    cout << "------------ProjectID:StaffName:MaxCapacity------------" << endl;
    for (auto y : projects_)
    {
        cout << y.first << ":" << get<0>(y.second) << ":" << get<1>(y.second) << " " << get<2>(y.second) << endl;
    }
    cout << "----------------------- students ----------------------" << endl;
    cout << "------------------StudnetName:Choices------------------" << endl;
    for (auto z : students_)
    {
        string test;
        for (int i : z.second)
        {
            test += to_string(i) + " ";
        }
        cout << z.first << ":" << test << endl;
    }
}

