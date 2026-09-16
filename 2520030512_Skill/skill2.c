#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Command
{
    char *command;
    struct Command *next;
} Command;

/* Add command to linked-list history */
void add_command(Command **head, const char *input)
{
    Command *new_node;

    new_node = malloc(sizeof(Command));

    if (new_node == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    new_node->command = malloc(strlen(input) + 1);

    if (new_node->command == NULL)
    {
        perror("malloc");
        free(new_node);
        exit(EXIT_FAILURE);
    }

    strcpy(new_node->command, input);

    new_node->next = *head;
    *head = new_node;
}

/* Free all dynamically allocated memory */
void free_commands(Command *head)
{
    Command *temp;

    while (head != NULL)
    {
        temp = head;
        head = head->next;

        free(temp->command);
        free(temp);
    }
}

int main()
{
    char *buffer;
    size_t buffer_size = 10;
    size_t length;

    Command *history = NULL;

    /* Dynamically allocate input buffer */
    buffer = malloc(buffer_size);

    if (buffer == NULL)
    {
        perror("malloc");
        return 1;
    }

    printf("Dynamic Command Shell\n");
    printf("Type commands. Type exit to quit.\n");

    while (1)
    {
        printf("myshell> ");
        fflush(stdout);

        length = 0;

        /* Read command character by character */
        while (1)
        {
            int ch = getchar();

            if (ch == '\n' || ch == EOF)
                break;

            /* Resize buffer when required */
            if (length + 1 >= buffer_size)
            {
                buffer_size *= 2;

                char *temp = realloc(buffer, buffer_size);

                if (temp == NULL)
                {
                    perror("realloc");
                    free(buffer);
                    free_commands(history);
                    return 1;
                }

                buffer = temp;
            }

            buffer[length++] = ch;
        }

        buffer[length] = '\0';

        /* Exit condition */
        if (strcmp(buffer, "exit") == 0)
            break;

        /* Ignore empty commands */
        if (length == 0)
            continue;

        /* Store command in linked list */
        add_command(&history, buffer);

        printf("Command stored: %s\n", buffer);
    }

    /* Release memory */
    free(buffer);
    free_commands(history);

    printf("Memory released successfully.\n");

    return 0;
}
