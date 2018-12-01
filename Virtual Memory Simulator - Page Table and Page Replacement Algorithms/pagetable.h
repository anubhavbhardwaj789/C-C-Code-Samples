typedef struct {
		int pid;
		int page;
} Invert;

typedef struct {
		int frameNo;
		bool valid;
		bool dirty;
} PTE;

typedef struct {
		int hitCount;
		int missCount;
		int tlbHitCount;
		int tlbMissCount;
} STATS;

typedef struct {
		PTE entry[MAX_PID][MAX_PAGE];
		STATS stats;
} PT;

typedef struct {
		int entry[2]; // virtual pageno
		int frameNo[2];
		bool valid[2];
		int lru;
} TLB;

struct Node  {
	int data;
	struct Node* next;
	struct Node* prev;
};

struct Node* head; // global variable - pointer to head node.

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(int x);
void InsertAtHead(int x);
void InsertAtTail(int x);

typedef struct frequency 
{
	int frequency;
	int old;
}FREQ;
int lfu_page[MAX_FRAME];
int lfu_freq[MAX_FRAME];
int lfu_old[MAX_FRAME];
typedef struct  ftrack
{
Invert inv;
FREQ f;
}FTRK;
int inv_pg[MAX_FRAME];

int pagefault_handler(int pid, int pageNo, char type, bool *hit);
