//Programmer: Ethan Chiang
//Programmer ID: 1538719
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include <cstring> // for strtok and strcpy

#include "StaticArray.h"
#include "DynamicArray.h"

//structs
struct classCount // to keep track of subjects and sections offered
{
  string subjectCode;
  int count;
};

struct sectionsForTerm // to store sections in each term
{
  string term;
  int numOfSectionsSeen;
  DynamicArray<string> seenSection;
};

//prototpyes
void updateClass(DynamicArray<classCount>&, const string&, int&);
bool dupCheck(const string&, const string&, int&, StaticArray<sectionsForTerm, 100>&, int&, ofstream&);

int main()
{
  cout << "Programmer: Ethan Yi Chiang\n"
       << "Programmer's ID: 1538719\n"
       << "File: " << __FILE__ << "\n\n";

  // for parsing the inputfile
  char* token;
  char buf[1000];
  const char* const tab = "\t";

  DynamicArray<classCount> record; // subject count
  StaticArray<sectionsForTerm, 100> seenTerm; // sections in term

  // open the input file
  ifstream fin;
  fin.open("dvc-schedule.txt");
  try
  {
    if(!fin.good()) throw "I/O error";
  }
  catch(const char* error)
  {
    cout << error << endl;
  }
  ofstream fout;
  fout.open("scheduleOut.txt");

  int count = 0;
  int recordIndex = 0; // how many records
  int termNum = 0; // how many terms
  int duplication = 0;

  // read the input file
  while(fin.good())
  {
    // read the line
    string line;
    getline(fin, line);
    strcpy(buf, line.c_str());
    if (buf[0] == 0) continue; // skip blank lines

    // parse the line
    const string term(token = strtok(buf, tab));
    const string section(token = strtok(0, tab));
    const string course((token = strtok(0, tab)) ? token : "");
    const string instructor((token = strtok(0, tab)) ? token : "");
    const string whenWhere((token = strtok(0, tab)) ? token : "");
    if(course.find('-') == string::npos) continue; // invalid line: no dash in course name
    const string subjectCode(course.begin(), course.begin() + course.find('-'));

    if(dupCheck(term, section, termNum, seenTerm, duplication, fout))
      updateClass(record, subjectCode, recordIndex);

    count++;
    if(count % 1000 == 0)
    {
      cout << ". ";
      cout.flush();
    }
  }
  cout << "\nFinish!\n";
  fin.close();
  
  for(int i = 0; i < recordIndex; i++)
  {
    for(int j = i + 1; j < recordIndex; j++)
    {
      if(record[i].subjectCode > record[j].subjectCode)
        swap(record[i], record[j]);
    }
  }

  for(int i = 0; i < recordIndex; i++)
  {
    if(record[i].count != 0)
    {
      cout << record[i].subjectCode << ", "
           << record[i].count << " section\n";
    }
  }
  cout << "Total duplication: " << duplication << endl;
  return 0;
}

// functions
void updateClass(DynamicArray<classCount>& record, const string& subjectCode, int& recordIndex)
{
  bool found = false;

    for(int i = 0; i < recordIndex; i++)
    {
      if(record[i].subjectCode.compare(subjectCode) == 0)
      {
        found = true;
        record[i].count++;
        break;
      }
    }

  if(!found)
  {
    record[recordIndex].subjectCode = subjectCode;
    record[recordIndex].count = 1;
    recordIndex++;
  }
}

bool dupCheck(const string& term, const string& section, int& termNum, StaticArray<sectionsForTerm, 100>& seenTerm, int& duplication, ofstream& fout)
{
  static int temp;

  for(int i = 0; i < termNum; i++)
  {
    if(seenTerm[i].term == term)
    {
      for(int j = 0; j < seenTerm[i].numOfSectionsSeen; j++)
      {
        if(seenTerm[i].seenSection[j] == section)
        {
          duplication++;
          return false;
        }
      }
      // add new section
      seenTerm[i].numOfSectionsSeen++;
      temp = seenTerm[i].numOfSectionsSeen;
      seenTerm[i].seenSection[temp-1] = section;
      return true;
    }
  }

  // create new term
  seenTerm[termNum].term = term;
  seenTerm[termNum].numOfSectionsSeen = 1;
  seenTerm[termNum].seenSection[0] = section;
  termNum++;
  return true;
}
