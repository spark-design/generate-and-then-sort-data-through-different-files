#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-i inputFile> <-o outputFile>\n", prog);
    exit(1);
}


static int cmpkey(const void *p1, const void *p2) {
    rec_t *r1 = (rec_t*) p1;
    rec_t *r2 = (rec_t*) p2;
    return r1->key - r2->key;
}


int
main(int argc, char *argv[])
{
    // arguments
    char *inFile = "/no/such/file";
    char *outFile = "/no/such/file";

    // input params
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
        switch (c) {
        case 'i':
            inFile = strdup(optarg);
            break;
        case 'o':
            outFile = strdup(optarg);
            break;
        default:
            usage(argv[0]);
        }
    }

    // open input file
    int fd = open(inFile, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    struct stat statbuf;
    if (fstat(fd, &statbuf)!=0) {
        fprintf(stderr,"Error getting the stats of the input file");
        exit(-1);
    }
    off_t inputFile_size = statbuf.st_size;
    unsigned int number_of_records = inputFile_size/sizeof(rec_t);
    
    
    //Allocating array for the records
    rec_t *records = calloc(number_of_records, sizeof(rec_t));
    int rc;
    for (int i=0; i<number_of_records; i++) {
        rc = read(fd, records+i, sizeof(rec_t));
        if (rc == 0) // 0 indicates EOF
            break;
        if (rc < 0) {
            perror("read");
            exit(1);
        }
    }
    
    //Closing the input file descriptor
    if (close(fd)!=0) {
        fprintf(stderr,"Error closing the input file descriptor");
        exit(-1);
    }
    
    
    //Sorting the elements
    qsort(records, number_of_records, sizeof(rec_t), &cmpkey);
    
    //Opening the output file descriptor
    fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    //Writing the sorted elements
    for (int i=0; i< number_of_records; i++) {
        rc = write(fd, records+i, sizeof(rec_t));
        if (rc < 0) {
            fprintf(stderr,"Error writing the records to disk");
            exit(-1);
        }
    }
    
    //Closing the output file descriptor
    if (close(fd)!=0) {
        fprintf(stderr,"Error closing the input file descriptor");
        exit(-1);
    }
    
    //De-allocating records
    free(records);
    return 0;
}
