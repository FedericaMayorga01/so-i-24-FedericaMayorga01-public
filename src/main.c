/**
 * @file main.c
 * @brief Entry point of the system
 */

#include "../include/expose_metrics.h"
#include "../include/metrics.h"
#include <stdbool.h>

/**
 * @brief Wait time in seconds between metrics updates.
 */
#define SLEEP_TIME 1

/**
 * @brief Main function of the program.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return int Exit code of the program.
 */
int main(int argc, char* argv[])
{
    init_metrics();
    // Create a thread to expose metrics via HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error creating HTTP server thread\n");
        return EXIT_FAILURE;
    }

    // Main loop to update metrics every second
    while (true)
    {
        update_cpu_gauge();
        update_memory_gauge();
        update_IOdisk();
        update_network_transferrate();
        update_processes_counter();
        update_context_switchs();
        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}
