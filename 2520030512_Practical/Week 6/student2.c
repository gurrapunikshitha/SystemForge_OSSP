#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM2 "/student2_marks"
#define SEM2 "/student2_ready"

int main()
{
    int fd;
    int *marks;
    int total = 0;
    float average;

    /* Open only Student 2 shared memory */
    fd = shm_open(SHM2, O_RDONLY, 0666);

    if (fd == -1)
    {
        perror("Student 2 shm_open");
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
    sem_t *sem2 = sem_open(SEM2, 0);

    if (sem2 == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_wait(sem2);

    printf("\nStudent 2\n");
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
    sem_close(sem2);

    return 0;
}
