#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* reference = "potato";

char* get_malloced_reference() {
    char* something = malloc(sizeof(char) * strlen(reference) + 1);

    strcpy(something, "potato");

    return something;
}

int strcmp_forloop(char *a, char *b) {
    int offset = 0;
    size_t reference_size = strlen(a);
    size_t content_size = strlen(b);
    size_t max = reference_size > content_size ? reference_size : content_size;

    while (offset < max && a[offset] == b[offset]) {
        offset++;
    }
    
    if (offset < max) {
    	return a[offset] - b[offset];
    }

    return 0;
}

int main()
{
    char* something = get_malloced_reference();

    int result = strcmp_forloop(reference, something);

    printf("%d\n", result);

    free(something);
}


