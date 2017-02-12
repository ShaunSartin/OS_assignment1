#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING_LENGTH 255

//TODO ADD Description
void printProc(float userTime, float systemTime, float idleTime, int ctxtSwitches,
  int processCreations, float totalMem, float freeMem, float diskStats)
{
  float totalTime = userTime + systemTime + idleTime;
  float userPercent = (userTime / totalTime) * 100;
  float systemPercent = (systemTime / totalTime) * 100;
  float idlePercent = (idleTime / totalTime) * 100;
  float memPercent = (freeMem / totalMem) * 100;

  printf("userTime: %f%%\n", userPercent);
  printf("systemTime: %f%%\n", systemPercent);
  printf("idleTime: %f%%\n", idlePercent);
  printf("freeMem: %d kB(%f%%)\n", (int) freeMem, memPercent);
  printf("diskStats: %d\n", (int) diskStats);
  printf("ctxtSwitches: %d\n", ctxtSwitches);
  printf("processCreations: %d\n", processCreations);
}

int main(int argc, char** argv)
{

  FILE * filePointer;
  char processorString[MAX_STRING_LENGTH];
  char kernelString[MAX_STRING_LENGTH];
  char totalMemString[MAX_STRING_LENGTH];
  char uptimeString[MAX_STRING_LENGTH];
  char dummyString[MAX_STRING_LENGTH];

  int userTime;
  int systemTime;
  int idleTime;
  int ctxtSwitches;
  int processCreations;
  int totalMem;
  int freeMem;
  int diskStats;
  int sleepTime;

  if (argv[1] != 0)
  {
    sleepTime = strtol(argv[1], NULL, 10);
    while(1)
    {

        filePointer = fopen("/proc/stat", "r");
        if (filePointer == NULL)
        {
          printf("Failed to open stat file.\n");
          return -1;
        }

        fscanf(filePointer, "%s %d %s %d %d", dummyString, &userTime, dummyString,
          &systemTime, &idleTime);

        // Skip past everything in the file until we find 'ctxt'
        while(strcmp(dummyString, "ctxt") != 0)
        {
          memset(dummyString, 0, sizeof(dummyString));
          fscanf(filePointer, "%s", dummyString);
        }

        // Store the number immediately after ctxt
        fscanf(filePointer, "%d", &ctxtSwitches);

        // Skip past everything in the file until we find 'processes'
        while(strcmp(dummyString, "processes") != 0)
        {
          memset(dummyString, 0, sizeof(dummyString));
          fscanf(filePointer, "%s", dummyString);
        }

        // Store the number that appears immediately after ctxt
        fscanf(filePointer, "%d", &processCreations);

        // Close the stat file.
        fclose(filePointer);

        // Open the meminfo file
        filePointer = fopen("/proc/meminfo", "r");
        if (filePointer == NULL)
        {
          printf("Failed to open meminfo file.\n");
          return -1;
        }

        // Skip past everything in the file until we find 'MemTotal:'
        while(strcmp(dummyString, "MemTotal:") != 0)
        {
          memset(dummyString, 0, sizeof(dummyString));
          fscanf(filePointer, "%s", dummyString);
        }

        // Store the number that appears immediately after 'MemTotal:'
        fscanf(filePointer, "%d", &totalMem);

        // Skip past everything in the file until we find 'MemFree:'
        while(strcmp(dummyString, "MemFree:") != 0)
        {
          memset(dummyString, 0, sizeof(dummyString));
          fscanf(filePointer, "%s", dummyString);
        }

        // Store the number that appears immediately after 'MemFree:'
        fscanf(filePointer, "%d", &freeMem);

        // Close the meminfo file
        fclose(filePointer);

        // Open the diskstats file
        filePointer = fopen("/proc/diskstats", "r");
        if (filePointer == NULL)
        {
          printf("Failed to open diskstats file.\n");
          return -1;
        }

        // Skip past everything in the file until we find 'sda'
        while(strcmp(dummyString, "sda") != 0)
        {
          memset(dummyString, 0, sizeof(dummyString));
          fscanf(filePointer, "%s", dummyString);
        }

        // Ignore the first two numbers after sda. Store the 3rd.
        fscanf(filePointer, "%s", dummyString);
        fscanf(filePointer, "%s", dummyString);
        fscanf(filePointer, "%d", &diskStats);

        // Close the diskstats file
        fclose(filePointer);

        printProc(userTime, systemTime, idleTime, ctxtSwitches, processCreations, totalMem, freeMem, diskStats);

        sleep(sleepTime);
        printf("\n");
    }
  }

  else
  {
    //Start working with data in cpuinfo file
    filePointer = fopen("/proc/cpuinfo", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open cpuinfo file.\n");
      return -1;
    }

    // Skip past the first 4 lines. The information we need is on the 5th line.
    int lineCount;
    for(lineCount = 1; lineCount < 5; lineCount++)
    {
      fgets(dummyString, MAX_STRING_LENGTH, filePointer);
    }
    fgets(processorString, MAX_STRING_LENGTH, filePointer);
    fclose(filePointer);

    //Start working with data in version file
    filePointer = fopen("/proc/version", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open version file.\n");
      return -1;
    }
    fgets(kernelString, MAX_STRING_LENGTH, filePointer);
    fclose(filePointer);

    //Start working with data in meminfo file
    filePointer = fopen("/proc/meminfo", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open meminfo file.\n");
      return -1;
    }
    fgets(totalMemString, MAX_STRING_LENGTH, filePointer);
    fclose(filePointer);

    //Start working with data in uptime file
    filePointer = fopen("/proc/uptime", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open uptime file.\n");
      return -1;
    }
    fscanf(filePointer, "%s", uptimeString);
    fclose(filePointer);

    printf("%s", processorString);
    printf("%s", kernelString);
    printf("%s", totalMemString);
    printf("System uptime: %s seconds\n", uptimeString);
  }

  return 0;
}
