#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_CHARS 26
#define BARLENGTH 30

/**
 * Counts the number of characters in a file and stores the count in char_stats
 * 
 * @param filename The name of the file to count characters in
 * @param char_stats An array of longs to store the character counts in
*/
void
count_file_characters (const char *filename, long *char_stats)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      fprintf (stderr, "Failed to open file: %s\n", filename);
      return;
    }

  char c;
  while ((c = fgetc (file)))
    {
      /* break if end of file */
      if (c == EOF)
	{
	  break;
	}
      else if ((tolower (c) - 'a') >= 0 && (tolower (c) - 'a') < 26)
	{
	  char_stats[tolower (c) - 'a'] += 1;
	}
      else
	{
	  continue;
	}
    }

  fclose (file);
}

/**
 * Generates a histogram of the character counts
 * 
 * @param char_count The total count for a particular character
 * @param char_stats An array of longs containing the character counts
*/
void
generate_histogram (long char_count, long char_stats[])
{

  long max_count = char_stats[0];	// Assume the first element is the largest

  for (int i = 1; i < sizeof (*char_stats); i++)
    {
      if (char_stats[i] > max_count)
	{
	  max_count = char_stats[i];	// Update the largest value
	}
    }
  long stars = (char_count * BARLENGTH) / max_count;

  for (int i = 0; i <= stars; i++)
    {
      fprintf (stderr, "*");
    }
}

/**
 * Reports the character counts and generates a histogram for each character
 * 
 * @param char_stats An array of longs containing the character counts
*/
void
report_stats (long *char_stats)
{
  for (int i = 0; i < MAX_CHARS; i++)
    {
      if (char_stats[i] > 0)
	{
	  fprintf (stderr, "\nTotal number of char %c: %ld\t| ",
		   'a' + i, char_stats[i]);
	  generate_histogram (char_stats[i], char_stats);
	}
    }
  fprintf (stderr, "\n");
}

/**
 * Retrieve a list of text files in a given directory
 * 
 * @param directory The directory to search for text files
*/
char **
get_text_files (const char *directory)
{
  DIR *dir;
  struct dirent *entry;
  char **text_files = NULL;
  int count = 0;

  dir = opendir (directory);
  if (dir == NULL)
    {
      perror ("Unable to open directory");
      return NULL;
    }

  while ((entry = readdir (dir)) != NULL)
    {
      if (entry->d_type == DT_REG)
	{
	  char *name = entry->d_name;
	  if (strstr (name, ".txt") != NULL)
	    {
	      char *file_path = malloc (strlen (directory) + strlen (name) + 2);	// +2 for "/" and null terminator
	      sprintf (file_path, "%s/%s", directory, name);
	      count++;
	      text_files = realloc (text_files, count * sizeof (char *));
	      text_files[count - 1] = file_path;
	    }
	}
    }

  closedir (dir);

  if (count == 0)
    {
      printf ("No .txt files found.\n");
      return NULL;
    }

  text_files = realloc (text_files, (count + 1) * sizeof (char *));
  text_files[count] = NULL;	// Mark the end of the array with NULL

  return text_files;
}

/**
 * Free the memory allocated for the text files
 * 
 * @param text_files An array of strings containing the text files
*/
void
free_text_files (char **text_files)
{
  if (text_files == NULL)
    return;

  for (int j = 0; text_files[j] != NULL; j++)
    {
      free (text_files[j]);
    }

  free (text_files);
}
