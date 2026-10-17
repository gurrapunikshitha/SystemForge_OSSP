#include <stdio.h>

#define MAX 10

int n, m;
int alloc[MAX][MAX], max[MAX][MAX], need[MAX][MAX];
int avail[MAX], work[MAX];
int finish[MAX], safeSeq[MAX];
int count = 0;

void findSafeSequences(int depth)
{
    if (depth == n)
    {
        count++;
        printf("Safe Sequence %d: ", count);

        for (int i = 0; i < n; i++)
            printf("T%d ", safeSeq[i]);

        printf("\n");
        return;
    }

    for (int i = 0; i < n; i++)
    {
        if (!finish[i])
        {
            int possible = 1;

            for (int j = 0; j < m; j++)
            {
                if (need[i][j] > work[j])
                {
                    possible = 0;
                    break;
                }
            }

            if (possible)
            {
                for (int j = 0; j < m; j++)
                    work[j] += alloc[i][j];

                finish[i] = 1;
                safeSeq[depth] = i;

                findSafeSequences(depth + 1);

                finish[i] = 0;

                for (int j = 0; j < m; j++)
                    work[j] -= alloc[i][j];
            }
        }
    }
}

int main()
{
    printf("Enter number of tenants and resources: ");
    scanf("%d %d", &n, &m);

    if (n < 1 || n > MAX || m < 1 || m > MAX)
    {
        printf("Invalid input!\n");
        return 1;
    }

    printf("Enter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &alloc[i][j]);

    printf("Enter Maximum Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &max[i][j]);

    printf("Enter Available Resources:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &avail[j]);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (alloc[i][j] < 0 || max[i][j] < alloc[i][j])
            {
                printf("Invalid allocation or maximum claim!\n");
                return 1;
            }

            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    for (int j = 0; j < m; j++)
    {
        if (avail[j] < 0)
        {
            printf("Invalid available resources!\n");
            return 1;
        }

        work[j] = avail[j];
    }

    for (int i = 0; i < n; i++)
        finish[i] = 0;

    printf("\nSafe Sequences:\n");
    findSafeSequences(0);

    if (count > 0)
    {
        printf("\nSystem is in a SAFE STATE.\n");
        printf("Total Safe Sequences: %d\n", count);
    }
    else
    {
        printf("\nSystem is in an UNSAFE STATE.\n");
        printf("No complete safe sequence exists.\n");
        printf("Tenants unable to finish with the remaining resources:\n");

        for (int i = 0; i < n; i++)
        {
            int possible = 1;

            for (int j = 0; j < m; j++)
            {
                if (need[i][j] > work[j])
                {
                    possible = 0;
                    break;
                }
            }

            if (!possible)
                printf("T%d ", i);
        }

        printf("\n");
    }

    return 0;
}
