#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){
    char f[] = "data1.bin";
    char* filaname = (char*)vm_map(f, 0);
    return 0;
}