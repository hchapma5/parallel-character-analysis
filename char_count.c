/******************************************************************************************************
 *
 *                                      Author: Harrison Chapman
 *  Date: 24/07/2023
 *
 *  Description:        This program uses parallel processing to perform character analysis
 *                      on a set of text files.
 *
 *  How to compile:     Use the make utility with the provided makefile by running the 'make' command.
 *
 *  How to run:         Run the program with the following command:
 *                      char_count <number of processes> <directory>
 *                      e.g. char_count 2 text_files
 *
 *
 *******************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>
#include "charlib.h"
#include "ringlib.h"

int
main (int argc, char *argv[])
{
  int i;			/* number of this process (starting with 1) */
  int childpid;			/* indicates process should spawn another */
  int nprocs;			/* total number of processes in ring */
  char *textdir;		/* directory containing text files */
  char buff[BUFFSIZE] = { 0 };
  long char_stats[MAX_CHARS] = { 0 }, read_stats[MAX_CHARS] =
    { 0 }, total_stats[MAX_CHARS] = { 0 };
  char **text_files;

  /* parse command line arguments and build array of .txt files */
  parse_args (argc, argv, &nprocs, &textdir);
  text_files = get_text_files (textdir);

  /* Build ring of processes */
  for (i = 1; i < nprocs; i++)
    {
      if (add_new_node (&childpid) < 0)
	{
	  perror ("Could not add new node to ring");
	  exit (EXIT_FAILURE);
	};
      if (childpid)
	{
	  break;
	}
    };
  /* ring of processes code */
  {
    /* Distribute files to each process and count characters */
    for (int j = i - 1; j < sizeof (*text_files) - 1; j += nprocs)
      {
	count_file_characters (text_files[j], char_stats);
	fprintf (stderr, "Process %d: calculating characters in file: %s\n",
		 i, text_files[j]);
      }
    free_text_files (text_files);

    /* Interprocess communication */
    if (nprocs == 1)
      {
	report_stats (char_stats);
	exit (EXIT_SUCCESS);
      }
    if (i == 1)
      {
	write_to_pipe (buff, char_stats);
      }
    if (i > 1)
      {
	read_from_pipe (buff, read_stats);
	safe_sum (char_stats, read_stats, total_stats);
	if (i != nprocs)	/* if not the last process */
	  {
	    memset (buff, 0, sizeof (buff));	// clear buffer
	    write_to_pipe (buff, total_stats);

	  }
	else
	  {
	    report_stats (total_stats);
	  }
      }
    wait (&childpid);
    exit (EXIT_SUCCESS);
  }
  return 0;
}

/* end of main program here */
