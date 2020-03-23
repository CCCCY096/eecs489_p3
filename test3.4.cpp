#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){
    char* filename = (char*) vm_map(nullptr, 0);
    cout << *filename << endl;
    strcpy(filename, "shakespeare.txt");
    cout << *filename << endl;
    char *p1 = (char *) vm_map (filename, 0);
    char *p2 = (char *) vm_map (filename, 1);
    char *p3 = (char *) vm_map (filename, 2);
    char *p4 = (char *) vm_map (filename, 3);
    char *p5 = (char *) vm_map (filename, 4);
    cout << "mapped 6 pages" << endl;
    cout << *p2 << endl; 
    cout << *p1 << endl; 
    cout << *p3 << endl; 
    *p2 = 'h';
    *p5 = 'a';
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