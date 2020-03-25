#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <vector>

using namespace std;

int main(){
    vector<char*> info;
    for( unsigned i = 0; i < 1024; i++ )
        info[i] = (char*) vm_map(nullptr, 0);
    for( unsigned i = 0; i < 1024; i++)
        *(info[i]) = 1;
    return 0;
}