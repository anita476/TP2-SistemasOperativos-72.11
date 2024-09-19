/* 
#define BLOCK_SIZE 10
#define BLOCK_COUNT 10
void * start, current;
int size;
void * free_ptrs[BLOCK_COUNT];
int current = 0;
void my_mm_init(void *p, int s){
	start = p;
	size = s;
	
	free_ptrs[0] = start;
	free_ptrs[1] = start + BLOCK_SIZE ; //y etc.. meter en un for
	free_ptrs[2] = start + BLOCK_SIZE + BLOCK_SIZE;
	current = 0;

}

void * my_malloc(){
	if(current <  size + start){
		return free_ptrs[current++];
	}
	return null;
}

void my_free(void * p){
	free_ptrs[--current] = p;

} */
