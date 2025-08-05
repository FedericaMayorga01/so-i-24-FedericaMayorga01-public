# Preguntas y respuestas sobre el sistema de archivos /proc y métricas en Linux

1. **¿Cuál es el propósito del sistema de archivos /proc en Linux, y cómo podemos usarlo para recopilar métricas del sistema?**

    El sistema de archivos `/proc` en Linux es un sistema de archivos virtual que proporciona una interfaz dinámica para acceder a información del kernel y del estado del sistema en tiempo real. No contiene archivos físicos almacenados en el disco, sino que expone estructuras internas del sistema operativo como si fueran archivos de texto. Esto permite a los programas leer información del sistema de forma eficiente, sin llamadas al sistema adicionales o herramientas específicas.

    Se utiliza para recopilar métricas como:

    - Uso de CPU: `/proc/stat`
    - Memoria: `/proc/meminfo`
    - Procesos activos: `/proc/loadavg` o `/proc/[pid]`
    - Disco: `/proc/diskstats`
    - Red: `/proc/net/dev`

    Esta información es útil para construir monitores que exporten métricas a sistemas como, en este caso, Prometheus.


2. **¿Cómo interpretas los campos en /proc/stat relacionados con el uso de la CPU, y cómo se utilizan para calcular el porcentaje de utilización de la CPU?**

    En `/proc/stat`, la primera línea comienza con cpu y muestra una serie de números que representan el tiempo que los núcleos pasaron en distintos estados (en jiffies o ticks del sistema):

    `cpu user nice system idle iowait irq softirq steal guest guest_nice`

    Los campos más usados para el cálculo son:

    `user, nice, system → Tiempo de CPU "activo"`  
    `idle, iowait → Tiempo "inactivo"`

    Para calcular el porcentaje de utilización de CPU:

    `delta_idle = idle_t2 - idle_t1`  
    `delta_total = total_t2 - total_t1`  
    `CPU_usage = 100 * (delta_total - delta_idle) / delta_total`

    Se toman dos muestras separadas por un intervalo de tiempo, y se usa la diferencia para estimar el uso real de CPU.


3. **Explica cómo Prometheus recopila y almacena métricas, y cómo Grafana visualiza estos datos. ¿Cuáles son los componentes clave de cada uno?**

    Prometheus es un sistema de monitoreo basado en pull, que periódicamente realiza solicitudes HTTP a endpoints `/metrics` expuestos por las aplicaciones. Estas métricas se recogen como texto plano y se almacenan como series temporales en su base de datos interna (TSDB).

    Componentes clave de Prometheus:

    - Prometheus Server: realiza scrapes y almacena datos.
    - Job targets: aplicaciones instrumentadas que exponen métricas.
    - Configuración (`prometheus.yml`): define a quién y cada cuánto consultar.
    - Lenguaje de consulta (PromQL): permite hacer consultas avanzadas sobre las métricas.

    Grafana se conecta a Prometheus como fuente de datos y permite crear dashboards interactivos para visualizar esas métricas.

    Componentes clave de Grafana:

    - Data sources (como Prometheus).
    - Panels y dashboards: gráficos, tablas, alertas.
    - Consultas PromQL integradas.


4. **¿Cuál es la diferencia entre un gauge, un counter y un histograma en Prometheus? Proporciona un ejemplo de cuándo debería usarse cada uno.**

    **Gauge:**  
    - Mide un valor que puede subir o bajar.
    - Por ejemplo: memory_usage_percentage o cpu_usage_percentage.
    - Útil para recursos variables (memoria libre, procesos activos).

    **Counter:**  
    - Solo se incrementa (nunca decrece), y se reinicia con el reinicio del proceso.
    - Por ejemplo: http_requests_total, context_switches.
    - Para eventos acumulativos (cantidad de errores, peticiones).

    **Histogram:**
    - Agrupa observaciones en rangos de valores y cuenta cuántas caen en cada uno.
    - Por ejemplo: tiempos de respuesta de API (http_request_duration_seconds).
    - Para analizar distribuciones (latencias, tamaños de archivos).


5. **¿Por qué sería necesario un mutex al trabajar con métricas en un entorno multi-thread? ¿Qué podría salir mal si no se utiliza?**

    En un entorno multithread, varios hilos pueden modificar o leer métricas al mismo tiempo. Si no se protege ese acceso concurrente con un mutex u otro mecanismo de sincronización, pueden ocurrir:
    - Condiciones de carrera (race conditions).
    - Lecturas inconsistentes (valores parcialmente escritos).
    - Crash o corrupción de memoria.

    Por ejemplo, si dos hilos actualizan un counter sin sincronización, podrían perderse incrementos o escribirse valores inválidos.

    Usar un mutex garantiza exclusión mutua: solo un hilo puede modificar una métrica a la vez, asegurando integridad y consistencia.