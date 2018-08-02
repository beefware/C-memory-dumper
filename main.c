#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define PAGE_LENGTH 4096
#define BUFFER_SIZE 25

int main(int argc, char **argv) 
  {
	char memn[BUFFER_SIZE], mapn[BUFFER_SIZE], line[256];
	unsigned long start_address, end_address, address;
    unsigned char page[PAGE_LENGTH];
	
	if (argc != 2) 
	  {
		printf("{*} %s {pid}\n", argv[0]);
		return EXIT_FAILURE;
	  }

	pid_t pid = atoi(argv[1]);

	snprintf(memn, BUFFER_SIZE, "/proc/%d/mem", pid);
	snprintf(mapn, BUFFER_SIZE, "/proc/%d/maps", pid);

	int mapfd = open(mapn, O_RDONLY);
	int memfd = open(memn, O_RDONLY);
	
	if ((mapfd == -1) || (memfd == -1))
	  {
		printf("{*} %s\n", strerror(errno));
		return EXIT_FAILURE;
	  }
		
	while (0 <= read(mapfd, &line, 256))
	  {
            sscanf(line, "%08lx-%08lx\n", &start_address, &end_address);
			long length = end_address - start_address;
			lseek(memfd, start_address, SEEK_SET);
			for (address=start_address; address < start_address + length; address += PAGE_LENGTH)
    		  {
				int fd = read(memfd, &page, PAGE_LENGTH);
				if (fd <= 0)
				  {
					return EXIT_FAILURE;
				  }
				fwrite(&page, 1, PAGE_LENGTH, stdout);
			  }

	  }
	  
	return EXIT_SUCCESS;
  }

# C-memory-dumper
