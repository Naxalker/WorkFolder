#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

typedef struct file_info {
    char file_path[4096];
    int bytes;
} file_info;

void compress_to_archive();
void uncompress();
void add_directory_to_archive(char *directory_name, FILE *archive, FILE *info);
void add_file_to_archive(char *filename, FILE *archive, FILE *info);
void add_info_to_archive(FILE *info, FILE* archive);
file_info *div_into_files(char *str, int *elements);
void create_file(file_info file_info, FILE *archive);
void error(char *msg);
char first_input();
int number_of_digits(int number);

#endif