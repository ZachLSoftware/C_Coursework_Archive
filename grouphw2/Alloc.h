#ifndef ALLOC_H_
#define ALLOC_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

class Alloc {
    public:
        // constructor, takes input file names
        Alloc(string, string, string);

        // deconstructor
        ~Alloc();

        // returns staff information
        map<string, int> getStaff() const;

        // returns project information
        map<int, tuple<string, int, string>> getProjects() const;

        // returns student infromation
        map<string, vector<int>> getStudents() const;

        // returns allocation information
        map<string,int> getAllocations() const;

        // increases the score based on choice priority
        void setScore(vector<int>, int);

        // reads standard input for projects and writes it into a map
        void setProjects(ifstream&);

        // reads standard input for students and writes it into a map
        void setStudents(ifstream&);

        // reads standard input for staff and writes it into a map
        void setStaff(ifstream&);

        // check if an allocation is legal
        bool checkLegal(int);

        // intialize the object (prevents crash in constructor)
        virtual int init();

        // print information from input files
        virtual void printInputs();


    protected:

        // store file data in maps
        map<string, int> staff_;
        map<int, tuple<string, int, string>> projects_;
        map<string, vector<int>> students_;

        // store allocations in a map
        map<string, int> allocations_;

        // store projects multiplicity and staff load
        map<int, int> projectsMult_;
        map<string, int> staffLoad_;

        // allocation total score
        int score_;

        // store input file names
        string staffFile_;
        string studentsFile_;
        string projectsFile_;
};


#endif