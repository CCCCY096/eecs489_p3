#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

void pc(const char* c)
{
   while( *c )
      cout << *(c++);
   cout << endl;
}

int main(){
    char* filename1 = (char*) vm_map(nullptr, 0);
    char* filename2 = (char*) vm_map(nullptr, 0);
    char* filename3 = (char*) vm_map(nullptr, 0);
    char* filename4 = (char*) vm_map(nullptr, 0);
    char* filename5 = (char*) vm_map(nullptr, 0);
    char* filename6 = (char*) vm_map(nullptr, 0);
    pc(filename1);
    strcpy(filename1, "shakespeare.txt");
    pc(filename1);
    strcpy(filename2, "pikachu");
    strcpy(filename3, "charizard");
    strcpy(filename4, "squirtle");
    strcpy(filename5, "ash");
    pc(filename5);
    pc(filename1);
    pc(filename1);
    strcpy(filename1, "shakespeare");
    int pid = fork();
    if ( !pid ){
       strcpy(filename2, "dead_pikachu");
       strcpy(filename3, "dead_charizard"); 
    }else{
       strcpy(filename2, "rachu");
       strcpy(filename3, "mega charizard");
    }
    return 0;
}