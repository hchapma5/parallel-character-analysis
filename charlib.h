#define MAX_CHARS 26
#define BARLENGTH 30
void count_file_characters(const char *filename, long *char_stats);
void generate_histogram(long char_count, long char_stats[]);
void report_stats(long *char_stats);
char **get_text_files(const char *directory);
void free_text_files(char **text_files);