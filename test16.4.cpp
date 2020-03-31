#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char *swap0 = (char*) vm_map(nullptr, 0);
    char *swap1 = (char*) vm_map(nullptr, 0);
    char *swap2 = (char*) vm_map(nullptr, 0);
    char *swap3 = (char*) vm_map(nullptr, 0);
    char *swap4 = (char*) vm_map(nullptr, 0);
    int pid = fork();
    strcpy(swap0, "shakespeare.txt");
    strcpy(swap1, "shakespeare.txt");
    char *fs1 = (char*) vm_map(swap0,0);
    char *fs2 = (char*) vm_map(swap0,0);
    strcpy(swap2, "shakespeare.txt");
    strcpy(swap3, "shakespeare.txt");

    cout<<*fs1<<endl;
    cout<<*fs2<<endl;

    if(pid){
        int pid1 = fork();
        if (pid1)
        {
            // Parent. Now shared with child1 and child2
            cout << *swap0 << endl; // swap0 is made resident
            *swap3 = 'a'; // swap5 is made resident
            *swap4 = 'a'; // swap6 is made resident
            *fs1 = 't';
            char *myswap = (char*) vm_map(nullptr, 0); 
            strcpy(myswap, "shakespeare.txt"); // swap7 made resident
            cout << *swap0 << endl; // swap0 is made resident
            cout<<*fs1 <<endl;
            char *tmp = (char*) vm_map(fs1, 0);
        }
        else
        {
            // Child2. Now shared with child1
            cout << *swap0 << endl; // there should be no fault
            char *myswap = (char*) vm_map(nullptr, 0); 
            strcpy(myswap, "shakespeare.txt"); // swap8 made resident. There should be no evictoin       
        }     
    }else{
        int pid2 = fork();
        if(pid2){
            // Child1. Shared with its children
            char *myswap = (char*) vm_map(nullptr, 0);
            *fs2 = 't'; 
            strcpy(myswap, "shakespeare.txt"); // swap9 made resident. There should be no evictoin        
            cout << *swap0 << endl; // there should be not fualt
            *swap0 = 1; // there should be copy on write
            cout<<*fs1<<endl;
        }else{
            cout << *swap0 << endl;
            *swap3 = 'd';
            *swap4 = 'f';
            char *myswap = (char*) vm_map(nullptr, 0); 
            strcpy(myswap, "shakespeare.txt");
            cout << *swap0 << endl;
        }
    }
    return 0;
}