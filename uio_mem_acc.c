#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>


int read_value(char *filename, long *dst)
{
	int fd;
	char tmp[64];
	long val;

	fd = open(filename, O_RDONLY);
	if (fd < 0) 
		return -1;

	if (read(fd, tmp, sizeof(tmp)) <= 0) {
		close(fd);
		return -1;
	}

	close(fd);

	*dst = strtol(tmp, NULL, 16);
	return 0;
}

int main(int argc, char **argv)
{
	int fd, status;
	char tmp[64];
	long offset, len;
	void *ptr_bar0;
	uint32_t val;

	if (read_value("/sys/class/uio/uio0/maps/map0/offset", &offset)) {
		perror("reading offset");
		return 1;
	}

	if (read_value("/sys/class/uio/uio0/maps/map0/size", &len)) {
		perror("reading size");
		return 1;
	}

	printf("Offset of BAR0 is 0x%x\n", offset);
	printf("Length of BAR0 mapping is %d\n", len);

	fd = open("/dev/uio0", O_RDWR | O_SYNC);

	if (fd < 0) {
		perror("open /dev/uio0");
		return 1;
	}


	ptr_bar0 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 * 4096);

	if (ptr_bar0 == MAP_FAILED) {
		perror("mmap /dev/uio0");
		close(fd);
		return 1;
	}

	ptr_bar0 += offset;

#define IO_DIR_REG 0xf8
#define IO_DATA_REG1_8 0xfc

	printf("DATA_REG: %x\n", *((uint8_t *) ptr_bar0 + IO_DATA_REG1_8));

	*((uint8_t *) ptr_bar0 + IO_DIR_REG) = 1;
	getchar();
	*((uint8_t *) ptr_bar0 + IO_DIR_REG) = 0;


	munmap(ptr_bar0, len);
	close(fd);

	return 0;
}
