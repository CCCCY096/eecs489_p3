#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main()
{
//2 swap in mem
char *f1 = (char*)vm_map(nullptr, 0);
char *f2 = (char*)vm_map(nullptr, 0);
f1[4095] = 'd';
f2[0] = 'a';
f2[1] = 't';
f2[2] = 'a';
f2[3] = '1';
f2[4] = '.';
f2[5] = 'b';
f2[6] = 'i';
f2[7] = 'n';
f2[8] = '\0';
char *file = (char*)vm_map( f1 + 4095, 0 );
cout << "2 sawp in mem: " << *file << endl;

//1 swap in mem, 1 in disk
char* tmp1 = (char*)vm_map(nullptr,0);
*tmp1 = 'h';
file = (char*)vm_map( f1 + 4095, 0 );
cout << "1 sawp in mem, 1 swap in disk: " << *file << endl;

// 2 swap in disk
char* tmp2 = (char*)vm_map(nullptr,0);
*tmp2 = 'h';
*tmp1 = 'h';
file = (char*)vm_map( f1 + 4095, 0 );
cout << "2 sawp in disk: " << *file << endl;

//2 file in mem
char* f3 = (char*)vm_map(f1 + 4095, 0);
char* f4 = (char*)vm_map(f1 + 4095, 1);
f3[4095] = 'd';
f4[0] = 'a';
f4[1] = 't';
f4[2] = 'a';
f4[3] = '2';
f4[4] = '.';
f4[5] = 'b';
f4[6] = 'i';
f4[7] = 'n';
f4[8] = '\0';
file = (char*)vm_map( f3 + 4095, 0 );
cout << "2 file block in mem: " << *file << endl;

//1 file in mem, 1 in disk
*tmp1 = 'h';
file = (char*)vm_map( f3 + 4095, 0 );
cout << "1 file in mem, 1 file in disk: " << *file << endl;


// 2 file in disk
*tmp2 = 'h';
*tmp1 = 'h';
file = (char*)vm_map( f3 + 4095, 0 );
cout << "2 file in disk: " << *file << endl;

}