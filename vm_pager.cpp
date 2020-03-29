#include "vm_pager.h"
#include "vm_arena.h"
#include "unordered_map"
#include <vector>
#include <queue>
#include <utility>
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <deque>
#include <tuple>
using namespace std;


// Refactor: inversion.erase

struct extra_info;
typedef pair<page_table_entry_t *, extra_info *> pte_deluxe; // a pte entry with extra info
typedef tuple<string, unsigned int, bool> valid_page_id;            // valid physical page unique idenditifier

/* ------------- Variables and Structs -------------------  */
char buffer[VM_PAGESIZE];
// extra info for page table entry
struct extra_info
{
    bool isswap;
    bool valid;
    bool resident;
    bool referenced;
    bool dirty;
    string filename;
    unsigned block;
    pid_t pid;
};
// info for each process
struct process_info
{
    page_table_t *process_page_table;
    vector<extra_info> pt_extension;
    size_t avail_vp;
    size_t sb_used;
};

struct orphan_info{
  bool dirty;
  bool referenced;
  unsigned ppn;  
};

queue<unsigned> sb_table;
unordered_map<string, unordered_map<unsigned, orphan_info> > orphans;
unordered_map<unsigned int, valid_page_id> resident_pages; // ppn -> valid_page_id
unordered_map<string, unordered_map<unsigned, vector<pte_deluxe>>> file_inversion;
unordered_map<unsigned int,vector<pte_deluxe> > swap_inversion;
// unordered_map<valid_page_id, vector<pte_deluxe> > inversion; // valid_page_id -> pte_deluxe
unsigned int num_memory_pages;
unsigned int avail_swap_blocks;
pid_t curr_pid;
unordered_map<pid_t, process_info *> arenas;
deque<unsigned int> clock_queue; // For now the element is the ref to vector of pte_deluxe in shared_pages
/* ------------- Variables and Structs -------------------  */
int success;

void vm_init(unsigned int memory_pages, unsigned int swap_blocks)
{
    success = 0;
    // Assign values to these variables
    num_memory_pages = memory_pages;
    avail_swap_blocks = swap_blocks;
    // Create and pin the "zero page"
    for (unsigned i = 0; i < swap_blocks; i++)
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
    for( unsigned i = 0; i < VM_ARENA_SIZE / VM_PAGESIZE; i++ ){
        child_info->process_page_table->ptes[i].write_enable = 0;
        child_info->process_page_table->ptes[i].read_enable = 0;
        child_info->process_page_table->ptes[i].ppage = 0;
    }
    child_info->pt_extension.resize(VM_ARENA_SIZE / VM_PAGESIZE);
    child_info->sb_used = 0; // Serve for assersion
    arenas[child_pid] = child_info;
    if(arenas.find(parent_pid) != arenas.end())
        child_info->avail_vp = arenas[parent_pid]->avail_vp; // Refactor
    else{
        child_info->avail_vp = 0;
        return 0;
    }
    
    // Eager check
    if (arenas[parent_pid]->sb_used > avail_swap_blocks) return -1;

    // For all vp in parent process, make a copy (almost)
    page_table_t *parent_table = arenas[parent_pid]->process_page_table;
    vector<extra_info> *parent_extension_table = &(arenas[parent_pid]->pt_extension);
    page_table_t *child_table = arenas[child_pid]->process_page_table;
    vector<extra_info> *child_extension_table = &(arenas[child_pid]->pt_extension);

    for (unsigned int i = 0; i < arenas[parent_pid]->avail_vp; i++)
    {
        page_table_entry_t *parent_entry = &(parent_table->ptes[i]);
        extra_info *parent_extra = &((*parent_extension_table)[i]);
        // cout << "page created: " << parent_extra->filename << "  " <<parent_extra->block << " " << inversion[parent_extra->filename][parent_extra->block].size()<< endl;
        if (parent_extra->isswap)
        {
            // cout << "copied virtual: " << i << " with physical: " << parent_entry->ppage << endl;
            // Make the parent page's write_enable 0
            parent_entry->write_enable = 0;

            child_table->ptes[i].ppage = parent_entry->ppage;
            child_table->ptes[i].read_enable = parent_entry->read_enable;
            child_table->ptes[i].write_enable = parent_entry->write_enable;
            (*child_extension_table)[i].valid = parent_extra->valid;
            (*child_extension_table)[i].resident = parent_extra->resident;
            (*child_extension_table)[i].referenced = parent_extra->referenced;
            (*child_extension_table)[i].dirty = parent_extra->dirty;
            // (*child_extension_table)[i].filename = parent_extra->filename;
            (*child_extension_table)[i].block = parent_extra->block;
            (*child_extension_table)[i].pid = child_pid;
            (*child_extension_table)[i].isswap = true;
            avail_swap_blocks--;
            arenas[child_pid]->sb_used++;
            if( parent_entry->ppage )
                swap_inversion[parent_extra->block].push_back(make_pair(&(child_table->ptes[i]), &(*child_extension_table)[i]));
        }
        else
        {
            child_table->ptes[i].ppage = parent_entry->ppage;
            child_table->ptes[i].read_enable = parent_entry->read_enable;
            child_table->ptes[i].write_enable = parent_entry->write_enable;
            (*child_extension_table)[i].valid = parent_extra->valid;
            (*child_extension_table)[i].resident = parent_extra->resident;
            (*child_extension_table)[i].referenced = parent_extra->referenced;
            (*child_extension_table)[i].dirty = parent_extra->dirty;
            (*child_extension_table)[i].filename = parent_extra->filename;
            (*child_extension_table)[i].block = parent_extra->block;
            (*child_extension_table)[i].pid = child_pid;
            (*child_extension_table)[i].isswap = false;
            file_inversion[parent_extra->filename][parent_extra->block].push_back(make_pair(&(child_table->ptes[i]), &(*child_extension_table)[i]));
        }
    }
    // cout << "child with num of " << arenas[child_pid]->avail_vp << " pages" << endl;
    return 0;
}

