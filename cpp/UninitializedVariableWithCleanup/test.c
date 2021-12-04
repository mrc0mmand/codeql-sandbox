#define _cleanup_foo_ __attribute__((__cleanup__(foo)))
#define _cleanup_(x) __attribute__((__cleanup__(x)))

static inline void freep(void *p) {
        *(void**)p = mfree(*(void**) p);
}

#define _cleanup_free_ _cleanup_(freep)

static inline void foo(char **p) {
    if (*p)
        *p = free(*p);
}

int main(void) {
    __attribute__((__cleanup__(foo))) char *a;
    char *b;
    _cleanup_foo_ char *c;
    char **d;
    _cleanup_free_ char *e;
    int r;

    r = fun(&e);
    if (r < 0)
        return 1;

    puts(a);
    puts(b);
    puts(c);
    puts(*d);
    puts(e);

    return 0;
}
