#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define BUFFER_SIZE 256
#define FIFO_NAME_SIZE 100

volatile sig_atomic_t signal_received = 0;

/* Signal handler */
void signal_handler(int sig)
{
    signal_received = 1;
    printf("\n[Client] Signal received from server.\n");
}

int main()
{
    int client_number;
    char buffer[BUFFER_SIZE];

    /* Install signal handler */
    signal(SIGUSR1, signal_handler);

    printf("========== CLIENT ==========\n");
    printf("Client PID: %d\n", getpid());

    printf("Enter client number: ");
    scanf("%d", &client_number);
    getchar();

    char client_to_server[FIFO_NAME_SIZE];
    char server_to_client[FIFO_NAME_SIZE];

    sprintf(client_to_server, "/tmp/client%d_to_server", client_number);
    sprintf(server_to_client, "/tmp/server_to_client%d", client_number);

    printf("Waiting for server...\n");

    /* Open Client -> Server FIFO */
    int write_fd = open(client_to_server, O_WRONLY);

    if (write_fd == -1)
    {
        perror("open client-to-server FIFO");
        return 1;
    }

    /* Open Server -> Client FIFO */
    int read_fd = open(server_to_client, O_RDONLY);

    if (read_fd == -1)
    {
        perror("open server-to-client FIFO");
        close(write_fd);
        return 1;
    }

    printf("Connected to server.\n");

    /* Send message */
    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';

    write(write_fd, buffer, strlen(buffer) + 1);

    printf("Message sent to server.\n");

    /* Wait for server response */
    memset(buffer, 0, BUFFER_SIZE);

    read(read_fd, buffer, BUFFER_SIZE - 1);

    printf("Server response: %s\n", buffer);

    printf("Waiting for signal...\n");

    sleep(1);

    close(write_fd);
    close(read_fd);

    printf("Client terminated.\n");

    return 0;
}
