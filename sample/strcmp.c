#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* reference = "potato";

int with_strcmp(char *string) {
    return strcmp(reference, string);
}

int with_forloop(char *string) {
    int offset = 0;
    size_t reference_size = strlen(reference);
    size_t content_size = strlen(string);

    while (offset < reference_size && offset < content_size && string[offset] == reference[offset]) {
        offset++;
    }

    return offset < strlen(reference);
}

char* generate_something() {
    char* something = malloc(sizeof(char) * strlen(reference) + 1);

    strcpy(something, "potati");

    return something;
}

int main()
{
    char* something = generate_something();

    int result1 = with_strcmp(something);
    int result2 = with_forloop(something);

    printf("%d vs %d", result1, result2);

    free(something);
}

