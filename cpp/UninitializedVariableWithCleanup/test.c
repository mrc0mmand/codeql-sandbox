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

static inline void erase_char(char *p) {
    *p = '\0';
}

int main(void) {
    __attribute__((__cleanup__(foo))) char *full_attribute;
    char *simple_pointer;
    _cleanup_foo_ char *macro_attribute_1;
    char **double_pointer;
    _cleanup_free_ char *macro_attribute_and_fun_call;
    _cleanup_(erase_char) char not_a_pointer;
    int r;

    r = fun(&macro_attribute_and_fun_call);
    if (r < 0)
        return 1;

    puts(full_attribute);
    puts(simple_pointer);
    puts(macro_attribute_1);
    puts(*double_pointer);
    puts(macro_attribute_and_fun_call);
    printf("%c\n", not_a_pointer);

    return 0;
}