void vm_switch(pid_t pid)
{
    curr_pid = pid;
    page_table_base_register = arenas[pid]->process_page_table;
    // cout << "size2 is: " << inversion[""][0].size() << endl;
    // cout << "Size of next arenas" << arenas[pid]->avail_vp << endl;
}

void *vm_map(const char *filename, unsigned int block)
{
    page_table_t *table = arenas[curr_pid]->process_page_table;
    vector<extra_info> *extension_table = &(arenas[curr_pid]->pt_extension);
    unsigned index = arenas[curr_pid]->avail_vp;
    if ( filename )
        if( (size_t) filename > ( (size_t)VM_ARENA_BASEADDR + VM_ARENA_SIZE ) || (size_t) filename < (size_t) VM_ARENA_BASEADDR )
            return nullptr;
    if (index == VM_ARENA_SIZE / VM_PAGESIZE ) return nullptr;
    // Make one virtual page valid
    if (filename)
    {
        // Find the filename
        string pg_filename = "";
        unsigned curr_page_num = ( (size_t) filename - (size_t) VM_ARENA_BASEADDR) / VM_PAGESIZE;
        page_table_entry_t *entry = &(arenas[curr_pid]->process_page_table->ptes[curr_page_num]);
        extra_info *extra = &(arenas[curr_pid]->pt_extension[curr_page_num]);
        // cout << "fault in file map" << endl;
        bool file_pg_valid = vm_fault(filename, false) + 1;
        if( !file_pg_valid )
            return nullptr;
        char tmp_char = ((char*)vm_physmem)[(entry->ppage<<12) + (size_t) filename % VM_PAGESIZE];
        pg_filename += tmp_char;
        filename++;
        while(tmp_char)
        {
            if (((size_t) filename - (size_t) VM_ARENA_BASEADDR) / VM_PAGESIZE != curr_page_num)
            {
                curr_page_num = ( (size_t) filename - (size_t) VM_ARENA_BASEADDR) / VM_PAGESIZE;
                entry = &(arenas[curr_pid]->process_page_table->ptes[curr_page_num]);
                extra = &(arenas[curr_pid]->pt_extension[curr_page_num]);
                file_pg_valid = vm_fault(filename, false) + 1;
                if( !file_pg_valid )
                    return nullptr;
            }
            tmp_char = ((char*)vm_physmem)[((entry->ppage)<<12) + (size_t) filename % VM_PAGESIZE];
            pg_filename += tmp_char;
            filename++;
        }
        if (orphans.find(pg_filename) != orphans.end() && orphans[pg_filename].find(block) != orphans[pg_filename].end()) {
            table->ptes[index].ppage = orphans[pg_filename][block].ppn;
            if (!orphans[pg_filename][block].referenced)
            {
                table->ptes[index].write_enable = 0;
                table->ptes[index].read_enable = 0;        
            }
            else
            {
                table->ptes[index].write_enable = orphans[pg_filename][block].dirty;
                table->ptes[index].read_enable = 1;
            }
            (*extension_table)[index].valid = 1;
            (*extension_table)[index].resident = 1;
            (*extension_table)[index].referenced = orphans[pg_filename][block].referenced;
            (*extension_table)[index].dirty = orphans[pg_filename][block].dirty;
            (*extension_table)[index].filename = pg_filename;
            (*extension_table)[index].block = block;   
            (*extension_table)[index].pid = curr_pid;
            (*extension_table)[index].isswap = false;
            orphans[pg_filename].erase(block);
        }
        else if (file_inversion.find(pg_filename) != file_inversion.end() && file_inversion[pg_filename].find(block) != file_inversion[pg_filename].end()
            && file_inversion[pg_filename][block].size() > 0)
        {
            table->ptes[index].ppage = file_inversion[pg_filename][block][0].first->ppage;
            table->ptes[index].write_enable = file_inversion[pg_filename][block][0].first->write_enable;
            table->ptes[index].read_enable = file_inversion[pg_filename][block][0].first->read_enable;
            (*extension_table)[index].valid = file_inversion[pg_filename][block][0].second->valid;
            (*extension_table)[index].resident = file_inversion[pg_filename][block][0].second->resident;
            (*extension_table)[index].referenced = file_inversion[pg_filename][block][0].second->referenced;
            (*extension_table)[index].dirty = file_inversion[pg_filename][block][0].second->dirty; //dirty bit?
            (*extension_table)[index].filename = pg_filename;
            (*extension_table)[index].block = block;   
            (*extension_table)[index].pid = curr_pid;
            (*extension_table)[index].isswap = false;   
        }
        else{
            table->ptes[index].ppage = 0;
            table->ptes[index].write_enable = 0;
            table->ptes[index].read_enable = 0;
            (*extension_table)[index].valid = 1;
            (*extension_table)[index].resident = 0;
            (*extension_table)[index].referenced = 0;
            (*extension_table)[index].dirty = 0; //dirty bit?
            (*extension_table)[index].filename = pg_filename;
            (*extension_table)[index].block = block;  
            (*extension_table)[index].pid = curr_pid;
            (*extension_table)[index].isswap = false;
        }
        // cout<< " map file pages: " << pg_filename << " " << block << " with vp " << index << endl;
        file_inversion[pg_filename][block].push_back(make_pair(&(table->ptes[index]), &(*extension_table)[index]));
    }
    else
    {
        if (!avail_swap_blocks)
            return nullptr;
        table->ptes[index].ppage = 0;
        table->ptes[index].write_enable = 0;
        table->ptes[index].read_enable = 1;
        (*extension_table)[index].valid = 1;
        (*extension_table)[index].resident = 1;
        (*extension_table)[index].referenced = 1;
        (*extension_table)[index].dirty = 0;
        // (*extension_table)[index].filename = string("");
        (*extension_table)[index].pid = curr_pid;
        (*extension_table)[index].isswap = true;
        avail_swap_blocks--;
        arenas[curr_pid]->sb_used++;
    }
    arenas[curr_pid]->avail_vp++;
    return (void *)((char*)VM_ARENA_BASEADDR + index * VM_PAGESIZE);
    // return nullptr;
}

