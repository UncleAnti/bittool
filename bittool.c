#include "bittool.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
	int fec_encode(FILE*,FILE*, const char*);
	int fec_decode(FILE*,FILE*, const char*);

	int has_char(const char * str, const char c){
		for(size_t idx=0;idx<strlen(str); ++idx)
			if (str[idx] == c)
				return 1;
		return 0;
	}
#ifdef __cplusplus
}
#endif

typedef struct process_t {
	fptr func;
	const char * name;
	char opts[8];
	struct process_t *next;
	FILE *ifd;
	FILE *ofd;
	pthread_t thread;
} process_t;

process_t *head = NULL;
process_t *current = NULL;

const struct option long_options[] = {
		{"encode", required_argument, 0, 'e'},
		{"decode", required_argument, 0, 'd'},
		{"space" , no_argument, 0, 's'},
    {"help"  , no_argument, 0, '?'},
		{0, 0, 0, 0}};

const char usage[] = "(Usage: %s -[e|d] engine [OPTIONS] [-[e|d] engine [OPTIONS]]...\n"
"\t-?, --help         \tDisplay this help and exit\n"
"\t-e, --encode engine\tEncode data\n"
"\t-d, --decode engine\tDecode data\n"
"\t-s, --space        \tAdd a whitespacespace every 8 bits\n";

extern engines_t __start_engine, __stop_engine;

int add_engine(char mode, const char * arg){
	process_t *np = (process_t*)malloc(sizeof(process_t));
	np->next = NULL;
	np->func = NULL;
	memset(np->opts, 0, sizeof(np->opts));

	for (engines_t *e = &__start_engine; e < (engines_t*)&__stop_engine; e++){
		if (strcmp(e->name, arg) == 0 && e->mode == mode){
			char m[2] = { mode, 0 };
			strcat(np->opts, m);
			np->name = e->name;
			np->func = e->func;
		}
	}

	if (np->name == NULL){
		fprintf(stderr, "unknown engine: %s\n", optarg);
		free(np);
		return -1;
	}

	if (head == NULL) {
		head = np;
	} else {
		process_t *it = head;
		for(; it->next != NULL; it=it->next);
		it->next = np;
	}
	current = np;
	return 0;
}

void *thread_run(void * param) {
	process_t *me = (process_t*)param;
	me->func(me->ifd, me->ofd, me->opts);
	fclose(me->ifd);
	fclose(me->ofd);
	return NULL;
}

int main(int argc, char *argv[]){
	int opt = 0;
	int option_index = 0;

	while ((opt = getopt_long(argc, argv, "e:d:s10?", long_options, &option_index)) != -1){
		switch (opt){

		case '1':
		case '0':
		case 's':
			if(current){
			  char nopt[2];
			  snprintf(nopt, 2, "%c", opt);
				strcat(current->opts, nopt);
			}
			break;

		case 'e':
		case 'd':
			if(add_engine(opt, optarg))
				return EXIT_FAILURE;
			break;

		case '?':{
      printf(usage, argv[0]);
			const char * last = "";
			if (&__start_engine != &__stop_engine)
				printf("\n\tEngines:\n");

	    for (engines_t *p = &__start_engine; p < (engines_t*)&__stop_engine; p++){
				if(strcmp(last, p->name) != 0){
					printf("\t%-14s\t%s\n", p->name, p->help);
					last = p->name;
				}
			}

      return EXIT_SUCCESS;
		}

		default:
			fprintf(stderr, "unknown option '%c'", opt);
			if (optarg)
				fprintf(stderr, " with arg %s", optarg);

			fprintf(stderr, "\n");
			return EXIT_FAILURE;
		}
	}

	if (optind < argc){
		fprintf(stderr, "unknown arguments: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
		return EXIT_FAILURE;
	}

	if (!head) // no engines, passthrough
		add_engine('e', "pass");

	size_t numEngines = 0;
	for(process_t* it = head; it != NULL; it = it->next, ++numEngines);

	int p[2], *fd = (int*)malloc(sizeof(int) * numEngines * 2);
	fd[0] = 0; // stdin
	for(size_t idx = 1; idx < (numEngines * 2) - 1; idx += 2){
		if(pipe(p) == 0){
			fd[idx + 0] = p[1];
			fd[idx + 1] = p[0];
		}
	}
	fd[(numEngines * 2) - 1] = 1; // stdout

	process_t* it = head;
	for(size_t idx=0; idx<(numEngines*2)-1; idx += 2){
		it->ifd = fdopen(fd[idx    ], "r");
		it->ofd = fdopen(fd[idx + 1], "w");
		it=it->next;
	}
	free(fd);

	for(process_t* it = head; it != NULL; it=it->next)
		pthread_create(&it->thread, NULL, thread_run, it);

	for(process_t* it = head; it != NULL; it=it->next)
		pthread_join(it->thread, NULL);

	for(process_t* it = head; it != NULL; ) {
		process_t* next = it->next;
		free(it);
		it=next;
	}

}
