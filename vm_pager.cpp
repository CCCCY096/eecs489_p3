#include "vm_pager.h"
#include "vm_arena.h"
#include "unordered_map"
#include <vector>
#include <queue>
#include <utility>
using namespace std;

typedef pair<page_table_entry_t*, extra_info*> pte_deluxe; // a pte entry with extra info
typedef pair<string, unsigned int> valid_page_id;

// How many buffers should we have
// Should clock queue consist of physical pages or virtual pages?
// A newly mapped page is resident or not? 

/* ------------- Variables and Structs -------------------  */
// extra info for page table entry
struct extra_info{
    bool valid;
    bool resident;
    bool referenced;
    bool dirty;
    unsigned counter;
};
// info for each process
struct process_info
{
    page_table_t *process_page_table;
    vector<extra_info> pt_extension;
    //void *arena_curr_addr;
    
};

unordered_map<valid_page_id, vector<pte_deluxe> > inversion;
// vector< vector < pte_deluxe > > physical_inversion;
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
    // for (unsigned int i = 0; i < VM_PAGESIZE; ++i)
    // {
    //     ((char *)vm_physmem)[i] = 0;
    // }
}

int vm_create(pid_t parent_pid, pid_t child_pid)
{
//     // Eager swap reservation check
//     // Leave it for now   

//     // Refactor? Too much duplicate codes
//     page_table_t *child_ptbr = new page_table_t;
//     info *child_info = new info;
//     // child_info->arena_curr_addr = VM_ARENA_BASEADDR;
//     child_info->process_page_table = new page_table_t;
//     arenas[child_pid] = child_info;
//     // If parent is not mangaged by pager
//     if (arenas.find(parent_pid) == arenas.end())
//     {
//         // Then also add both parent to our management
//         info *parent_info = new info;
//         parent_info->arena_curr_addr = VM_ARENA_BASEADDR;
//         parent_info->ptbr = new page_table_t;
//         arenas[parent_pid] = parent_info;
//     }
    return 1;
}

void vm_switch(pid_t pid)
{
    curr_pid = pid;
    page_table_base_register = arenas[pid]->ptbr;
}

int vm_fault(const void* addr, bool write_flag)
{
    // Acess the PTE

}

void *vm_map(const char *filename, unsigned int block)
{
    // Make one virtual page valid

}

