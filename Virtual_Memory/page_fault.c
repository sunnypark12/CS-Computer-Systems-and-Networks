#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table

    pte_t* fault_pt_entry = (pte_t*)(mem + PTBR * PAGE_SIZE) + vaddr_vpn(addr);
    pfn_t new_pfn = free_frame();
    (*fault_pt_entry).pfn = new_pfn;
    (*fault_pt_entry).dirty = 0;
    (*fault_pt_entry).valid = 1;
    

    fte_t * new_ft = new_pfn + frame_table;
    (*new_ft).mapped = 1;
    (*new_ft).referenced = 1;
    (*new_ft).protected = 0;
    (*new_ft).vpn = vaddr_vpn(addr);
    (*new_ft).process = current_process;
    
    uint8_t *new_frame = mem + new_pfn * PAGE_SIZE;

    if (swap_exists(fault_pt_entry)) {
        swap_read(fault_pt_entry, new_frame);
    } else {
        (void) memset(new_frame, 0, PAGE_SIZE);
    }
    stats.page_faults ++;
}

#pragma GCC diagnostic pop

