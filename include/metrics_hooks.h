#ifndef METRICS_HOOKS_H
#define METRICS_HOOKS_H

/**
 * @brief Structure for external metrics that can be registered by extensions
 */
typedef struct
{
    /** @brief Name of the metric */
    const char* name;
    /** @brief Function pointer to get the metric value */
    double (*get_value)(void);
    /** @brief Description of what the metric measures */
    const char* description;
    /** @brief Type of the metric (gauge, counter, etc.) */
    const char* type;
} external_metric_t;

/**
 * @brief External variables that extensions can override (weak symbols)
 */
extern external_metric_t* external_metrics;
extern int external_metrics_count;

/**
 * @brief Function that extensions can implement to register their metrics
 */
void __attribute__((weak)) register_external_metrics(void);

/**
 * @brief Function that extensions can implement to update their metrics
 */
void __attribute__((weak)) update_external_metrics(void);

#endif // METRICS_HOOKS_H