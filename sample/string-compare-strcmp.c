#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* reference = "potato";

char* get_malloced_reference() {
    char* something = malloc(sizeof(char) * strlen(reference) + 1);

    strcpy(something, "potati");

    return something;
}

int main()
{
    char* something = get_malloced_reference();

    int result = strcmp(reference, something);

    printf("%d\n", result);

    free(something);
}

