#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char *block0 = (char *)vm_map(nullptr, 0);
    *block0 = 'c';
    char* block1 = (char*) vm_map(nullptr, 0);
    *block1 = 'c';
    if (fork())
    {
        
        *block0 = 'c';
        *block1 = 'c';
    }
}