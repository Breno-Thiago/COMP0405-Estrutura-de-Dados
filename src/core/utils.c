#include "utils.h"
#include <stdlib.h>
#include <string.h>

char* utl_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copia = (char*)malloc(len);
    if (copia) {
        memcpy(copia, s, len);
    }
    return copia;
}

void utl_chomp(char* s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

int utl_str_not_empty(const char* s) {
    return (s && s[0] != '\0');
}
