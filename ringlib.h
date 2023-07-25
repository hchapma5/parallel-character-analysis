#define BUFFSIZE 256
void parse_args(int argc, char *argv[], int *num, char **str);
int make_trivial_ring();
int add_new_node(int *pid);
int write_to_pipe (char buff[], long char_counts[]);
void read_from_pipe(char buff[], long char_counts[]);
void safe_sum (long *char_count, long *read_count, long* total_count);