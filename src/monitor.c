#include "monitor.h"
pid_t monitor_pid = -1;
void start_monitor() {
    if (monitor_pid != -1) {
        printf("El programa de monitoreo ya está ejecutándose (PID: %d).\n", monitor_pid);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error al iniciar el monitor");
        return;
    }
    if (pid == 0) {
        // Proceso hijo: ejecuta el monitor
        execl("/home/fg/Documentos/Facultad/SO/Laboratorios/so-i-24-chp2-facundoguerrero2/build/so-i-24-facundoguerrero2/METRICAS", "MONITOR", NULL);
        perror("Error al ejecutar el programa de monitoreo");
        exit(EXIT_FAILURE);
    }
    // Proceso padre: guarda el PID
    monitor_pid = pid;
    printf("Monitor iniciado (PID: %d).\n", monitor_pid);
}


void stop_monitor() {
    if (monitor_pid == -1) {
        printf("El programa de monitoreo no está en ejecución.\n");
        return;
    }

    if (kill(monitor_pid, SIGTERM) == 0) {
        printf("Monitor detenido.\n");
        monitor_pid = -1;
    } else {
        perror("Error al detener el monitor");
    }
}

void status_monitor() {
    if (monitor_pid == -1) {
        printf("El programa de monitoreo no está en ejecución.\n");
        return;
    }

    if (kill(monitor_pid, 0) == 0) {
        printf("El monitor está en ejecución (PID: %d).\n", monitor_pid);
    } else {
        printf("El monitor no está en ejecución.\n");
        monitor_pid = -1; // Resetear PID si el proceso ya no está
    }
}
