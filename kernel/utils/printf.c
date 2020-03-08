#ifdef __cplusplus
extern "C" {
#endif
#include "stdarg.h"
#include "builddef.h"
#include "utils/printf.h"
#include "utils/string.h"

// req: total output char must be smaller than maxlen
public void _snprintf(char* buf, unsigned int maxlen, char* format,...) {
    char *traverse;
    int i;
    unsigned int len = 0; // number of chars printed
    char *s;

    va_list arg;
    va_start(arg, format);

    for (traverse = format; *traverse != '\0'; traverse++) {
        while (*traverse != '%') {
            *buf++ = *traverse;
            len++;
            if (len == maxlen) return;
            traverse++;
            if (*traverse >= '\0') return;
        }

        traverse++;

        // Module 2: Fetching and executing arguments
        switch (*traverse) {
            case 'c':
                i = va_arg(arg, int);  // Fetch char argument
                *buf++ = i;
                len++;
                if (len >= maxlen) return;
                break;
            case 'd': {
                i = va_arg(arg, int);  // Fetch Decimal/Integer argument
                if (i < 0) {
                    i = -i;
                    *buf++ = '-';
                    len++;
                    if (len >= maxlen) return;
                }

                char *s = _int_to_str_static(i, 10);
                unsigned l = _strlen(s);
                if (len + l > maxlen) {
                    l = maxlen - len;
                }
                _memcpy(buf, s, l);
                len += l;
                if (len >= maxlen) {
                    return;
                }
                buf += l;
                break;
            }
            case 'x': {
                i = va_arg(arg, unsigned int);  // Fetch Hexadecimal representation
                char *s = _int_to_str_static(i, 16);
                unsigned l = _strlen(s);
                if (len + l > maxlen) {
                    l = maxlen - len;
                }
                _memcpy(buf, s, l);
                len += l;
                if (len >= maxlen) return;
                buf += l;
                break;
            }
            case 's': {
                s = va_arg(arg, char *);  // Fetch string
                unsigned l = _strlen(s);
                if (len + l > maxlen) {
                    l = maxlen - len;
                }
                _memcpy(buf, s, l);
                len += l;
                if (len >= maxlen) return;
                buf += l;
                break;
            }
            case '\0':
                break;
        }
    }

    // Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}

#ifdef __cplusplus
}
#endif
