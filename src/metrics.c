#include "../include/metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

double get_memory_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Open the file /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/meminfo");
        return -1.0;
    }

    // Read the total and available memory
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal found, continue to read for MemAvailable
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable found, break the loop
        }
    }

    fclose(fp);

    // Verifies that we have valid values
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error reading memory information from /proc/meminfo\n");
        return -1.0;
    }

    // Calculate the percentage of memory usage
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    return mem_usage_percent;
}

double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Open and read /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error reading /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analyze the first line of /proc/stat
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error parsing /proc/stat\n");
        return -1.0;
    }

    // Calculate the differences between the current and previous readings
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald is zero, cannot calculate CPU usage!\n");
        return -1.0;
    }

    // Calculate the CPU usage percentage
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Update the previous values for the next reading
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

double get_IOdisk()
{
    FILE* fp = NULL;
    char buffer[BUFFER_SIZE];
    double io_busy_porcent = 0;
    unsigned long long io_busy_time = 0, uptime_ms = 0;
    char device_name[32];
    static unsigned long long prev_io_busy_time = 0;
    static unsigned long long prev_uptime_ms = 0;

    // Open the file /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/diskstats");
        return -1.0;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        unsigned long long device_busy_time;

        // Finds the line for the nvme0n1 device and extracts the busy time
        if (sscanf(buffer, "%hhu %llu %*s %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u", device_name, &device_busy_time) ==
            2)
        {
            if (strcmp(device_name, "nvme0n1") == 0)
            {
                io_busy_time = device_busy_time;
                break; // Stop reading after finding the device
            }
        }
    }

    // To get the uptime, we read /proc/uptime
    fp = fopen("/proc/uptime", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/uptime");
        return -1.0;
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error reading /proc/uptime");
        fclose(fp);
        return -1.0;
    }
    fclose(fp); // Close the file after reading

    // Get the uptime in seconds from the first value in /proc/uptime
    double uptime_seconds;
    if (sscanf(buffer, "%lf", &uptime_seconds) != 1)
    {
        fprintf(stderr, "Error reading uptime from /proc/uptime\n");
        return -1.0;
    }

    // Now we convert the uptime to milliseconds
    uptime_ms = (unsigned long long)(uptime_seconds * 1000);
    // Calculates the difference in busy time and uptime since the last reading
    unsigned long long io_busy_time_diff = io_busy_time - prev_io_busy_time;
    unsigned long long uptime_ms_diff = uptime_ms - prev_uptime_ms;
    if (uptime_ms_diff == 0)
    {
        fprintf(stderr, "Invalid value for uptime\n");
        return -1.0;
    }
    // Obtain the percentage of time that the nvme0n1 disk has been busy
    // with I/O relative to the total elapsed time
    io_busy_porcent = ((double)io_busy_time_diff / uptime_ms_diff) * 100;

    // Update the previous values for the next reading
    prev_io_busy_time = io_busy_time;
    prev_uptime_ms = uptime_ms;

    return io_busy_porcent;
}

double get_network_transfer_rate()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long rx_bytes = 0, tx_bytes = 0;
    static unsigned long long prev_rx_bytes = 0, prev_tx_bytes = 0;
    static struct timeval prev_time = {0, 0};
    struct timeval current_time;
    double transfer_rate = 0.0;

    // Get the current time
    gettimeofday(&current_time, NULL);

    // Open the file /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/net/dev");
        return -1.0;
    }

    // Read the file line by line
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Read until we find the line with the network interface data
        if (sscanf(buffer, "%*s %llu %*s %*s %*s %*s %*s %*s %*s %llu", &rx_bytes, &tx_bytes) == 2)
        {
            break; // Stop reading after finding the network interface data
        }
    }

    fclose(fp);

    // Calculate the time difference since the last reading in seconds
    double time_diff = (current_time.tv_sec - prev_time.tv_sec) + (current_time.tv_usec - prev_time.tv_usec) / 1e6;

    // Calculate the transfer rate in bytes per second
    if (time_diff > 0)
    {
        unsigned long long rx_diff = rx_bytes - prev_rx_bytes;
        unsigned long long tx_diff = tx_bytes - prev_tx_bytes;
        transfer_rate = (double)(rx_diff + tx_diff) / time_diff;
    }

    // Update the previous values for the next reading
    prev_rx_bytes = rx_bytes;
    prev_tx_bytes = tx_bytes;
    prev_time = current_time;

    return transfer_rate;
}

int get_processcounter()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    int running_processes = 0;

    // Open the file /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/stat");
        return -1.0;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {

        // Search for the line that contains procs_running
        if (sscanf(buffer, "procs_running %d", &running_processes) == 1)
        {
            break; // Stop reading after finding procs_running
        }
    }

    fclose(fp);

    return running_processes;
}

int get_context_switchs()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    int context_switches = 0;

    // Open the file /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error opening /proc/stat");
        return -1.0;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {

        // Search for the line that contains ctxt
        if (sscanf(buffer, "ctxt %u", &context_switches) == 1)
        {
            break; // Stop reading after finding ctxt
        }
    }

    fclose(fp);

    return context_switches;
}
