#define NULL (void*)0
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
    __attribute__((__cleanup__(foo))) char *b_full_attribute;
    /* BAD: has a cleanup attribute (macrofied), is a pointer, missing initialization */
    _cleanup_foo_ char *b_macro_attribute_1;
    /* BAD: has a cleanup attribute (macrofied), is a pointer, missing initialization */
    _cleanup_free_ char *b_macro_attribute_and_fun_call;
    /* GOOD: doesn't have a cleanup attribute */
    char *g_simple_pointer;
    /* GOOD: doesn't have a cleanup attribute */
    char **g_double_pointer;
    /* GOOD: has a cleanup attribute, is not a pointer */
    _cleanup_(erase_char) char g_not_a_pointer;
    /* GOOD: has a cleanup attribute, is a pointer, is initialized */
    _cleanup_free_ char *g_macro_attribute_initialized = NULL;
    int r;

    /* We don't care if the variable is passed to a function, since the function
     * might not initialize it */
    r = fun(&b_macro_attribute_and_fun_call);
    if (r < 0)
        return 1;

    puts(b_full_attribute);
    puts(b_macro_attribute_1);
    puts(b_macro_attribute_and_fun_call);

    puts(g_simple_pointer);
    puts(*g_double_pointer);
    printf("%c\n", g_not_a_pointer);
    puts(g_macro_attribute_initialized);

    return 0;
}
