#pragma once
#include <stdint.h>
#include <stddef.h>  //NULL, size_t
#define ABC_MBUF_HEADROOM 0
struct abc_mempool;
struct abc_mbuf{
    void *buf_addr;
    uint16_t buf_len;
    uint16_t data_off;
	uint16_t data_len;        /**< Amount of data in segment buffer. */
    struct abc_mbuf *next;
    struct abc_mempool *pool;
};
struct abc_mempool{
    void *handle;
};
typedef struct abc_mempool abc_mempool;
typedef struct abc_mbuf  abc_mbuf;
#define ABC_PTR_ADD(ptr, x) ((void*)((uintptr_t)(ptr) + (x)))

#define abc_mbuf_mtod_offset(m, t, o)	\
	((t)((char *)(m)->buf_addr + (m)->data_off + (o)))
    
#define abc_mbuf_mtod(m, t) abc_mbuf_mtod_offset(m, t, 0)
abc_mempool* abc_mempool_create(size_t pool_size,uint16_t priv_size, 
                    uint16_t data_size);

uint16_t abc_mbuf_data_room_size(abc_mempool *mp);
uint16_t abc_mbuf_priv_size(abc_mempool *mp);
void* abc_mbuf_to_priv(abc_mbuf* m);


abc_mbuf* abc_mbuf_raw_alloc(abc_mempool *mp);
void abc_mbuf_raw_free(abc_mbuf* m);

static inline char* abc_mbuf_adj(abc_mbuf* m,uint16_t len){
    if(len> m->data_len){return NULL;}
    m->data_len = (uint16_t)(m->data_len - len);
    m->data_off = (uint16_t)(m->data_off + len);
    return (char *)m->buf_addr + m->data_off;
}

void abs_mbuf_module_init();
