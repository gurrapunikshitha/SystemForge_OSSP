#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM1 "/student1_marks"
#define SEM1 "/student1_ready"

int main()
{
    int fd;
    int *marks;
    int total = 0;
    float average;

    /* Open only Student 1 shared memory */
    fd = shm_open(SHM1, O_RDONLY, 0666);

    if (fd == -1)
    {
        perror("Student 1 shm_open");
        exit(1);
    }

    marks = mmap(NULL, 5 * sizeof(int),
                 PROT_READ,
                 MAP_SHARED, fd, 0);

    if (marks == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    /* Wait for Teacher notification */
    sem_t *sem1 = sem_open(SEM1, 0);

    if (sem1 == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_wait(sem1);

    printf("\nStudent 1\n");
    printf("--------------------------------\n");

    for (int i = 0; i < 5; i++)
    {
        printf("Subject %d : %d\n", i + 1, marks[i]);
        total += marks[i];
    }

    average = total / 5.0;

    printf("Total   : %d\n", total);
    printf("Average : %.2f\n", average);

    munmap(marks, 5 * sizeof(int));
    close(fd);
    sem_close(sem1);

    return 0;
}
