#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SHM1 "/student1_marks"
#define SHM2 "/student2_marks"

#define SEM1 "/student1_ready"
#define SEM2 "/student2_ready"

int main()
{
    int fd1, fd2;
    int *marks1, *marks2;
    pid_t pid1, pid2;

    /* Remove old shared memory and semaphores if they exist */
    shm_unlink(SHM1);
    shm_unlink(SHM2);
    sem_unlink(SEM1);
    sem_unlink(SEM2);

    /* Create shared memory for Student 1 */
    fd1 = shm_open(SHM1, O_CREAT | O_RDWR, 0666);
    if (fd1 == -1)
    {
        perror("shm_open student1");
        exit(1);
    }

    ftruncate(fd1, 5 * sizeof(int));

    marks1 = mmap(NULL, 5 * sizeof(int),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, fd1, 0);

    /* Create shared memory for Student 2 */
    fd2 = shm_open(SHM2, O_CREAT | O_RDWR, 0666);
    if (fd2 == -1)
    {
        perror("shm_open student2");
        exit(1);
    }

    ftruncate(fd2, 5 * sizeof(int));

    marks2 = mmap(NULL, 5 * sizeof(int),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, fd2, 0);

    /* Create semaphores */
    sem_t *sem1 = sem_open(SEM1, O_CREAT, 0666, 0);
    sem_t *sem2 = sem_open(SEM2, O_CREAT, 0666, 0);

    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    /* Enter Student 1 marks */
    printf("Enter marks for Student 1:\n");

    for (int i = 0; i < 5; i++)
    {
        printf("Subject %d: ", i + 1);
        scanf("%d", &marks1[i]);
    }

    printf("\nStudent 1 marks written successfully.\n");

    /* Enter Student 2 marks */
    printf("\nEnter marks for Student 2:\n");

    for (int i = 0; i < 5; i++)
    {
        printf("Subject %d: ", i + 1);
        scanf("%d", &marks2[i]);
    }

    printf("\nStudent 2 marks written successfully.\n");

    /*
     * Create Student 1 process
     */
    pid1 = fork();

    if (pid1 == 0)
    {
        execl("./student1", "./student1", NULL);
        perror("execl student1");
        exit(1);
    }

    /*
     * Create Student 2 process
     */
    pid2 = fork();

    if (pid2 == 0)
    {
        execl("./student2", "./student2", NULL);
        perror("execl student2");
        exit(1);
    }

    /* Notify students that marks are ready */
    sem_post(sem1);
    sem_post(sem2);

    /* Wait for both students */
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("\nBoth students completed calculations.\n");

    /* Cleanup */
    munmap(marks1, 5 * sizeof(int));
    munmap(marks2, 5 * sizeof(int));

    close(fd1);
    close(fd2);

    shm_unlink(SHM1);
    shm_unlink(SHM2);

    sem_close(sem1);
    sem_close(sem2);

    sem_unlink(SEM1);
    sem_unlink(SEM2);

    printf("Shared memory cleaned up successfully.\n");

    return 0;
}