unsigned int bringto_mem_handler(string& filename, unsigned int block, bool isswap, const char *buffer )
{
    // O(n)
    unsigned int avai_ppn = 0;// = resident_pages.size() + 1;
    //cout << "number of resident pages " << resident_pages.size() << endl;
    // for (auto x: resident_pages)
    // {
    //     //cout << "This page is resident: " << inversion[x.second.first][x.second.second][0].first->ppage << endl;
    // }
    for( size_t i = 1; i < num_memory_pages; i++)
    {
        if( resident_pages.find(i) == resident_pages.end() ){
            avai_ppn = i;
            break;
        }
    }
    //cout << "Find the empty ppn (if any): " << avai_ppn << endl;
    // cout << clock_queue.size() << "||||" << resident_pages.size() << endl;
    if (resident_pages.size() == num_memory_pages - 1)
    {
        // cout << "Start eviction" << endl;
        valid_page_id id;
        // Find the victim
        while (true)
        {
            //cout << "In the loop" << endl;
            // if (resident_pages.find(clock_queue.front()) == resident_pages.end())
            // {
            //     // cout << "Deleted useless ppn from clock queue" << endl;
            //     clock_queue.pop();
            //     continue;
            // }
            id = resident_pages[clock_queue.front()];
            //cout << "Current candidate: ppn - " << clock_queue.front() << "block - " << get<1>(id) << endl;
            // cout << "candidate vp referenced: " << inversion[get<0>(id)][get<1>(id)][0].second->referenced << endl; 
            if (orphans.find(get<0>(id)) != orphans.end() && orphans[get<0>(id)].find(get<1>(id)) != orphans[get<0>(id)].end() ) 
            {
                if (!orphans[get<0>(id)][get<1>(id)].referenced)
                    break;
                orphans[get<0>(id)][get<1>(id)].referenced = 0;
            }
            else
            {
                if( get<2>(id) )//isswap
                {
                    if (!swap_inversion[get<1>(id)][0].second->referenced)
                    {
                        // cout << "Found the vicitim: " << clock_queue.front() << endl;
                        break;
                    }
                    for (auto &page : swap_inversion[get<1>(id)])
                    {
                        page.first->write_enable = 0;
                        page.first->read_enable = 0;
                        page.second->referenced = 0;
                    }
                }else{
                    if (!file_inversion[get<0>(id)][get<1>(id)][0].second->referenced)
                    {
                        // cout << "Found the vicitim: " << clock_queue.front() << endl;
                        break;
                    }
                    for (auto &page : file_inversion[get<0>(id)][get<1>(id)])
                    {
                        page.first->write_enable = 0;
                        page.first->read_enable = 0;
                        page.second->referenced = 0;
                    }
                }
            }
            auto tmp = clock_queue.front();
            clock_queue.pop_front();
            clock_queue.push_back(tmp);
        }
        // Write the content of evicted page into disk
        if (orphans.find(get<0>(id)) != orphans.end() && orphans[get<0>(id)].find(get<1>(id)) != orphans[get<0>(id)].end() ) {
            if (orphans[get<0>(id)][get<1>(id)].dirty)
            {
                // Write back
                success = file_write(get<0>(id).c_str(), get<1>(id), (char *)vm_physmem + clock_queue.front() * VM_PAGESIZE);
                if (success == -1) return 99999;
            }
            avai_ppn = clock_queue.front();
            clock_queue.pop_front();
            orphans[get<0>(id)].erase(get<1>(id));

        }
        else{
            pte_deluxe page;
            if (get<2>(id)) //isswap
                page = swap_inversion[get<1>(id)][0];
            else
                page = file_inversion[get<0>(id)][get<1>(id)][0];
            if (page.second->dirty)
            {
                // cout << "Started write back" << endl;
                if (page.second->isswap)
                    success = file_write(nullptr, page.second->block, (char *)vm_physmem + clock_queue.front() * VM_PAGESIZE);
                else
                    success = file_write(page.second->filename.c_str(), page.second->block, (char *)vm_physmem + clock_queue.front() * VM_PAGESIZE);
                if ( success == -1 )
                    return 99999;
            }
            // cout << "Check ppn: " << avai_ppn << endl;
            avai_ppn = clock_queue.front();
            // cout << avai_ppn << endl;
            // Update the clock queue
            clock_queue.pop_front();
            if (get<2>(id)) //isswap
            {
                for (auto &page : swap_inversion[get<1>(id)]) // Modify the info of the evicted page
                {
                    page.first->read_enable = 0;
                    page.first->write_enable = 0;
                    page.second->resident = 0;
                    page.second->referenced = 0;
                    page.second->dirty = 0;
                }
            }else{
                for (auto &page : file_inversion[get<0>(id)][get<1>(id)]) // Modify the info of the evicted page
                {
                    page.first->read_enable = 0;
                    page.first->write_enable = 0;
                    page.second->resident = 0;
                    page.second->referenced = 0;
                    page.second->dirty = 0;
                }
            }
        }
    }
    // Copy content to this physical page
    // cout <<"read file: "<< filename << " with block: " <<block << endl; 
    if (!buffer){
        if ( isswap )
            success = file_read(nullptr, block, (char *)vm_physmem + avai_ppn * VM_PAGESIZE);
        else
            success = file_read(filename.c_str(), block, (char *)vm_physmem + avai_ppn * VM_PAGESIZE);
        if( success == -1 )
        {
            resident_pages.erase(avai_ppn);
            return 99999;
        }
    }
    else
        memcpy((char *)vm_physmem + avai_ppn * VM_PAGESIZE, buffer, VM_PAGESIZE);
    // Update resident pages
    resident_pages[avai_ppn] = make_tuple(filename, block, isswap);
    // Update the clock queue
    // cout << "newly pushed ppn: " << avai_ppn << endl;
    clock_queue.push_back(avai_ppn);
    // cout << "phsymem size: "<< resident_pages.size();
    // The modification of virtual pages pointing to this new resident page should be handled else where
    return avai_ppn;
}

