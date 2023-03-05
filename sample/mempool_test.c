#include <stdio.h>
#include "buf.h"
#include "my_log.h"
int main(){
    abs_mbuf_module_init();
    log_category_enable("mbuf",LOG_ALL);
    size_t pool_size=4096000;//4M;
    uint16_t buf_len=1500;
    uint16_t priv_len=0;
    abc_mempool *mp=abc_mempool_create(pool_size,priv_len,buf_len);
    abc_mbuf *mbuf=NULL;
    int count=0;
    if(mp){
        for(int i=0;i<10;i++){
            mbuf=abc_mbuf_raw_alloc(mp);
            printf("addr %p %d\n",mbuf,(int)mbuf->buf_len);
            abc_mbuf_raw_free(mbuf);
        }
        while(1){
            mbuf=abc_mbuf_raw_alloc(mp);
            count++;
            if(!mbuf){
                break;
            }
        }
    }
    printf("count %d\n",count);
}