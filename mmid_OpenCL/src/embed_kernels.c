#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

const char *FILE_EXTENSION = ".cl";
#ifdef _WIN32
const char SEPARATOR = '\\';
#else
const char SEPARATOR = '/';
#endif

FILE *open_or_exit(const char *fname, const char *mode) {
    FILE *f = fopen(fname, mode);
    if (f == NULL) {
        perror(fname);
        exit(EXIT_FAILURE);
    }
    return f;
}

void read_to_file(FILE *toFile, char *path, char *file) {
    char variable[strlen(file) - 2], fromFile[strlen(path) + strlen(file) + 2];
    strncpy(variable, file, strlen(file) - 3);
    variable[strlen(file) - 3] = '\0';
    strcpy(fromFile, path);
    if (path[strlen(path) - 1] != SEPARATOR)
        strcat(file, &SEPARATOR);
    strcat(fromFile, file);
    FILE *in = open_or_exit(fromFile, "r");
    fprintf(toFile, "const char %s[] = \"", variable);
    int prevC = EOF;
    for (int c = getc(in); c != EOF; c = getc(in)) {
        if (c == '\r')
            continue;
        if (prevC == '\n' && c != '\n')
            fprintf(toFile, "\n\"");
        if (c == '\n') {
            if (prevC == '\n')
                continue;
            fprintf(toFile, "\\n\"");
        } else if (c == '\"')
            fprintf(toFile, "\\\"");
        else if (c == '\\')
            fprintf(toFile, "\\\\");
        else
            fprintf(toFile, "%c", c);
        prevC = c;
    }
    if (prevC != '\"' && prevC != '\n')
        fprintf(toFile, "\"");
    fprintf(toFile, ";\nconst size_t %s_len = sizeof(%s);\n\n", variable, variable);
    fclose(in);
}

int main(int argc, char **argv) {
    if (argc < 3)
        return EXIT_FAILURE;
    DIR *dir = opendir(argv[1]);
    FILE *out = open_or_exit(argv[2], "w");
    if (dir == NULL || out == NULL)
        return EXIT_FAILURE;
    fprintf(out, "#include \"kernels.h\"\n\n");
    for (struct dirent *ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        if (strcmp(FILE_EXTENSION, ent->d_name + strlen(ent->d_name) - 3) != 0)
            continue;
        read_to_file(out, argv[1], ent->d_name);
    }
    return closedir(dir) + fclose(out);
}