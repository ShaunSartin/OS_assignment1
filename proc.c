#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 255

int main(int argc, char** argv)
{

  FILE * filePointer;
  char processorString[MAX_STRING_LENGTH];
  char kernelString[MAX_STRING_LENGTH];
  char totalMemString[MAX_STRING_LENGTH];
  char uptimeString[MAX_STRING_LENGTH];
  char dummyString[MAX_STRING_LENGTH];

  if (argv[1] != 0)
  {
    // verbose
  }

  else
  {
    //default
    //Start working with data in cpuinfo file
    filePointer = fopen("/proc/cpuinfo", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open cpuinfo folder.\n");
      return -1;
    }

    // Skip past the first 4 lines. The information we need is on the 5th line.
    for(int lineCount = 1; lineCount < 5; lineCount++)
    {
      fgets(dummyString, MAX_STRING_LENGTH, filePointer);
    }
    fgets(processorString, MAX_STRING_LENGTH, filePointer);
    fclose(filePointer);

    //Start working with data in version file
    filePointer = fopen("/proc/version", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open version folder.\n");
      return -1;
    }
    fgets(kernelString, MAX_STRING_LENGTH, filePointer);
    fclose(filePointer);

    //Start working with data in meminfo file
    filePointer = fopen("/proc/meminfo", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open meminfo folder.\n");
      return -1;
    }
    fgets(totalMemString, MAX_STRING_LENGTH, filePointer);
    fclose(filePointer);

    //Start working with data in uptime file
    filePointer = fopen("/proc/uptime", "r");
    if (filePointer == NULL)
    {
      printf("Failed to open uptime folder.\n");
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
