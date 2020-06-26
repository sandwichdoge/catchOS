#include "utils/debug.h"
#include "builddef.h"

public void _dbg_log(char *format, ...) {
#ifdef TARGET_HOST
    printf(format);
#else
    char *traverse;
    int i;
    unsigned int u;
    char *s;

    va_list arg;
    va_start(arg, format);

    for (traverse = format; *traverse != '\0'; traverse++) {
        while (*traverse != '%') {
            serial_write(SERIAL_COM1_BASE, traverse, 1);
            traverse++;
            if (*traverse == '\0') {
                return;
            }
        }

        traverse++;

        // Module 2: Fetching and executing arguments
        switch (*traverse) {
            case 'c':
                i = va_arg(arg, int);  // Fetch char argument
                serial_write(SERIAL_COM1_BASE, (char*)&i, 1);
                break;
            case 'u':
                u = va_arg(arg, unsigned int);
                s = _int_to_str_static(u, 10);
                serial_write(SERIAL_COM1_BASE, s, _strlen(s));
                break;
            case 'd':
                i = va_arg(arg, int);  // Fetch Decimal/Integer argument
                if (i < 0) {
                    i = -i;
                    serial_write(SERIAL_COM1_BASE, "-", 1);
                }
                s = _int_to_str_static(i, 10);
                serial_write(SERIAL_COM1_BASE, s, _strlen(s));
                break;
            case 'o':
                i = va_arg(arg, unsigned int);  // Fetch Octal representation
                s = _int_to_str_static(i, 8);
                serial_write(SERIAL_COM1_BASE, s, _strlen(s));
                break;
            case 's':
                s = va_arg(arg, char *);  // Fetch string
                serial_write(SERIAL_COM1_BASE, s, _strlen(s));
                break;
            case 'x':
                i = va_arg(arg, unsigned int);  // Fetch Hexadecimal representation
                s = _int_to_str_static(i, 16);
                serial_write(SERIAL_COM1_BASE, s, _strlen(s));
                break;
            case '\0':
                break;
        }
    }

    // Module 3: Closing argument list to necessary clean-up
    va_end(arg);
#endif
}
