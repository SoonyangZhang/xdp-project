#include <stdio.h>
#include "lib/thirdparty/ncx_mempool/ncx_slab.h"
#include "my_log.h"
int main(int argc, char **argv)
{
    ncx_slab_module_init();
    log_category_enable("ncx_memory",LOG_INFO);
	char *p;
	size_t 	pool_size = 4096000;  //4M 
	ncx_slab_stat_t stat;
	u_char 	*space;
	space = (u_char *)malloc(pool_size);

	ncx_slab_pool_t *sp;
	sp = (ncx_slab_pool_t*) space;

	sp->addr = space;
	sp->min_shift = 3;
	sp->end = space + pool_size;

	ncx_slab_init(sp);

	int count=0;
	while(1)
	{   
		p = ncx_slab_alloc(sp, 1500);
        count++;
		if (p == NULL) 
		{   
			printf("out of memory%d\n",count); 
            break;
		}    
	}
	ncx_slab_stat(sp, &stat);

	free(space);
    printf("free space\n");
	return 0;
}
