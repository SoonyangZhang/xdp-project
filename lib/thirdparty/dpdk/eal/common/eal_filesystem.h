#pragma once

/** Path of rte config file. */

#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

#include <rte_string_fns.h>
/** Function to read a single numeric value from a file on the filesystem.
 * Used to read information from files on /sys */
int eal_parse_sysfs_value(const char *filename, unsigned long *val);