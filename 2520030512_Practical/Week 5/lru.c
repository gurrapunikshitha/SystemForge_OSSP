#include <stdio.h>

int main()
{
    int pages[100], frames[20], time[20];
    int n, f, faults = 0, hits = 0, counter = 0;

    printf("Enter number of pages: ");
    scanf("%d", &n);

    if (n < 1 || n > 100)
    {
        printf("Invalid number of pages!\n");
        return 1;
    }

    printf("Enter page reference string:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &pages[i]);

    printf("Enter number of frames: ");
    scanf("%d", &f);

    if (f < 1 || f > 20)
    {
        printf("Invalid number of frames!\n");
        return 1;
    }

    for (int i = 0; i < f; i++)
    {
        frames[i] = -1;
        time[i] = 0;
    }

    printf("\nPage\tFrames\t\tStatus\n");

    for (int i = 0; i < n; i++)
    {
        int found = 0;
        counter++;

        for (int j = 0; j < f; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                time[j] = counter;
                hits++;
                break;
            }
        }

        if (!found)
        {
            int pos = -1;
            faults++;

            for (int j = 0; j < f; j++)
            {
                if (frames[j] == -1)
                {
                    pos = j;
                    break;
                }
            }

            if (pos == -1)
            {
                pos = 0;

                for (int j = 1; j < f; j++)
                {
                    if (time[j] < time[pos])
                        pos = j;
                }
            }

            frames[pos] = pages[i];
            time[pos] = counter;
        }

        printf("%d\t", pages[i]);

        for (int j = 0; j < f; j++)
        {
            if (frames[j] == -1)
                printf("- ");
            else
                printf("%d ", frames[j]);
        }

        printf("\t%s\n", found ? "Hit" : "Page Fault");
    }

    printf("\nTotal Page Faults = %d\n", faults);
    printf("Total Hits = %d\n", hits);
    printf("Page Fault Ratio = %.2f\n", (float)faults / n);

    return 0;
}
