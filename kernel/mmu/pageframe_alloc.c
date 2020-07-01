#include "builddef.h"
#include "kheap.h"  // kmalloc_align()
#include "kinfo.h"
#include "paging.h"
#include "stddef.h"
#include "utils/bitmap.h"
#include "utils/debug.h"

#define VIRTUAL_ADDR_SIZE 4294967296  // 4GiB

private
int _is_initialized = 0;
// Bitmap representing allocated phys pages: 0 means available, 1 means already allocated
private
unsigned char *_pageframe_bitmap = NULL;
private
unsigned int _pages_total_phys = 0;  // Total pages in the _pageframe_bitmap

private
void pageframe_alloc_set_page(unsigned int page_no) { bitmap_set_bit(_pageframe_bitmap, page_no); }

private
int pageframe_alloc_get_page(unsigned int page_no) { return bitmap_get_bit(_pageframe_bitmap, page_no); }

private
void pageframe_alloc_set_pages(unsigned int page_start, unsigned int pages) {
    bitmap_set_bits(_pageframe_bitmap, page_start, pages);
}

private
void pageframe_alloc_clear_page(unsigned int page_no) { bitmap_clear_bit(_pageframe_bitmap, page_no); }

private
unsigned int pageframe_addr_from_page(unsigned int page_no) { return (page_no * 4096); }

private
unsigned int page_from_addr(unsigned int addr) { return (addr / 4096); }

private
void *pageframe_alloc_bestfit(unsigned int pages) {
    void *ret = NULL;

    // Check every bit to find satisfactory free pages
    // Check 1 byte at a time for performance
    for (unsigned int i = 0; i < _pages_total_phys / 8; i++) {
        if (_pageframe_bitmap[i] == 0xff) {  // No available pages (aka no available bits in byte)
            // Carry on to next byte
        } else {
            // Now check the byte whether it has enough available consecutive bits

            // Find most consecutive bits in byte (e.g. 01001000)
            // Find best match (e.g. request 2 pages, provide shortest 2 free bits)
            int j = 0, best_fit_len = 9, best_fit_pos = -1, cur_len = 0, last_one = -1;
            for (j = 0; j < 8; j++) {
                if ((_pageframe_bitmap[i] & (1 << j)) == 0) {
                    cur_len++;
                    if (j == 7) {
                        if (best_fit_len > cur_len && cur_len >= (int)pages) {
                            best_fit_len = cur_len;
                            best_fit_pos = last_one + 1;
                        }
                    }
                } else {
                    if (best_fit_len > cur_len && cur_len >= (int)pages) {
                        best_fit_len = cur_len;
                        best_fit_pos = last_one + 1;
                    }
                    cur_len = 0;
                    last_one = j;
                }
            }

            if (best_fit_pos >= 0) {  // We got enough available bits in byte
                unsigned int page_no = i * 8 + best_fit_pos;
                // Mark pages as allocated in bitmap
                pageframe_alloc_set_pages(page_no, pages);
                ret = (void *)pageframe_addr_from_page(page_no);
                break;
            }
        }
    }

    return ret;
}

// Number of pages is always bigger than 8
private
void *pageframe_alloc_firstfit(unsigned int pages) {
    void *ret = NULL;
    unsigned int page_no = 0;

    // Find the first available page quickly.
    for (unsigned int i = 0; i < _pages_total_phys / 8; i++) {
        if (_pageframe_bitmap[i] == 0xff) {
            page_no += 8;
        } else {
            break;
        }
    }

    // Treat the whole pageframe bitmap as a long bitstring.
    // Loop next bits to check for space for remaining pages.
    // If not satisfy, skip to next available bit and start over.
    // If reaches the end, that means no more space available.
    unsigned int cur_len = 0;
    for (unsigned int i = page_no; i < _pages_total_phys; i++) {
        if (pageframe_alloc_get_page(i) == 0) {
            cur_len++;
            if (cur_len == pages) {
                pageframe_alloc_set_pages(page_no, pages);
                ret = (void *)pageframe_addr_from_page(page_no);
                break;
            }
        } else {
            page_no = page_no + cur_len + 1;
            cur_len = 0;
        }
    }

    return ret;
}

// Allocate and init _pageframe_bitmap
private
void pageframe_alloc_init() {
    struct kinfo *kinfo = get_kernel_info();
    _pages_total_phys = (kinfo->phys_mem_upper * 1024) / 4096;
    // 1 bit represents 1 page in phys mem (4 KiB). We assign just enough memory to contain the bitmap of physical
    // memory.
    _pageframe_bitmap = kmalloc_align(_pages_total_phys / 8, 4096);

    // Reserved Kernel data area (1024 pages - 4 MiB) starting from 0x0.
    for (unsigned int i = 0; i < 1024; i++) {
        pageframe_alloc_set_page(i);
    }

    _dbg_log("Total pageframes: %u\n", _pages_total_phys);
    _is_initialized = 1;
}

public
void *pageframe_alloc(unsigned int pages) {
    if (!_is_initialized) {
        pageframe_alloc_init();
    }
    void *ret = NULL;

    if (pages > 8) {  // First fit algo if more than 8 pages requested
        ret = pageframe_alloc_firstfit(pages);
    } else {  // Fewer than 8 pages requested
        ret = pageframe_alloc_bestfit(pages);
    }

    _dbg_log("[MMU]%u pages requested. Return: [0x%x]\n", pages, ret);
    // If out of memory (not enough pages), ret is NULL
    return ret;
}

public
void pageframe_free(void *phys_addr, unsigned int pages) {
    if (!_is_initialized) return;

    unsigned int page_no = page_from_addr((unsigned int)((char *)phys_addr));

    for (int i = 0; i < (int)pages; i++) {  // Free 1 page at a time (which represents 4KiB)
        int page_status = pageframe_alloc_get_page(page_no + i);

        if (page_status == 1) {
            pageframe_alloc_clear_page(page_no + i);
        } else {
            _dbg_log("Error trying to free already freed frame.\n");
            // Handle error trying to free an already freed page
        }
    }
}
