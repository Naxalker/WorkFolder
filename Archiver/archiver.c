#include "archiver.h"

int main() {
    char input = first_input();
    if (input == '1') {
        compress_to_archive();
    } else if (input == '2') {
        uncompress();
    }
}

void uncompress() {
    char archive_name[256], *info, ch;
    int info_size = 0, number_of_files;
    FILE *archive;
    file_info *file_info;
    struct stat st;

    printf("Enter the archive name: ");
    scanf("%255s", archive_name);
    archive = fopen(archive_name, "rb");

    if (archive == NULL) {
        error("failed to open archive");
    }

    while (stat(archive_name, &st) == -1) {

    }

    if (stat(archive_name, &st) == -1) {
        if (mkdir(archive_name, 0777) == -1) {
            error("failed to create folder");
        }
        chdir(archive_name);
    } else {
        while (stat(archive_name, &st) == 0) {
            if (strlen(archive_name) > 253) {
                error("archive name is too big");
            }

            strcat(archive_name, "_1");
        }
        if (mkdir(archive_name, 0777) == -1) {
            error("failed to create folder");
        }
        chdir(archive_name);
    }

    while ((ch = fgetc(archive)) != '\n') {
        info_size++;
    }

    info = malloc(info_size + 1);

    rewind(archive);
    fgets(info, info_size, archive);

    file_info = div_into_files(info, &number_of_files);

    for (int i = 0; i < number_of_files; i++) {
        create_file(file_info[i], archive);
    }

    free(file_info);
    free(info);
}

void create_file(file_info file_info, FILE *archive) {
    char *tmp = file_info.file_path;
    char original_dir[4096];
    int slashes = 0;

    while (*tmp != '\0') {
        if (*tmp == '/') {
            slashes++;
        }
        tmp++;
    }
    
    getcwd(original_dir, 4096);

    for (int i = 0; i < slashes; i++) {
        char *folder;
        if (i == 0) {
            folder = strtok(file_info.file_path, "/");
        } else {
            folder = strtok(NULL, "/");
        }
        if (mkdir(folder, 0777) == -1) {
            error("failed to create folder");
        }
        chdir(folder);
    }

    char *filename = strtok(NULL, "\0");

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        error("failed to create file");
    }

    chdir(original_dir);
}

file_info *div_into_files(char *str, int *elements) {
    if (str == NULL) {
        return NULL;
    }

    char *tmp = str;
    int spaces = 1;
    file_info *files;

    while(*tmp != '\0') {
        if(*tmp == ' ' && *(tmp - 1) != '\\') {
            spaces++;
        }
        tmp++;
    }

    *elements = spaces / 2;
    files = malloc(sizeof(file_info) * (*elements));

    strcpy(files[0].file_path, strtok(str, " "));
    files[0].bytes = atoi(strtok(NULL, " "));
    for (int i = 1; i < (*elements); i++) {
        strcpy(files[i].file_path, strtok(NULL, " "));
        files[i].bytes = atoi(strtok(NULL, " "));
    }

    return files;
}

void compress_to_archive() {
    FILE *info = fopen("info.txt", "wb+rb");
    FILE *archive = NULL;
    char archive_name[256];
    char directory_path[4096];
    char original_dir[4096];

    printf("Enter the archive name: ");
    scanf("%255s", archive_name);
    archive = fopen(archive_name, "wb+rb");

    if (archive == NULL) {
        error("failed to create archive");
    }

    printf("Enter the directory path: ");

    if (scanf("%4095s", directory_path) != 1) {
        error("incorrect path");
    }

    DIR *dir;
    struct dirent *entry;

    if((dir = opendir(directory_path)) == NULL) {
        error("failed to open directory");
    }
    
    getcwd(original_dir, 4096);
    chdir(directory_path);
    while((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        struct stat st;
        if (stat(entry->d_name, &st) == -1) {
            error("failed to get file information");
        }

        if(S_ISDIR(st.st_mode)) {
            add_directory_to_archive(entry->d_name, archive, info);
        } else {
            add_file_to_archive(entry->d_name, archive, info);
        }
    }
    chdir(original_dir);

    add_info_to_archive(info, archive);

    remove("info.txt");
    closedir(dir);
}

void add_file_to_archive(char *file_path, FILE *archive, FILE *info) {
    FILE *source_file = fopen(file_path, "rb");
    char *file_info = NULL;
    char ch;
    struct stat st;

// Записываем информацию файла
    stat(file_path, &st);
    file_info = malloc(strlen(file_path) + number_of_digits(st.st_size) + 3);
    sprintf(file_info, "%s %ld ", file_path, st.st_size);
    fwrite(file_info, strlen(file_info), 1, info);
    free(file_info);

// Записываем содержимое файла
    if (source_file == NULL) {
        error("failed to open the file");
    }

    while ((ch = fgetc(source_file)) != EOF)
    {
        fputc(ch, archive);
    }
    fputc('\n', archive);

    fclose(source_file);
}

void add_directory_to_archive(char *directory_path, FILE *archive, FILE *info) {
    DIR *dir;
    struct dirent *entry;

    if((dir = opendir(directory_path)) == NULL) {
        error("failed to open directory");
    }

    while((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char original_dir[4096];
        getcwd(original_dir, 4096);
        chdir(directory_path);
        
        struct stat st;
        if (stat(entry->d_name, &st) == -1) {
            error("failed to get file information");
        }

        chdir(original_dir);

        char *full_path = malloc(strlen(directory_path) + strlen(entry->d_name) + 2);
        sprintf(full_path, "%s/%s", directory_path, entry->d_name);

        if(S_ISDIR(st.st_mode)) {
            add_directory_to_archive(full_path, archive, info);
        } else {
            add_file_to_archive(full_path, archive, info);
        }

        free(full_path);
    }   
    closedir(dir);
}

void add_info_to_archive(FILE *info, FILE* archive) {
    char ch;
    char *buffer;
    int size; 

    fseek(archive, 0L, SEEK_END);
    size = ftell(archive);
    rewind(archive);

    buffer = malloc(size + 1);
    fread(buffer, 1, size, archive);
    buffer[size] = '\0';

    rewind(archive);
    rewind(info);

    while ((ch = fgetc(info)) != EOF)
    {
        fputc(ch, archive);
    }
    fputc('\n', archive);

    fputs(buffer, archive);

    free(buffer);
}

int number_of_digits(int number) {
    int count = 0;
    while (number) {
        count++;
        number /= 10;
    }
    return count;
}

void error(char *msg) {
    printf("\n\terror: <%s>\n", msg);
    exit(1);
}

char first_input() {
    printf("Press '1' to archive\n'2' to unarchive\n");
    char input = '\0';
    while(scanf("%c", &input) != 1 || (input != '1' && input != '2'));

    return input;
}