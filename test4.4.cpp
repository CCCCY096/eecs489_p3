#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

void pc(const char* c)
{
   for(unsigned i = 0; i < 32768; i++)
   {
      if(!*c || *c == '\n' ) break; 
      cout << *(c++);
   }
   cout << endl;
}


int main(){
    char* filename = (char*) vm_map(nullptr, 0);
    pc(filename);
    strcpy(filename, "data1.bin");
    pc(filename);
    char *p1 = (char *) vm_map (filename, 0);
    char *p2 = (char *) vm_map (filename, 1);
    char *p3 = (char *) vm_map (filename, 2);
    char *p4 = (char *) vm_map (filename, 3);
    char *p5 = (char *) vm_map (filename, 4);
    cout << "mapped 6 pages" << endl;
    pc(p2);
    pc(p1);
    pc(p3);
    pc(p4);
    pc(p5);
    p2 = (char *) vm_map (filename, 0);
    p3 = (char *) vm_map (filename, 0);
    int pid = fork();
    if (!pid){
        *p2 = 's';
        cout << *p2 << endl;
        cout << *p3 << endl;
        cout << *p5 << endl;
    }else{
        *p2 = 's';
        cout << *p2 << endl;
        cout << *p3 << endl;
        cout << *p5 << endl;
    }
    return 0;
    // return 0;
}