int read_handler(uintptr_t index)
{
    page_table_entry_t *entry = &(arenas[curr_pid]->process_page_table->ptes[index]);
    extra_info *extra = &(arenas[curr_pid]->pt_extension[index]);
    // pte_deluxe this_page = make_pair(entry, extra);
    // for( unsigned i = 0; i < 100000000; i++)
    //     continue;
    if ( !entry->ppage && extra->isswap )
        return 0;
    // cout << "to read ppn: " << extra->filename << extra->block << " with ppn: " <<entry->ppage<< " virtual page: "<< index << endl;
    if (extra->resident)
    {
        // cout << "read a resident page" << endl;
        // cout << extra->filename << " " << extra->block << inversion[extra->filename][extra->block].size() << endl;
        if (extra->dirty && !(extra->isswap && swap_inversion[extra->block].size() > 1))
        {
            if(extra->isswap){
                for (auto &page : swap_inversion[extra->block]) //reference needed?
                    page.first->write_enable = 1;
            }else{
                for (auto &page : file_inversion[extra->filename][extra->block]) //reference needed?
                    page.first->write_enable = 1;
            }
        }
        if( extra->isswap ){
            for (auto &page : swap_inversion[extra->block]) //reference needed? add new element in map?
            {
                page.first->read_enable = 1;
                page.second->referenced = 1;
            }
        }else{
            for (auto &page : file_inversion[extra->filename][extra->block]) //reference needed? add new element in map?
            {
                page.first->read_enable = 1;
                page.second->referenced = 1;
            }
        }
    }
    else
    {
        // Check if there is free physical page
        unsigned int new_ppn = bringto_mem_handler(extra->filename, extra->block, extra->isswap, nullptr);
        if ( success == -1 )
        {
            success = 0;
            return -1;
        }
        // Modify virtual pages pointing to this new resident page
        // cout << "virtual to change " << extra->filename << " " << extra->block << " with ppn: " << new_ppn << " "<< inversion[extra->filename][extra->block].size() << endl;
        if(extra->isswap){
            for (auto &page : swap_inversion[extra->block])
            {
                // cout << "changing " << extra->filename << " " << extra->block << " " <<page.second << endl; 
                page.first->ppage = new_ppn;
                page.first->read_enable = 1;
                page.second->resident = 1;
                page.second->referenced = 1;
            }
        }else{
            for (auto &page : file_inversion[extra->filename][extra->block])
            {
                // cout << "changing " << extra->filename << " " << extra->block << " " <<page.second << endl; 
                page.first->ppage = new_ppn;
                page.first->read_enable = 1;
                page.second->resident = 1;
                page.second->referenced = 1;
            }
        }
        // Done
    }
    return 0;
}

