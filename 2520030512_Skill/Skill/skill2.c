#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Command
{
    char *command;
    struct Command *next;
} Command;
