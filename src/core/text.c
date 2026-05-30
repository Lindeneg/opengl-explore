#include "text.h"

#include <string.h>

#include "../common.h"

char *text_next_token(char **cursor) {
    char *s = *cursor;
    while (*s == ' ' || *s == '\t' || *s == '\r')
        ++s;
    if (*s == '\0') {
        *cursor = s;
        return NULL;
    }
    char *start = s;
    while (*s && *s != ' ' && *s != '\t' && *s != '\r')
        ++s;
    if (*s)
        *s++ = '\0';
    *cursor = s;
    return start;
}

char *text_trim(char *s) {
    while (*s == ' ' || *s == '\t' || *s == '"' || *s == '\r')
        ++s;
    u64 n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '"' || s[n - 1] == '\r'))
        s[--n] = '\0';
    return s;
}
