#include <unistd.h>
#include <stdio.h>

int main() {
    long sem_nsems_max = sysconf(_SC_SEM_NSEMS_MAX);
    long sem_value_max = sysconf(_SC_SEM_VALUE_MAX);

    printf("sem_nsems_max: %ld\n", sem_nsems_max);
    printf("sem_value_max: %ld\n", sem_value_max);
}