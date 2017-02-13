/*
 * Name: Shaun Sartin
 * ID #: 1000992189
 * Programming Assignment 1 (Proc)
 * Description: This program has 2 modes, default-mode and verbose-mode.
 * Default-mode will display some system info and then terminate.
 * Verbose-mode will display more system info every specified interval and
 * will not terminate unless forced to do so. (For Example: Ctrl+C)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING_LENGTH 255

/*
 * Function: printProc
 * Parameter 1: userTime - A float representing the time (in milliseconds) that
 * the system has spent in user mode.
 * Parameter 2: systemTime - A float representing the time (in milliseconds) that
 * the system has spent in system mode.
 * Parameter 3: systemTime - A float representing the time (in milliseconds) that
 * the system has spent idling.
 * Parameter 4: ctxtSwitches - A float representing the number of context switches
 * that have taken place since the system's last boot.
 * Parameter 5: processCreations - A float representing the number of processes
 * that have been created since the system's last boot.
 * Parameter 6: totalMem - A float representing the total amount of physical RAM.
 * Parameter 7: freeMem - A float representing the amount of physical RAM unused
 * by the system.
 * Parameter 8: diskStats - A float representing the number of disk reads/writes
 * since the system's last boot.
 * Parameter 9: uptime - A float representing the number of seconds since the system's
 * last boot.
 * Description: This function is only used in the verbose mode.
 * It calculates the percent values and average rates since system uptime
 * It also prints all of the calculated values, except for totalTime, to stdout.
 */
void printProc(float userTime, float systemTime, float idleTime, float ctxtSwitches,
  float processCreations, float totalMem, float freeMem, float diskStats, float uptime)
{
  float totalTime = userTime + systemTime + idleTime;

  float userPercent = (userTime / totalTime) * 100;
  float systemPercent = (systemTime / totalTime) * 100;
  float idlePercent = (idleTime / totalTime) * 100;
  float memPercent = (freeMem / totalMem) * 100;

  float diskRate = (diskStats / uptime);
  float ctxtRate = (ctxtSwitches / uptime);
  float processRate = (processCreations / uptime);

  printf("Percentage of time in user-mode: %f%%\n", userPercent);
  printf("Percentage of time in system-mode: %f%%\n", systemPercent);
  printf("Percentage of time spent idling: %f%%\n", idlePercent);
  printf("Amount of free memory in RAM: %d kB (%f%%)\n", (int) freeMem, memPercent);
  printf("Number of disk reads/writes per second (since last boot): %f\n", diskRate);
  printf("Number of context switches per second (since last boot): %f\n", ctxtRate);
  printf("Number of processes created per second (since last boot): %f\n", processRate);
}

int main(int argc, char** argv)
{

  // This is used to open all the necessary files in /proc
  FILE * filePointer;

  // These strings hold information that is displayed in 'default-mode'
  char processorString[MAX_STRING_LENGTH];
  char kernelString[MAX_STRING_LENGTH];
  char totalMemString[MAX_STRING_LENGTH];
  char uptimeString[MAX_STRING_LENGTH];

  // This string is overwritten many times. It is used as a place to dump
  // irrelevant information, as well as perform string comparisons for
  // certain strings in the /proc files.
  char dummyString[MAX_STRING_LENGTH];

  // This variable will hold the time (in seconds) since the system has last booted
  int uptime;

  // These variables contain information from /proc/stat.
  // They are only used in 'verbose-mode'
  int userTime;
  int systemTime;
  int idleTime;
  int ctxtSwitches;
  int processCreations;

  // These variables hold information from /proc/meminfo
  // totalMem is used in both modes, whereas freeMem is only used in 'verbose-mode'
  int totalMem;
  int freeMem;

  // This variable contains the number of disk sectors that have been read
  // That information is pulled from /proc/diskstats
  int diskStats;

  // This variable is set via an argument when executing this file
  // Therefore, it is only relevant for 'verbose-mode'.
  // It determines how long to wait before performing calculations and printing.
  int sleepTime;


  // If an argument has been passed, run 'verbose-mode'
  // 'verbose-mode' refers to everything in this if-statement.
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

        // Open the uptime file
        filePointer = fopen("/proc/uptime", "r");
        if (filePointer == NULL)
        {
          printf("Failed to open uptime file.\n");
          return -1;
        }
        fscanf(filePointer, "%d", &uptime);

        // Close the uptime file
        fclose(filePointer);

        printProc(userTime, systemTime, idleTime, ctxtSwitches, processCreations,
          totalMem, freeMem, diskStats, uptime);

        sleep(sleepTime);
        printf("\n");
    }
  }

  // This 'else' block holds the 'default-mode' functionality of the program
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
