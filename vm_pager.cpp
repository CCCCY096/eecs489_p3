#include "vm_pager.h"
#include "vm_arena.h"
#include "unordered_map"
#include <vector>
#include <queue>
#include <utility>
#include <string>
using namespace std;

typedef pair<page_table_entry_t *, extra_info *> pte_deluxe; // a pte entry with extra info
typedef pair<string, unsigned int> valid_page_id;            // valid physical page unique idenditifier

/* ------------- Variables and Structs -------------------  */
char buffer[VM_PAGESIZE];
// extra info for page table entry
struct extra_info
{
    bool valid;
    bool resident;
    bool referenced;
    bool dirty;
    string filename;
    unsigned block;
};
// info for each process
struct process_info
{
    page_table_t *process_page_table;
    vector<extra_info> pt_extension;
    //void *arena_curr_addr;
};
queue<unsigned> sb_table;
unordered_map<unsigned int, valid_page_id> resident_pages; // ppn -> valid_page_id
unordered_map<string, unordered_map<unsigned, vector<pte_deluxe>>> inversion;
// unordered_map<valid_page_id, vector<pte_deluxe> > inversion; // valid_page_id -> pte_deluxe
unsigned int num_memory_pages;
unsigned int num_swap_blocks;
pid_t curr_pid;
unordered_map<pid_t, process_info *> arenas;
queue<unsigned int> clock_queue; // For now the element is the ref to vector of pte_deluxe in shared_pages
/* ------------- Variables and Structs -------------------  */

void vm_init(unsigned int memory_pages, unsigned int swap_blocks)
{
    // Assign values to these variables
    num_memory_pages = memory_pages;
    num_swap_blocks = swap_blocks;
    // Create and pin the "zero page"
    for (unsigned i = 0; i < swap_blocks; i++ )
        sb_table.push(i);
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
    child_info->pt_extension.resize(VM_ARENA_SIZE / VM_PAGESIZE);
    arenas[child_pid] = child_info;
    return 0;
}

void vm_switch(pid_t pid)
{
    curr_pid = pid;
    page_table_base_register = arenas[pid]->process_page_table;
}

int vm_fault(const void *addr, bool write_flag)
{
    uintptr_t index = ((uintptr_t)addr - (uintptr_t)VM_ARENA_BASEADDR) / VM_PAGESIZE;
    // invalid page
    if (!arenas[curr_pid]->pt_extension[index].valid)
        return -1;

    //
    if (!write_flag)
        return read_handler(index);
    return write_handler(index);
}



void *vm_map(const char *filename, unsigned int block)
{
    // Make one virtual page valid
}

int read_handler(uintptr_t index)
{
    page_table_entry_t *entry = &(arenas[curr_pid]->process_page_table->ptes[index]);
    extra_info *extra = &(arenas[curr_pid]->pt_extension[index]);
    // pte_deluxe this_page = make_pair(entry, extra);
    if (extra->resident)
    {
        if (extra->dirty)
        {
            for (auto page & : inversion[extra->filename][extra->block]) //reference needed?
            {
                page->first->write_enable = 1;
            }
        }
        for (auto page & : inversion[extra->filename][extra->block]) //reference needed?
        {
            page->first->read_enbale = 1;
            page->second->referenced = 1;
        }
    }
    else
    {
        // Check if there is free physical page
        evict_handler(extra->filename.c_str(), extra->block);
        // Modify virtual pages pointing to this new resident page
        for (auto page&: inversion[extra->filename][extra->block])
        {
            page.first->read_enable = 1;
            page.second->resident = 1;
            page.second->referenced = 1;
        }
        // Done
    }
    return 0;
}

//parent or child?
int write_handler(uintptr_t index)
{
    page_table_entry_t *entry = &(arenas[curr_pid]->process_page_table->ptes[index]);
    extra_info *extra = &(arenas[curr_pid]->pt_extension[index]);
    if (inversion[extra->filename][extra->block].size() > 1)
    {
        //copy on write
        //read into buffer
        memccpy(buffer, vm_physmem + entry->ppage * VM_PAGESIZE);
        evict_handler(nullptr, 0, buffer);
        // Since this physical page is shared, first step is to create a new physical page
    }
}

void evict_handler(const char* filename, unsigned int block, const char *buffer)
{
    unsigned int avai_ppn = resident_pages.size();
    if (resident_pages.size() == num_memory_pages) {
        valid_page_id id;
        // Find the victim
        while (true)
        {
            id = resident_pages[clock_queue.front();
            if (!inversion[id.first][id.second][0].second->referenced) break;
            for (auto page&: inversion[id.first][id.second])
            {
                page.second->referenced = 0;
            }
            auto tmp = clock_queue.front();
            clock_queue.pop();
            clock_queue.push(tmp);
        }
        // Write the content of evicted page into disk
        auto page& = inversion[id.first][id.second][0];
        if (page.second->dirty)
        {
            file_write(page.second->filename, page.second->block, vm_physmem + clock_queue.front() * VM_PAGESIZE);
        }
        avai_ppn = clock_queue.front();
        // Update the clock queue
        clock_queue.pop();
        for (auto page&: inversion[id.first][id.second]) // Modify the info of the evicted page
        {
            page.first->read_enable = 0;
            page.first->write_enable = 0;
            page.second->resident = 0;
            page.second->referenced = 0;
            page.second->dirty = 0;
        }
    }
    // Copy content to this physical page
    if(buffer)
        file_read(filename, block, vm_physmem + avai_ppn * VM_PAGESIZE);
    else
        memccpy( vm_physmem + avai_ppn * VM_PAGESIZE, buffer);
    // Update resident pages
    resident_pages[avai_ppn] = make_pair<string, unsigned int>(to_string(filename), block);
    // Update the clock queue
    clock_queue.push(avai_ppn);
    // The modification of virtual pages pointing to this new resident page should be handled else where 
}