/**
 * @file expose_metrics.h
 * @brief Program to read CPU and memory usage and expose them as Prometheus metrics.
 */

#ifndef EXPOSE_METRICS_H
#define EXPOSE_METRICS_H

#include "../include/metrics.h"
#include "metrics.h"
#include "metrics_hooks.h"
#include <errno.h>
#include <prom.h>
#include <promhttp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Buffer size used for reading data.
 */
#define BUFFER_SIZE 256

/**
 * @brief Updates the CPU usage metric.
 */
void update_cpu_gauge();

/**
 * @brief Updates the memory usage metric.
 */
void update_memory_gauge();

/**
 * @brief Updates the disk usage metric.
 */
void update_IOdisk();

/**
 * @brief Updates the network transfer rate metric.
 */
void update_network_transferrate();

/**
 * @brief Updates the running processes count metric.
 */
void update_processes_counter();

/**
 * @brief Updates the context switches count metric.
 */
void update_context_switchs();

/**
 * @brief Thread function to expose metrics via HTTP on port 8000.
 * @param arg Unused argument.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Initialize mutex and metrics.
 */
void init_metrics();

/**
 * @brief Mutex destructor.
 */
void destroy_mutex();

/**
 * @brief Initialize external metrics from registered extensions
 */
void init_external_metrics(void);

/**
 * @brief Update all external metrics
 */
void update_all_external_metrics(void);

#endif