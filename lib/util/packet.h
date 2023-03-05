#pragma once
#include <stddef.h>
#include <stdbool.h>
void packet_module_init(void);
void packet_dump(const char *pkt,size_t sz);
bool packet_icmp_echo(const char *source,size_t sz);

