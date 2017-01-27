/*
 * Name: Shaun Sartin
 * ID #: 1000992189
 * Programming Assignment 1
 * Description: A simplifed command shell which uses fork() and exec()
 */

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // BEGIN STUDENT CODE
    // Copy command arguments into a new array, so that we can pass it to exec()
    char *exec_arg[MAX_NUM_ARGUMENTS - 1];
    for( int i = 1; i < MAX_NUM_ARGUMENTS; i++ )
    {
      exec_arg[i - 1] = token[i];
    }

    //******* BEGIN DECLARATION OF VARIABLES********

    // Create a new process so that we can call exec() and keep the shell running
    pid_t pid = fork();

    // Rename/Relocate data, so that the variable name is more intuitive
    char *cmd = token[0];

    // Create path strings, so that we can strcat the cmd to them
    // NOTE: There must be some unused indexes in the char array,
    // so that the cmd can be appended
    char path2[30] = "/usr/local/bin/";
    char path3[30] = "/usr/bin/";
    char path4[30] = "/bin/";

    //******* END DECLARATION OF VARIABLES********

    //Concat the inputted command to each of the path strings
    //TODO add if statement for when cmd is null. It causes seg. fault
    // strcat(path2, cmd);
    // strcat(path3, cmd);
    // strcat(path4, cmd);

    if( pid == -1 )
    {
      printf("An error occurred while using fork()\n");
      return(-1);
    }

    // The child process will execute the entered command, if possible
    // The order of the directories searched is as follows:
    // 1. The current directory
    // 2. /usr/local/bin
    // 3. /usr/bin
    // 4. /bin
    else if( pid == 0 )
    {
      execv(cmd, exec_arg);
      execv(path2, exec_arg);
      execv(path3, exec_arg);
      execv(path4, exec_arg);
      //TODO remove printf() see RQ 006
      printf("All exec() function calls failed.\n");
    }

    else
    {
      wait(&pid);
    }
    // END STUDENT CODE

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ )
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );
    }

    free( working_root );

  }
  return 0;
}
