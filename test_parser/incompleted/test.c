#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void run_parser(const char *input_file, const char *output_file) {
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "./parser %s > %s", input_file, output_file);
    system(command);
}

int compare_files(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");
    if (fp1 == NULL || fp2 == NULL) {
        perror("Error opening file");
        return -1;
    }

    char line1[BUFFER_SIZE], line2[BUFFER_SIZE];
    int line_num = 1;
    int diff = 0;

    while (fgets(line1, sizeof(line1), fp1) != NULL && fgets(line2, sizeof(line2), fp2) != NULL) {
        if (strcmp(line1, line2) != 0) {
            printf("Difference at line %d:\n", line_num);
            printf("File1: %s", line1);
            printf("File2: %s", line2);
            diff = 1;
            break;
        }
        line_num++;
    }

    if (fgets(line1, sizeof(line1), fp1) != NULL || fgets(line2, sizeof(line2), fp2) != NULL) {
        printf("Files have different lengths.\n");
        diff = 1;
    }

    fclose(fp1);
    fclose(fp2);
    return diff;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> <result_file>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];
    const char *result_file = argv[3];

    run_parser(input_file, output_file);

    if (compare_files(output_file, result_file) == 0) {
        printf("The files are identical.\n");
    } else {
        printf("The files are different.\n");
    }

    return 0;
}