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
    /* BAD: has a cleanup attribute, is a pointer, missing initialization */
    __attribute__((__cleanup__(foo))) char *full_attribute;
    /* BAD: has a cleanup attribute (macrofied), is a pointer, missing initialization */
    _cleanup_foo_ char *macro_attribute_1;
    /* BAD: has a cleanup attribute (macrofied), is a pointer, missing initialization */
    _cleanup_free_ char *macro_attribute_and_fun_call;
    /* GOOD: doesn't have a cleanup attribute */
    char *simple_pointer;
    /* GOOD: doesn't have a cleanup attribute */
    char **double_pointer;
    /* GOOD: has a cleanup attribute, is not a pointer */
    _cleanup_(erase_char) char not_a_pointer;
    int r;

    /* We don't care if the variable is passed to a function, since the function
     * might not initialize it */
    r = fun(&macro_attribute_and_fun_call);
    if (r < 0)
        return 1;

    puts(full_attribute);
    puts(macro_attribute_1);
    puts(macro_attribute_and_fun_call);

    puts(simple_pointer);
    puts(*double_pointer);
    printf("%c\n", not_a_pointer);

    return 0;
}
