#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_CLIENTS 10
#define FIFO_NAME_SIZE 100
#define BUFFER_SIZE 256

volatile sig_atomic_t signal_received = 0;

/* Signal handler */
void signal_handler(int sig)
{
    signal_received = 1;
    printf("\n[Server] Signal received from client.\n");
}

int main()
{
    int client_count = 0;
    char buffer[BUFFER_SIZE];

    /* Install signal handler */
    signal(SIGUSR1, signal_handler);

    printf("========== SERVER ==========\n");
    printf("Server PID: %d\n", getpid());

    printf("Enter number of clients (max %d): ", MAX_CLIENTS);
    scanf("%d", &client_count);
    getchar();

    if (client_count <= 0 || client_count > MAX_CLIENTS)
    {
        printf("Invalid number of clients.\n");
        return 1;
    }

    /* Create FIFOs for each client */
    for (int i = 1; i <= client_count; i++)
    {
        char client_to_server[FIFO_NAME_SIZE];
        char server_to_client[FIFO_NAME_SIZE];

        sprintf(client_to_server, "/tmp/client%d_to_server", i);
        sprintf(server_to_client, "/tmp/server_to_client%d", i);

        mkfifo(client_to_server, 0666);
        mkfifo(server_to_client, 0666);
    }

    printf("\nFIFOs created successfully.\n");

    /* Handle each client */
    for (int i = 1; i <= client_count; i++)
    {
        char client_to_server[FIFO_NAME_SIZE];
        char server_to_client[FIFO_NAME_SIZE];

        sprintf(client_to_server, "/tmp/client%d_to_server", i);
        sprintf(server_to_client, "/tmp/server_to_client%d", i);

        printf("\nWaiting for Client %d...\n", i);

        int read_fd = open(client_to_server, O_RDONLY);

        if (read_fd == -1)
        {
            perror("open client FIFO");
            continue;
        }

        int write_fd = open(server_to_client, O_WRONLY);

        if (write_fd == -1)
        {
            perror("open server FIFO");
            close(read_fd);
            continue;
        }

        printf("Client %d connected.\n", i);

        /* Read message from client */
        read(read_fd, buffer, BUFFER_SIZE - 1);
        buffer[BUFFER_SIZE - 1] = '\0';

        printf("Client %d says: %s\n", i, buffer);

        /* Prepare response */
        char response[BUFFER_SIZE];

        snprintf(response, BUFFER_SIZE,
                 "Server response to Client %d: Message received successfully.",
                 i);

        /* Send response */
        write(write_fd, response, strlen(response) + 1);

        /* Notify client using SIGUSR1 */
        char pid_string[20];

        printf("Enter PID of Client %d for signal notification: ", i);
        fgets(pid_string, sizeof(pid_string), stdin);

        pid_t client_pid = atoi(pid_string);

        if (client_pid > 0)
        {
            kill(client_pid, SIGUSR1);
        }

        close(read_fd);
        close(write_fd);
    }

    /* Remove FIFOs */
    for (int i = 1; i <= client_count; i++)
    {
        char client_to_server[FIFO_NAME_SIZE];
        char server_to_client[FIFO_NAME_SIZE];

        sprintf(client_to_server, "/tmp/client%d_to_server", i);
        sprintf(server_to_client, "/tmp/server_to_client%d", i);

        unlink(client_to_server);
        unlink(server_to_client);
    }

    printf("\nAll clients handled.\n");
    printf("Server terminated.\n");

    return 0;
}
