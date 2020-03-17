#include "vm_pager.h"
#include "vm_arena.h"
#include "unordered_map"
#include <vector>
#include <queue>
#include <utility>
using namespace std;

typedef pair<page_table_entry_t*, extra_info*> pte_deluxe; // a pte entry with extra info
typedef pair<string, unsigned int> valid_page_id; // valid physical page unique idenditifier

/* ------------- Variables and Structs -------------------  */
// extra info for page table entry
struct extra_info{
    bool valid;
    bool resident;
    bool referenced;
    bool dirty;
};
// info for each process
struct process_info
{
    page_table_t *process_page_table;
    vector<extra_info> pt_extension;
    //void *arena_curr_addr;
    
};
unordered_map<unsigned int, valid_page_id> resident_pages; // ppn -> valid_page_id
unordered_map<valid_page_id, vector<pte_deluxe> > inversion; // valid_page_id -> pte_deluxe
unsigned int num_memory_pages;
unsigned int num_swap_blocks;
pid_t curr_pid;
unordered_map<pid_t, process_info*> arenas;
queue<unsigned int> clock_queue; // For now the element is the ref to vector of pte_deluxe in shared_pages
/* ------------- Variables and Structs -------------------  */


void vm_init(unsigned int memory_pages, unsigned int swap_blocks)
{
    // Assign values to these variables
    num_memory_pages = memory_pages;
    num_swap_blocks = swap_blocks;
    // Create and pin the "zero page"
    for (unsigned int i = 0; i < VM_PAGESIZE; ++i)
    {
        ((char *)vm_physmem)[i] = 0;
    }
}

int vm_create(pid_t parent_pid, pid_t child_pid)
{
//     // Eager swap reservation check
//     // Leave it for now   

        
    // 4 credits version: assume child process has a empty arena
    process_info *child_info = new process_info;
    child_info->process_page_table = new page_table_t;
    child_info->pt_extension.resize(VM_ARENA_SIZE/VM_PAGESIZE);
    arenas[child_pid] = child_info;
    return 0;
}

void vm_switch(pid_t pid)
{
    curr_pid = pid;
    page_table_base_register = arenas[pid]->process_page_table;
}

int vm_fault(const void* addr, bool write_flag)
{
    uintptr_t index = ((uintptr_t) addr - (uintptr_t) VM_ARENA_BASEADDR) / VM_PAGESIZE;
    // invalid page
    if ( !arenas[curr_pid]->pt_extension[index].valid )
        return -1;
    
    // 
    if (!write_flag) return read_handler(index);
    return write_handler(index);
}

void *vm_map(const char *filename, unsigned int block)
{
    // Make one virtual page valid
    
}




int read_handler(uintptr_t index){
    // page_table_entry_t* entry = &(arenas[curr_pid]->process_page_table->ptes[index]);
    // extra_info * extra = &(arenas[curr_pid]->pt_extension[index]);
    // pte_deluxe this_page = make_pair(entry, extra);
    
}


int write_handler(uintptr_t index){

}