#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int flag;

struct mode_bits_t {
	unsigned char tickless:1;
	unsigned char affinity:1;
	unsigned char fork_test:1;
	unsigned char unused :5;
} __attribute__((__packed__));

static union mode_t {
	struct mode_bits_t bits;
 	unsigned char val;
} mode;

int set_tickless(void) {

	int fd;
	int ret;

	fd = open("/proc/self/nohz", O_WRONLY);

	if(-1 == fd) {
		perror("open");
		ret = -1;
		goto out;
	}

	ret = write(fd, "1\n", 2);

	if(ret != 2) {
		perror("write");
		ret = -1;
	} else
		ret = 0;

	close(fd);
out:
	return ret;
}


int set_affinity(int cpu) {

	cpu_set_t cpus;
	int ret;

	CPU_ZERO(&cpus);
        CPU_SET(cpu, &cpus);

        ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpus);

        if(-1 == ret)
                perror("sched_setaffinity");
        

	return ret;
}


void sig_func(int signum) {

	flag = signum;
	
	return;

}

void run_test(int do_fork_test) {

	volatile unsigned int i;
	int pid = 0;

	signal(SIGINT, sig_func);
	signal(SIGHUP, sig_func);
	signal(SIGKILL, sig_func);

	while(1) {
		i++;

		if(flag) {
			printf("i == %u\n", i);
			if(flag != SIGHUP)
				break;
			else
				flag = 0;
		}

		if(do_fork_test && (!pid) && (i == -1)) {
			pid = fork();
		
			if(pid == -1) {
				pid = 0;
				perror("fork");
				goto out;
			}

			if(!pid) { 
				/* I'm the child */
				sleep(3);
				printf("Yoohhooo!!!\n");
				break;
			}
		}
	}

out:
	if(pid) {
		printf("Finished.\n");
		waitpid(pid, NULL, 0);
	}

	return;
}


void parse_mode(int argc, char * argv[]) {

	if(argc == 1) {
		mode.bits.tickless = 1;
		mode.bits.affinity = 1;
	} else
		mode.val = atoi(argv[1]);

	return;
}


int main (int argc, char * argv[]) {


	parse_mode(argc, argv);


	if(mode.bits.affinity) {
		printf("Will set affinity to 3rd processor\n");
		set_affinity(3);
	}
	
	if(mode.bits.tickless) {
		printf("Will request tickless mode\n");
		set_tickless();
	}
	
	if(mode.bits.fork_test) {
		printf("Will perform test fork after 100k iterations\n");
	}
	
	run_test(mode.bits.fork_test);

	return EXIT_SUCCESS;
}
