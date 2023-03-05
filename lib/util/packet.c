//http://blog.chinaunix.net/uid-30012596-id-4729018.html
#include <stdio.h>
#include <memory.h>
#include <net/ethernet.h>
#include <netinet/ether.h>//ether_ntoa
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h> //ntohs
#include <sys/socket.h>
#include "packet.h"
#include "my_log.h"
DEFINE_LOG_CATEGORY("packet");
//https://blog.csdn.net/s2603898260/article/details/115773213
//http://ohm.bu.edu/~cdubois/CLEAN/pings%20and%20things/pinger.c
//linux icmp_echo
/*
* in_cksum --
* Checksum routine for Internet Protocol
* family headers (C Version)
*/
uint16_t in_cksum(const char* addr, int len)
{
   int sum = 0;
   uint16_t answer = 0;
   const uint16_t* w =(const uint16_t*)addr;
   int nleft;
   /*
   * Our algorithm is simple, using a 32 bit accumulator (sum), we add
   * sequential 16 bit words to it, and at the end, fold back all the
   * carry bits from the top 16 bits into the lower 16 bits.
   */
   for(nleft = len; nleft > 1; nleft -= 2)
   {
      sum += *w++;
   }
   /* mop up an odd byte, if necessary */
   if(nleft == 1)
   {
      *(uint8_t*) (&answer) = *(uint8_t*) w;
      sum += answer;
   }
   /* add back carry outs from top 16 bits to low 16 bits */
   sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
   sum += (sum >> 16); /* add carry */
   answer = ~sum; /* truncate to 16 bits */
   return answer;
}
uint16_t cksum_generic(const char *p, size_t len, uint16_t initial){
  uint32_t sum = htons(initial);
  const uint16_t *u16 = (const uint16_t *)p;

  while (len >= (sizeof(*u16) * 4)) {
    sum += u16[0];
    sum += u16[1];
    sum += u16[2];
    sum += u16[3];
    len -= sizeof(*u16) * 4;
    u16 += 4;
  }
  while (len >= sizeof(*u16)) {
    sum += *u16;
    len -= sizeof(*u16);
    u16 += 1;
  }
  /* if length is in odd bytes */
  if (len == 1)
    sum += *((const uint8_t *)u16);

  while(sum>>16)
    sum = (sum & 0xFFFF) + (sum>>16);
  return /*ntohs*/((uint16_t)~sum);
}
static void icmp_dump_hdr(const char *data,size_t sz){
	const struct icmphdr *icmph=(const struct icmphdr*)(data);
	int type=icmph->type;
	if(ICMP_ECHO==type||ICMP_ECHOREPLY==type){
		int id=ntohs(icmph->un.echo.id);
		int seq=ntohs(icmph->un.echo.sequence);
		log_info("icmp seq %d sz %d,type: %d\n",seq,sz,type);
	}
}
static void udp_dump_hdr(const char *data,size_t sz){

}
static void dump_mac_addr(struct ether_addr* ethaddr){
	/*const char *octet=(const char *)ethaddr;
    for (int i = 0; i <ETH_ALEN; ++i)
      printf(" %02x", (unsigned char)octet[i]);
    printf("\n");*/
    log_info("mac:%s\n",ether_ntoa(ethaddr));
}
void packet_dump(const char *pkt,size_t sz){
    const struct ethhdr * eth_hdr=(const struct ethhdr*)pkt;
    int offset = sizeof(*eth_hdr);
    //dump_mac_addr((struct ether_addr*)&(eth_hdr->h_source));
    //dump_mac_addr((struct ether_addr*)&(eth_hdr->h_dest));
    if(sz>offset){
        const struct iphdr *ip_hdr = (const struct iphdr*)(pkt+ offset);
        size_t ip_hlen=ip_hdr->ihl<< 2;
        offset+=ip_hlen;
        if(4==ip_hdr->version){
        	if(IPPROTO_ICMP==ip_hdr->protocol){
        		const char *icmp=(const char*)(pkt+offset);
        		icmp_dump_hdr(icmp,sz-offset);
        	}else if(IPPROTO_TCP==ip_hdr->protocol||IPPROTO_UDP==ip_hdr->protocol){
        		const void *udp=(const void*)(pkt+offset);
        		udp_dump_hdr(udp,sz-offset);
        	}
        }
    }
}
static inline void icmp_gen_echo(char* data,size_t sz){
	struct icmphdr *icmph=(struct icmphdr*)(data);
	int type=icmph->type;
	if(ICMP_ECHO==type){
		icmph->type=ICMP_ECHOREPLY;
		icmph->checksum=0;
		icmph->checksum=in_cksum(data,sz);
	}
}
static inline void swap_ipv4_header(char* data){
	struct iphdr *ip_hdr=(struct iphdr*)data;
	if(4==ip_hdr->version){
		uint32_t memory=ip_hdr->saddr;
		ip_hdr->saddr=ip_hdr->daddr;
		ip_hdr->daddr=memory;
	}
}
static inline void swap_ether_header(char* data){
	struct ethhdr *eth = (struct ethhdr *)data;
	struct ether_addr *src_addr = (struct ether_addr *)&eth->h_source;
	struct ether_addr *dst_addr = (struct ether_addr *)&eth->h_dest;
	struct ether_addr tmp;
	tmp = *src_addr;
	*src_addr = *dst_addr;
	*dst_addr = tmp;
}
__thread char packet_buffer[1024];
bool packet_icmp_echo(const char *source,size_t sz){
	bool flag=false;
	memcpy(packet_buffer,source,sz);
    struct ethhdr * eth_hdr=(struct ethhdr*)packet_buffer;
    int offset = sizeof(*eth_hdr);
    struct iphdr *ip_hdr=NULL;
    size_t ip_hlen=0;
    char *icmphdr=NULL;
    if(sz>offset){
    	ip_hdr=(struct iphdr*)(packet_buffer+offset);
    	ip_hlen=ip_hdr->ihl<< 2;
    	offset+=ip_hlen;
    	if(4==ip_hdr->version&&IPPROTO_ICMP==ip_hdr->protocol){
    		icmphdr=(char*)(packet_buffer+offset);
    		icmp_gen_echo(icmphdr,sz-offset);
    		swap_ipv4_header((char*)ip_hdr);
        	flag=true;
    	}
    	swap_ether_header(packet_buffer);
    }
    if(true==flag){
    	memcpy(source,packet_buffer,sz);
    }
    return flag;
}

void packet_module_init(void){
	LOG_ENSURE_REGISTER();
}
