/**
 * @file metrics.h
 * @brief Functions to obtain CPU and memory usage from the /proc filesystem.
 */

#ifndef METRICS_H
#define METRICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Buffer size used for reading data.
 */
#define BUFFER_SIZE 256

/**
 * @brief Gets the percentage of memory usage from /proc/meminfo.
 *
 * Reads the total and available memory values from /proc/meminfo and calculates
 * the percentage of memory usage.
 *
 * @return Memory usage as a percentage (0.0 to 100.0), or -1.0 on error.
 */
double get_memory_usage();

/**
 * @brief Gets the percentage of CPU usage from /proc/stat.
 *
 * Reads CPU times from /proc/stat and calculates the percentage of CPU usage
 * over a time interval.
 *
 * @return CPU usage as a percentage (0.0 to 100.0), or -1.0 on error.
 */
double get_cpu_usage();

/**
 * @brief Gets the percentage of time the disk nvme0n1 has been busy with I/O compared to total elapsed time.
 *
 * Reads field 13 from /proc/diskstats to determine the time spent performing I/O operations and compares it
 * for temporal analysis, with respect to the active time obtained from uptime.
 *
 * @return Disk usage as a percentage (0.0 to 100.0), or -1.0 on error.
 */
double get_IOdisk();

/**
 * @brief Calculates the effective network transfer rate.
 *
 * Reads the bytes received and sent from /proc/net/dev and calculates the effective transfer rate
 * in bytes per second.
 *
 * @return Effective transfer rate in bytes per second, or -1.0 on error.
 */
double get_network_transfer_rate();

/**
 * @brief Gets the number of running processes.
 *
 * Checks the number of running processes in the /proc/stat file.
 *
 * @return Number of running processes, or -1 on error.
 */
int get_processcounter();

/**
 * @brief Gets the number of context switches.
 *
 * Checks the number of context switches in the /proc/stat file.
 *
 * @return Number of context switches, or -1 on error.
 */
int get_context_switchs();

#endif // METRICS_H