//parent or child?
int write_handler(uintptr_t index)
{
    // cout << "Start write handler" << endl;
    page_table_entry_t *entry = &(arenas[curr_pid]->process_page_table->ptes[index]);
    extra_info *extra = &(arenas[curr_pid]->pt_extension[index]);
    // cout << " index of read in writer_handler is : " << index << endl;
    read_handler(index);
    if ( extra->isswap && (swap_inversion[extra->block].size() > 1 || entry->ppage == 0))
    {
        // cout << "Start copy on write" << endl;
        unsigned int old_block = extra->block;
        unsigned old_ppn = entry->ppage;
        // copy on write
        // read into buffer
        // read_handler(index);
        memcpy(buffer, (char *)vm_physmem + entry->ppage * VM_PAGESIZE, VM_PAGESIZE);
        // Assign available swap block to the leaf page
        extra->block = sb_table.front();
        // cout << "Assign block: " << sb_table.front() << endl;
        sb_table.pop();
        // copy to physmem, and evict if needed
        unsigned new_ppn = bringto_mem_handler(extra->filename, extra->block, extra->isswap, buffer);
        if ( success == -1 )
        {
            success = 0;
            return -1;
        }
        //change state
        //the virtual page get changed (leaf)
        entry->ppage = new_ppn;
        extra->resident = true;
        swap_inversion[extra->block].push_back(make_pair(entry, extra));
        //Handle trunck
        // Refactor???
        if (old_ppn != 0)
        {
            for (unsigned int i = 0; i < swap_inversion[old_block].size(); ++i)
            {
                if (swap_inversion[old_block][i].first->ppage == new_ppn && swap_inversion[old_block][i].second->resident){
                    swap_inversion[old_block].erase(swap_inversion[old_block].begin() + i);
                    break;
                }
            }
            if (swap_inversion[old_block].size() == 1)
            {
                pte_deluxe tmp = swap_inversion[old_block][0];
                if (tmp.second->dirty == 1)
                    tmp.first->write_enable = 1;
            }
        }
    }
    // If the page is not resident and is not shared
    // else if (!extra->resident)
    // {
    //     read_handler(index);
    // }
    //cout << "Start" << endl;
    if(extra->isswap)
    {
        for (auto& page: swap_inversion[extra->block])
        {
            page.second->valid = 1;
            page.second->referenced = 1;
            page.second->dirty = 1;
            page.second->resident = 1;
            page.first->read_enable = 1;
            page.first->write_enable = 1;
        }
    }else{
        for (auto& page: file_inversion[extra->filename][extra->block])
        {
            page.second->valid = 1;
            page.second->referenced = 1;
            page.second->dirty = 1;
            page.second->resident = 1;
            page.first->read_enable = 1;
            page.first->write_enable = 1;
        }
    }
    return 0;
}

