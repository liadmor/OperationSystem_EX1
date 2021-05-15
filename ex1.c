/*
 * ex1.c
 liad mor
 208508259
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 65536
#define DESTINATION_FILE_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

extern int opterr, optind;

void exit_with_usage(const char *message) {
	fprintf (stderr, "%s\n", message);
	fprintf (stderr, "Usage:\n\tex1 [-f] BUFFER_SIZE SOURCE DEST\n");
	exit(EXIT_FAILURE);
}

void append_file(const char *source_file, const char *dest_file, int buffer_size, int force_flag) {
	/*
	 * Append source_file content to dest_file, buffer_size bytes at a time.
	 * If force_flag is true, then also create dest_file if does not exist. Otherwise print error, and exit.
	 *
	 * TODO:
	 * 	1. Open source_file for reading V
	 * 	2. Open dest_file for writing (Hint: is force_flag true?)
	 * 	3. Loop reading from source and writing to the destination buffer_size bytes each time
	 * 	4. Close source_file and dest_file
	 *
	 *  ALWAYS check the return values of syscalls for errors!
	 *  If an error was found, use perror(3) to print it with a message, and then exit(EXIT_FAILURE)
	 */

	int sourcOpen = open(source_file, O_RDONLY);

	if (sourcOpen == -1)
	{
		perror("Unable to open source file for reading");
		exit(EXIT_FAILURE);
	}
	else
	{
		int destOpen;
		if (force_flag == 1)
		{ 
			destOpen = open(dest_file, O_WRONLY | O_CREAT | O_APPEND );
		}
		else
		{ 
			destOpen = open(dest_file, O_WRONLY |  O_APPEND );
		}
		if (destOpen == -1)
		{
			perror("Unable to open destination file for writing");
			close(sourcOpen);
			exit(EXIT_FAILURE);
		}
		char buffer[buffer_size];
		int readSource = read(sourcOpen, buffer, buffer_size);
		while(readSource != 0){
			if (readSource == -1){
				perror("Unable to read source file");
				close(sourcOpen);
				close(destOpen);
				exit(EXIT_FAILURE);
			}
			else{
				int writeDest = write(destOpen, buffer, readSource);
				if (writeDest == -1){
					perror("Unable to append to destination file");
					close(sourcOpen);
					close(destOpen);
					exit(EXIT_FAILURE);
				}
				else{
					if (writeDest != readSource){
						perror("Unable to append buffer content to destination file");
						close(sourcOpen);
						close(destOpen);
						exit(EXIT_FAILURE);
					}
				}
			}
			readSource = read(sourcOpen, buffer, buffer_size);
		}
		int closeSource = close(sourcOpen);
		if (closeSource == -1){
			perror("• Unable to close source file");
			close(destOpen);
			exit(EXIT_FAILURE);
		}
		int closeDest = close(destOpen);
		if (closeDest == -1){
			perror("• Unable to close destination file");
			exit(EXIT_FAILURE);
		}
		printf("Content from file %s was successfully appended to %s\n", source_file,dest_file);
	}




}

void parse_arguments(
		int argc, char **argv,
		char **source_file, char **dest_file, int *buffer_size, int *force_flag) {
	/*
	 * parses command line arguments and set the arguments required for append_file
	 */
	int option_character;

	opterr = 0; /* Prevent getopt() from printing an error message to stderr */

	while ((option_character = getopt(argc, argv, "f")) != -1) {
		switch (option_character) {
		case 'f':
			*force_flag = 1;
			break;
		default:  /* '?' */
			exit_with_usage("Unknown option specified");
		}
	}

	if (argc - optind != 3) {
		exit_with_usage("Invalid number of arguments");
	} else {
		*source_file = argv[argc-2];
		*dest_file = argv[argc-1];
		*buffer_size = atoi(argv[argc-3]);

		if (strlen(*source_file) == 0 || strlen(*dest_file) == 0) {
			exit_with_usage("Invalid source / destination file name");
		} else if (*buffer_size < 1 || *buffer_size > MAX_BUFFER_SIZE) {
			exit_with_usage("Invalid buffer size");
		}
	}
}

int main(int argc, char **argv) {
	int force_flag = 0; /* force flag default: false */
	char *source_file = NULL;
	char *dest_file = NULL;
	int buffer_size = MAX_BUFFER_SIZE;

	parse_arguments(argc, argv, &source_file, &dest_file, &buffer_size, &force_flag);

	append_file(source_file, dest_file, buffer_size, force_flag);

	return EXIT_SUCCESS;
}
