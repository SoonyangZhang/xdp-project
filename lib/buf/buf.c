#include <memory.h>
#include <unistd.h>
#include "buf.h"
#include "my_log.h"
#include "../thirdparty/ncx_mempool/ncx_slab.h"
DEFINE_LOG_CATEGORY("mbuf");
#define ABC_MIN(a, b) \
	__extension__ ({ \
		typeof (a) _a = (a); \
		typeof (b) _b = (b); \
		_a < _b ? _a : _b; \
	})
typedef struct{
	uint16_t mbuf_data_room_size; /**< Size of data space in each mbuf. */
	uint16_t mbuf_priv_size;      /**< Size of private area in each mbuf. */
	uint32_t flags; /**< reserved for future use. */
}abc_mbuf_pool_private;

static inline void* abc_mempool_get_priv(abc_mempool *mp){
    return (char *)mp+sizeof(abc_mempool);
}

abc_mempool* abc_mempool_create(size_t pool_size,uint16_t priv_size, 
                uint16_t data_size){
    abc_mempool *mp=NULL;
    abc_mbuf_pool_private *priv=NULL;
    void *bufs;
    ncx_slab_pool_t *sp=NULL;
    if(0==posix_memalign(&bufs, getpagesize(), /* PAGE_SIZE aligned */
			       pool_size)){
        mp=(abc_mempool*)malloc(sizeof(*mp)+sizeof(abc_mbuf_pool_private));
        if(mp){
            sp=(ncx_slab_pool_t*)bufs;
            priv=abc_mempool_get_priv(mp);
            priv->mbuf_data_room_size=data_size;
            priv->mbuf_priv_size=priv_size;
            mp->handle=sp;
            sp->addr = bufs;
            sp->min_shift =6;
            sp->end =bufs+pool_size;
            ncx_slab_init(sp);
        }else{
            free(mp);
            free(bufs);
        }
    }
    return mp;
}
uint16_t abc_mbuf_data_room_size(abc_mempool *mp){
    abc_mbuf_pool_private *priv=abc_mempool_get_priv(mp);
    return priv->mbuf_data_room_size;
}

uint16_t abc_mbuf_priv_size(abc_mempool *mp){
    abc_mbuf_pool_private *priv=abc_mempool_get_priv(mp);
    return priv->mbuf_priv_size;
}

void* abc_mbuf_to_priv(abc_mbuf* m){
    if(m&&abc_mbuf_priv_size(m->pool)){
        return ABC_PTR_ADD(m,sizeof(abc_mbuf));
    }
    return NULL;
}

abc_mbuf* abc_mbuf_raw_alloc(abc_mempool *mp){
    abc_mbuf* m=NULL;
    abc_mbuf_pool_private *priv=abc_mempool_get_priv(mp);
    ncx_slab_pool_t *sp=mp->handle;
    size_t mbuf_size=sizeof(*m)+priv->mbuf_priv_size;
    uint16_t buf_len=priv->mbuf_data_room_size;
    size_t chunk=mbuf_size+buf_len;
    m=ncx_slab_alloc(sp,chunk);
    if(m){
        memset(m,0,chunk);
        m->buf_addr=(char*)m+mbuf_size;
        m->buf_len=buf_len;
        m->data_off=ABC_MIN(ABC_MBUF_HEADROOM,m->buf_len);
        m->data_len=0;
        m->pool=mp;
    }
    return m;
}

void abc_mbuf_raw_free(abc_mbuf* m){
    if(m&&m->pool){
        ncx_slab_pool_t *sp=m->pool->handle;
        ncx_slab_free(sp,m);
    }
}

void abs_mbuf_module_init(){
    LOG_ENSURE_REGISTER();
}