#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dump_internal(FILE *stream, const char *type, va_list ap)
{
    if (!type || strlen(type) == 0)
        return;

    while (*type) {
        switch (*type++) {
        case 'd':
            fprintf(stream, "int: %d\n", va_arg(ap, int));
            break;
        case 'c':
            fprintf(stream, "char: %c\n", (char) va_arg(ap, int));
            break;
        case 's':
            fprintf(stream, "string: %s\n", va_arg(ap, char*));
            break;
        }
    }
}

void dump(FILE *stream, const char *type, ...)
{
    va_list ap;

    va_start(ap, type);
    dump_internal(stream, type, ap);
    va_end(ap);
}

int main(void) {

    dump(stderr, NULL);
    dump(stderr, NULL);
    dump(stdout, "ddd", 1, 2, 3);
    dump(stderr, "scs", "hello", '+', "world");
    dump(stderr, NULL);
    dump(stderr, NULL);
    dump(stderr, NULL);
    dump(stdout, "ddddddd", 1, 1, 1, 1, 1, 1, 1);
    dump(stdout, NULL);

    return 0;
}