int vm_fault(const void *addr, bool write_flag)
{
    // for(unsigned i = 0; i < 100000000; i++)
    //     continue;
    uintptr_t index = ((uintptr_t)addr - (uintptr_t)VM_ARENA_BASEADDR) / VM_PAGESIZE;
    // invalid page
    //debug
    page_table_t *table = arenas[curr_pid]->process_page_table;
    vector<extra_info> *extension_table = &(arenas[curr_pid]->pt_extension);
    // cout << "full vp table ---------: " << endl;
    // for( unsigned i = 0; i < 6; i++ )
    // {
    //     cout << &(*extension_table)[i] << table[i].ptes->ppage << table[i].ptes->read_enable << table[i].ptes->write_enable << (*extension_table)[i].resident << endl;
    // }
    // cout<< "-------------------" << endl;
    //debug end
    if (!arenas[curr_pid]->pt_extension[index].valid)
        return -1;

    //
    // cout << write_flag << endl;
    if (!write_flag)
        return read_handler(index);
    return write_handler(index);
}

void vm_destroy()
{
    page_table_t *table = arenas[curr_pid]->process_page_table;
    vector<extra_info> *extension_table = &(arenas[curr_pid]->pt_extension);
    //loop through all virtual pages in current process's arena
    for (unsigned int i = 0; i < VM_ARENA_SIZE / VM_PAGESIZE; i++)
    {
        page_table_entry_t *entry = &(table->ptes[i]);
        extra_info *extra = &((*extension_table)[i]);
        if (!extra->valid)
            break;
        // cout << "destroied virtual: " << i << " with physical: " << entry->ppage << endl;    
        //swap block
        // cout << "sizeq is " << inversion[""][0].size() << endl;
        
        if (extra->isswap)//cannot campare with ""
        {
            if (!entry->ppage)
                continue;
            // Refactor?
            //cout << "filename: " << extra->filename << " block: " << extra->block << " number of bros " <<  inversion[extra->filename][extra->block].size() << endl;
            if (swap_inversion[extra->block].size() == 1)
            {
                for( auto itr = clock_queue.begin(); itr != clock_queue.end(); itr++ )
                {
                    if ( *itr == entry->ppage )
                    {
                        clock_queue.erase(itr);
                        break;
                    }
                }
                sb_table.push(extra->block);
                avail_swap_blocks++;
                if (extra->resident)
                    resident_pages.erase(entry->ppage);
                swap_inversion.erase(extra->block);
                //cout << "Resident pages erased: ppage: " << entry->ppage << " and block number: " << extra->block << endl;
            }
            else
            {
                avail_swap_blocks++;
                for (unsigned int i = 0; i < swap_inversion[extra->block].size(); ++i)
                {
                    if (swap_inversion[extra->block][i].second->pid == curr_pid){
                        swap_inversion[extra->block].erase(swap_inversion[extra->block].begin() + i);
                        break;
                    }
                }
                if (swap_inversion[extra->block].size() == 1 && swap_inversion[extra->block][0].second->dirty == 1 
                    && swap_inversion[extra->block][0].second->referenced == 1)
                {
                    swap_inversion[extra->block][0].first->write_enable = 1;
                }
            }
            
        }
        //file block
        else
        {
            // TODO
            // resident?
            // Refactor
            if (extra->resident && inversion[extra->filename][extra->block].size() == 1)
            {
                //Add to orphan
                orphans[extra->filename][extra->block].referenced = extra->referenced;
                orphans[extra->filename][extra->block].dirty = extra->dirty;
                orphans[extra->filename][extra->block].ppn = entry->ppage;
                // if (extra->dirty)
                //     file_write(extra->filename.c_str(), extra->block, (char*)vm_physmem + entry->ppage * VM_PAGESIZE);
                // // // Delete the corresponding entry from resident_pages
                // // resident_pages.erase(entry->ppage);
            }
            for (unsigned int i = 0; i < inversion[extra->filename][extra->block].size(); i++)
            {
                if (inversion[extra->filename][extra->block][i].second->pid == curr_pid){
                    inversion[extra->filename][extra->block].erase(inversion[extra->filename][extra->block].begin() + i);
                    break;
                }
                // if (inversion[extra->filename][extra->block].size() == 0 )
                //     inversion[extra->filename].erase(extra->block);
            }
        }
    }
    delete table;
    (*extension_table).clear();
    delete arenas[curr_pid];
    arenas.erase(curr_pid);
}