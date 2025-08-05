#include "../include/expose_metrics.h"
#include "../include/metrics.h"
/** Mutex for thread synchronization */
pthread_mutex_t lock;

/** Prometheus metric for CPU usage */
static prom_gauge_t* cpu_usage_metric;

/** Prometheus metric for memory usage */
static prom_gauge_t* memory_usage_metric;

/** Prometheus metric for disk IO operations */
static prom_gauge_t* IOdisk_usage_metric;

/** Prometheus metric for network transfer rate */
static prom_gauge_t* network_transfer_rate_metric;

/** Prometheus metric for the number of running processes */
static prom_gauge_t* processes_metric;

/** Prometheus metric for the number of context switches */
static prom_gauge_t* context_switches_metric;

void update_cpu_gauge()
{

    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error getting CPU usage\n");
    }
}

void update_memory_gauge()
{
    double usage = get_memory_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error getting memory usage\n");
    }
}

void update_IOdisk()
{
    double usage = get_IOdisk();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(IOdisk_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error getting disk IO usage time\n");
    }
}

void update_network_transferrate()
{
    double transfer = get_network_transfer_rate();
    if (transfer >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(network_transfer_rate_metric, transfer, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error getting network transfer rate\n");
    }
}

void update_processes_counter()
{
    double processes = get_processcounter();
    if (processes >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(processes_metric, processes, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error getting the number of running processes\n");
    }
}

void update_context_switchs()
{
    double context_switches = get_context_switchs();
    if (context_switches >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, context_switches, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error getting the number of context switches\n");
    }
}

void* expose_metrics(void* arg)
{
    (void)arg; // Argument not used

    // Ensure the HTTP handler is attached to the default registry
    promhttp_set_active_collector_registry(NULL);

    // Start the HTTP server on port 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error starting the HTTP server\n");
        return NULL;
    }

    // Keep the server running
    while (1)
    {
        sleep(1);
    }

    // Should never reach here
    MHD_stop_daemon(daemon);
    return NULL;
}

void init_metrics()
{
    // Initialize the mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error initializing mutex\n");
    }

    // Initialize the Prometheus collector registry
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error initializing Prometheus registry\n");
    }

    // Create the metric for CPU usage
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "CPU usage percentage", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error creating CPU usage metric\n");
    }

    // Create the metric for memory usage
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Memory usage percentage", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error creating memory usage metric\n");
    }

    // Create the metric for disk IO busy time
    IOdisk_usage_metric = prom_gauge_new("IOdisk_usage_percentage", "Disk IO busy time percentage", 0, NULL);
    if (IOdisk_usage_metric == NULL)
    {
        fprintf(stderr, "Error creating disk IO busy time metric\n");
    }

    // Create the metric for network transfer rate
    network_transfer_rate_metric = prom_gauge_new("network_transfer_rate", "Network transfer rate", 0, NULL);
    if (network_transfer_rate_metric == NULL)
    {
        fprintf(stderr, "Error creating network transfer rate metric\n");
    }

    // Create the metric for the number of running processes
    processes_metric = prom_gauge_new("processes_running", "Number of running processes", 0, NULL);
    if (processes_metric == NULL)
    {
        fprintf(stderr, "Error creating running processes metric\n");
    }

    // Create the metric for the number of context switches
    context_switches_metric = prom_gauge_new("context_switches", "Number of context switches", 0, NULL);
    if (context_switches_metric == NULL)
    {
        fprintf(stderr, "Error creating context switches metric\n");
    }

    // Register metrics
    if (prom_collector_registry_must_register_metric(memory_usage_metric) == NULL)
    {
        fprintf(stderr, "Error registering memory usage metric\n");
    }
    if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL)
    {
        fprintf(stderr, "Error registering CPU usage metric\n");
    }
    if (prom_collector_registry_must_register_metric(IOdisk_usage_metric) == NULL)
    {
        fprintf(stderr, "Error registering disk IO busy time metric\n");
    }
    if (prom_collector_registry_must_register_metric(network_transfer_rate_metric) == NULL)
    {
        fprintf(stderr, "Error registering network transfer rate metric\n");
    }
    if (prom_collector_registry_must_register_metric(processes_metric) == NULL)
    {
        fprintf(stderr, "Error registering running processes metric\n");
    }
    if (prom_collector_registry_must_register_metric(context_switches_metric) == NULL)
    {
        fprintf(stderr, "Error registering context switches metric\n");
    }
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
