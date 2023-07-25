#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_CHARS 26
#define BUFFSIZE 1000

/**
 * Parse command line arguments.
 * 
 * @param argc The number of command line arguments
 * @param argv The array of command line arguments
 * @param num The number of processes to create
 * @param str The directory containing the text files
 */
void
parse_args (int argc, char *argv[], int *num, char **str)
{
  if (argc != 3)
    {
      printf ("Usage: %s nprocs textdir\n", argv[0]);
      exit (EXIT_FAILURE);
    }
  else if (atoi (argv[1]) < 1)
    {
      printf ("Usage: nprocs must be greater than 0 \n");
      exit (EXIT_FAILURE);
    }

  *num = atoi (argv[1]);
  *str = argv[2];
}

/**
 * Make a trival ring topology.
*/
int
make_trivial_ring ()
{
  int fd[2];
  if (pipe (fd) == -1)
    return (-1);
  if ((dup2 (fd[0], STDIN_FILENO) == -1) ||
      (dup2 (fd[1], STDOUT_FILENO) == -1))
    return (-2);
  if ((close (fd[0]) == -1) || (close (fd[1]) == -1))
    return (-3);
  return (0);
}

/**
 * Add a new node with interprocess communication using pipes.
 * 
 * @param pid The process id
*/
int
add_new_node (int *pid)
{
  int fd[2];
  if (pipe (fd) == -1)
    return (-1);
  if ((*pid = fork ()) == -1)
    return (-2);
  if (*pid > 0 && dup2 (fd[1], STDOUT_FILENO) < 0)
    return (-3);
  if (*pid == 0 && dup2 (fd[0], STDIN_FILENO) < 0)
    return (-4);
  if ((close (fd[0]) == -1) || (close (fd[1]) == -1))
    return (-5);
  return (0);
}

/**
 * Encodes each character count to a string, and write it to the pipe.
 * 
 * @param buff The buffer to write to
 * @param char_counts The array of character counts to write
*/
int
write_to_pipe (char buff[], long char_counts[])
{
  int bytes, len, i;
  char temp_buff[BUFFSIZE];
  for (i = 0; i < MAX_CHARS; i++)
    {
      sprintf (temp_buff, "%ld ", char_counts[i]);
      strcat (buff, temp_buff);
    }
  len = strlen (buff) + 1;
  if ((bytes = write (STDOUT_FILENO, buff, len)) != len)
    {
      fprintf (stderr, "Write of %d bytes failed, only sent %d bytes\n",
	       len, bytes);
      return -1;
    }
  else
    {
      return bytes;
    }
}

/**
 * Reads from the pipe and decodes the character counts.
 * 
 * @param buff The buffer to read from
 * @param char_counts The array of character counts to read
*/
void
read_from_pipe (char buff[], long char_counts[])
{
  int bytes_read, i;
  char *token = NULL;
  if (char_counts == NULL || buff == NULL)
    {
      fprintf (stderr, "Error: char_counts or buff is NULL\n");
      return;
    }
  if ((bytes_read = read (STDIN_FILENO, buff, BUFFSIZE - 1)) > 0)
    {
      token = strtok (buff, " ");
      for (i = 0; i < MAX_CHARS; i++)
	{
	  char_counts[i] = atol ((token == NULL) ? "0" : token);
	  token = strtok (NULL, " ");
	}
    }
  else
    {
      fprintf (stderr, "Issue reading from pipe, recieved %d bytes\n",
	       bytes_read);
    }
}

/**
 * Safely sum the character counts.
 * 
 * @param char_count The array of character counts
 * @param read_count The array of character counts read from the pipe
 * @param total_count The array of character counts after summing
*/
void
safe_sum (long *char_count, long *read_count, long *total_count)
{
  if ((char_count == NULL) || (read_count == NULL) || (total_count == NULL))
    return;

  for (int i = 0; i < MAX_CHARS; i++)
    {
      total_count[i] = char_count[i] + read_count[i];
      if (total_count[i] < 0)
	{
	  fprintf (stderr,
		   "Overflow detected at index %d: %ld + %ld != %ld\n",
		   i, char_count[i], read_count[i], total_count[i]);
	  total_count[i] = 0;
	}
    }
}
