#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include "rte_lcore.h"
#include "rte_random.h"
#include "rte_cycles.h"
#include "eal_private.h"
#include "rte_timer.h"

static inline uint64_t get_millis(void)
{
	struct timespec val;
	uint64_t v;

	while (clock_gettime(CLOCK_MONOTONIC_RAW, &val) != 0)
		/* no body */;

	v  = (uint64_t) val.tv_sec * 1000000000LL;
	v += (uint64_t) val.tv_nsec;
	return v/1000000;
}
int lcore_id_seq=0;
int lcore_id_alloc(){
    return lcore_id_seq++;
}
void main_init(){
    if(LCORE_ID_ANY==rte_lcore_id()){
        RTE_PER_LCORE(_lcore_id) =lcore_id_alloc();
    }
    
}
void time_delay_test(){
    const uint64_t ticks_per_ms = rte_get_tsc_hz()/1000;
    const uint64_t ticks_per_us = ticks_per_ms/1000;
    uint64_t start_tsc = rte_rdtsc();
    //rte_delay_us_sleep(US_PER_S);
    rte_delay_us_block(US_PER_S);
    uint64_t end_tsc=rte_rdtsc();
    uint32_t mills=(end_tsc-start_tsc+ticks_per_ms/2)/(ticks_per_ms);
    printf("elapse: %d\n",mills);
}
void time_hz_test(){
    uint64_t start_time = get_millis();
    uint64_t start=rte_get_timer_cycles();
    uint64_t hz=rte_get_timer_hz();
    while ((rte_get_timer_cycles() - start) <hz)
        rte_pause();
    uint64_t end_time=get_millis();
    uint32_t mills=end_time-start_time;
    printf("elapse: %d\n",mills);
}
uint64_t g_start_time=0;
uint64_t hz=0;
int cout1=0;
int cout2=0;
static volatile bool g_running=true;
void timer_cb1(struct rte_timer *timer,void *arg){
    const uint64_t ticks_per_ms = rte_get_tsc_hz()/1000;
    uint64_t end_tsc=rte_rdtsc();
    uint32_t mills=(end_tsc-g_start_time+ticks_per_ms/2)/(ticks_per_ms);
    uint32_t lcore_id=rte_lcore_id();
    cout1++;
    printf("%s %d,%d\n",__FUNCTION__,cout1,mills);
    if(cout1<10){
        rte_timer_reset(timer,
					hz/20,
					SINGLE,lcore_id,
					timer_cb1, NULL);
    }
}
void timer_cb2(struct rte_timer *timer,void *arg){
    const uint64_t ticks_per_ms = rte_get_tsc_hz()/1000;
    uint64_t end_tsc=rte_rdtsc();
    uint32_t mills=(end_tsc-g_start_time+ticks_per_ms/2)/(ticks_per_ms);
    uint32_t lcore_id=rte_lcore_id();
    cout2++;
    printf("%s %d,%d\n",__FUNCTION__,cout2,mills);
    if(cout2<10){
        rte_timer_reset(timer,
					hz/10,
					SINGLE,lcore_id,
					timer_cb2, NULL);
    }
}
static void sig_handler(int signo)
{
    (void)signo;
	g_running=false;
}
int main(int argc, char **argv){
    if (signal(SIGINT, sig_handler) ||
        signal(SIGHUP, sig_handler) ||
        signal(SIGTERM, sig_handler)||
        signal(SIGTSTP,sig_handler)){
        perror("signal");
        return 1;
    }
    main_init();
    unsigned int main_lcore = rte_get_main_lcore();
    if(0!=rte_eal_init(argc,argv)){
        return -1;
    }
    rte_timer_subsystem_init();
    hz = rte_get_timer_hz();
    struct rte_timer timer1,timer2;
    rte_timer_init(&timer1);
    rte_timer_init(&timer2);
    uint32_t lcore_id=rte_lcore_id();
    g_start_time=rte_rdtsc();
    rte_timer_reset(&timer1,
					hz/20,
					SINGLE,lcore_id ,
					timer_cb1, NULL);
    rte_timer_reset(&timer2,
					hz/10,
					SINGLE,lcore_id ,
					timer_cb2, NULL);
    while(g_running){
        rte_timer_manage();
    }
    return 0;
}
