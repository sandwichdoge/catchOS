#ifndef INCLUDE_PRINTF_H
#define INCLUDE_PRINTF_H
#ifdef __cplusplus
extern "C" {
#endif

void _snprintf(char* buf, unsigned int maxlen, char* format,...);

#ifdef __cplusplus
}
#endif
#endif // INCLUDE_PRINTF_H