#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "disk.h"
#include "pagetable.h"

PT pageTable;
Invert invert_table[MAX_FRAME];
TLB tlb[MAX_PID][TLB_ENTRY/2];

FTRK freq[MAX_FRAME];
int freePageNo=0;

int counter=0;

// If the page is already in the physical memory (page hit), return frame number.
// otherwise (page miss), return -1.
int is_page_hit(int pid, int pageNo, char type)
{
		if(pageTable.entry[pid][pageNo].valid == true)
		{
				
				if(pageTable.entry[pid][pageNo].dirty==true)			
				{	
					disk_write(pageTable.entry[pid][pageNo].frameNo, pid,pageNo);
					
				}
				
				pageTable.stats.hitCount++;
				
				InsertAtHead(pageNo);
				int i=0;
				for(i=0;i<MAX_FRAME;i++)
				{
					if(pageNo==freq[i].inv.page)
					{	
						freq[i].f.frequency++;
						freq[i].f.old++;
					}
						
				}
				return pageTable.entry[pid][pageNo].frameNo;

			
		}
		else				
			return -1;
}

int pagefault_handler(int pid, int pageNo, char type, bool *hit)
{
		int repl,i;
		disk_read(pageTable.entry[pid][pageNo].frameNo, pid,pageNo);
		if(type=='W')			
				{	
					pageTable.entry[pid][pageNo].dirty=true;
					
					
				}
				
		if(counter<MAX_FRAME)
		{
			
			pageTable.entry[pid][pageNo].valid = true;
			pageTable.stats.missCount++;
			pageTable.entry[pid][pageNo].frameNo = pageNo+2;
			invert_table[counter].pid=pid;
			invert_table[counter].page=pageNo;
			freq[counter].inv.pid=pid;
			freq[counter].inv.page=pageNo;
			freq[counter].f.frequency=1;
			freq[counter].f.old=1;
			InsertAtHead(pageNo);
			counter++;
			
		}
		else
		{	
			for(i=0;i<MAX_FRAME;i++)
			{
				inv_pg[i]=invert_table[i].page;
				lfu_page[i]=freq[i].inv.page;
				lfu_freq[i]=freq[i].f.frequency;
				lfu_old[i]=freq[i].f.old;
				
			}
			
			repl=page_replacement();
			for(i=0;i<MAX_FRAME;i++)
			{
				if(repl==i)
				{
					freq[i].inv.pid=pid;
					freq[i].inv.page=pageNo;
					freq[i].f.frequency=1;
					freq[i].f.old=1;
				}
				else
				{
					freq[i].f.old++;
				}
				
			}
			
			
			
			
			pageTable.entry[pid][pageNo].valid = true;
			pageTable.stats.missCount++;
			pageTable.entry[pid][pageNo].frameNo = (pageNo << 8);	
			pageTable.entry[invert_table[repl].pid][invert_table[repl].page].valid = false;
			invert_table[repl].pid=pid;
			invert_table[repl].page=pageNo;
			
			
			InsertAtHead(pageNo);
				
			
			
		}
	return pageTable.entry[pid][pageNo].frameNo;
}

// If the page is in TLB (tlb hit), return frame number.
// otherwise (tlb miss), return -1.
int is_TLB_hit(int pid, int pageNo, int type)
{
		return -1;
}

int MMU(int pid, int addr, char type, bool *hit, bool *tlbhit)
{
		//printf("====================================MMU=====================================\n");
		int frameNo;
		int pageNo = (addr >> 8);
		int offset = addr - (pageNo << 8);
		
		if(pageNo > MAX_PAGE) {
				printf("invalid page number (MAX_PAGE = 0x%x): pid %d, addr %x\n", MAX_PAGE, pid, addr);
				return -1;
		}
		if(pid > MAX_PID) {
				printf("invalid pid (MAX_PID = %d): pid %d, addr %x\n", MAX_PID, pid, addr);
				return -1;
		}
		
		// TLB hit
		frameNo = is_TLB_hit(pid, pageNo, type);
		if(frameNo > -1) {
				*tlbhit = true;
				*hit = true;
				pageTable.stats.hitCount++;
				pageTable.stats.tlbHitCount++;
				return (frameNo << 8) + offset;
		} else *tlbhit = false;

		// page hit
		frameNo = is_page_hit(pid, pageNo, type);
		if(frameNo > -1) {
				*hit = true;
				pageTable.stats.hitCount++;
				pageTable.stats.tlbMissCount++;

				return (frameNo << 8) + offset;
		}
		else
		{
			*hit = false;
			// pagefault
			frameNo = pagefault_handler(pid, pageNo, type, hit);

			pageTable.stats.missCount++;
			pageTable.stats.tlbMissCount++;
			return (frameNo << 8) + offset;
		}
}

void pt_print_stats()
{
		int req = pageTable.stats.hitCount + pageTable.stats.missCount;
		int hit = pageTable.stats.hitCount;
		int miss = pageTable.stats.missCount;
		int tlbHit = pageTable.stats.tlbHitCount;
		int tlbMiss = pageTable.stats.tlbMissCount;

		printf("Request: %d\n", req);
		printf("Page Hit: %d (%.2f%%)\n", hit, (float) hit*100 / (float)req);
		printf("Page Miss: %d (%.2f%%)\n", miss, (float)miss*100 / (float)req);
		printf("TLB Hit: %d (%.2f%%)\n", tlbHit, (float) tlbHit*100 / (float)req);
		printf("TLB Miss: %d (%.2f%%)\n", tlbMiss, (float) tlbMiss*100 / (float)req);

}

void pt_init()
{
		int i,j;

		pageTable.stats.hitCount = 0;
		pageTable.stats.missCount = 0;
		pageTable.stats.tlbHitCount = 0;
		pageTable.stats.tlbMissCount = 0;

		for(i = 0; i < MAX_PID; i++) {
				for(j = 0; j < MAX_PAGE; j++) {
						pageTable.entry[i][j].valid = false;
				}
		}
}

void tlb_init()
{
		int i, j;
		for (i = 0; i < MAX_PID; i++)
		{
				for(j = 0; j < TLB_ENTRY / 2; j++)
				{
						tlb[i][j].valid[0] = tlb[i][j].valid[1] = false;
						tlb[i][j].lru = 0;
				}
		}
}
struct Node* GetNewNode(int x) 
{
	
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Inserts a Node at head of doubly linked list
void InsertAtHead(int x) 
{
	
	struct Node* newNode = GetNewNode(x);
	if(head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head; 
	head = newNode;
}

