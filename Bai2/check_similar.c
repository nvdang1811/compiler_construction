#include <stdio.h>
#include <stdlib.h>

int areFilesIdentical(FILE *file1, FILE *file2) {
    char ch1, ch2;

    while (((ch1 = fgetc(file1)) != EOF) && ((ch2 = fgetc(file2)) != EOF)) {
        if (ch1 != ch2) {
            return 0; // Files are not identical
        }
    }

    // Check if both files have reached EOF
    if (feof(file1) && feof(file2)) {
        return 1; // Files are identical
    } else {
        return 0; // Files are not identical
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        return 1;
    }

    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");

    if (file1 == NULL || file2 == NULL) {
        perror("Error opening file");
        return 1;
    }

    if (areFilesIdentical(file1, file2)) {
        printf("Files are identical.\n");
    } else {
        printf("Files are not identical.\n");
    }

    fclose(file1);
    fclose(file2);

    return 0;
}