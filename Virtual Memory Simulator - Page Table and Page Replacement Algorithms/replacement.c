#include <stdio.h>
#include "vm.h"
#include "disk.h"
#include "pagetable.h"


int fifo_counter=0;
int lfu_fifo=0;

int random()
{

		int next = rand() % MAX_PAGE;
		return next;
}

int fifo()
{

		if(fifo_counter>MAX_FRAME-1)
			fifo_counter=0;
		int temp=fifo_counter;
		fifo_counter++;
		return temp;
}	

int lru()
{

	
		int i=0,j;
	struct Node* temp = head;
	struct Node* temp_nxt=NULL;
	Invert inv[MAX_FRAME];
	if(temp == NULL) return; // empty list, exit
	i=0;
	int chkd[256];
	int chkcount=0;
	for(j=0;j<256;j++)
	{
		chkd[j]=-1;
	}
	j=0;
	while(temp->next != NULL) 
	{
		
		temp_nxt=temp->next;
			if(temp->data==temp_nxt->data) 			
			{	
				temp = temp->next;
				for(j=0;j<=chkcount;j++)
				{
					if(chkd[j]==temp->data)
						break;
				
				}
				if(j>chkcount)
				{	chkd[j]=temp->data;
					chkcount++;
					i++;
				}
			}
			else 
			{
				
				for(j=0;j<=chkcount;j++)
				{
					if(chkd[j]==temp->data)
						break;
				
				}
				if(j>chkcount)
				{	chkd[j]=temp->data;
					chkcount++;
					i++;
				}
			}
			if(i==3)
			break;
			temp=temp->next;
		
	}
	for(i=0;i<MAX_FRAME;i++)
	{

		if(temp->data==inv_pg[i])
		{

			break;
			
		}	
	}
	return i;
}

int clock()
{
		int temp,repl=0,i=0,j=0;
		//FTRK freq[MAX_FRAME];
		temp=lfu_freq[0];
		int equity[MAX_FRAME];
		
		//checks the least frequency
		for(i=0;i<MAX_FRAME;i++)
		{
			if(temp>lfu_freq[i])
			{
				temp=lfu_freq[i];
				repl=i;
			}
		}

		for(i=0;i<MAX_FRAME;i++)
		{
			if(temp==lfu_freq[i])
			{
				if(lfu_old[repl]<lfu_old[i])
					repl=i;
			}
		}
		for(i=0;i<MAX_FRAME;i++)
		{
			if(inv_pg[i]==lfu_page[repl])
			{
				repl=i;
				break;
			}
		}
			return repl;
		
}

int page_replacement()
{
		int frame;
		if(replacementPolicy == RANDOM)  frame = random(); 
		else if(replacementPolicy == FIFO)  frame = fifo();
		else if(replacementPolicy == LRU) frame = lru();
		else if(replacementPolicy == CLOCK) frame = clock();

		return frame;
}

