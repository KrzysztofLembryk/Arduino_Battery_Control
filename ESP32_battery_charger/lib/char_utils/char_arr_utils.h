#ifndef CHAR_ARR_UTILS_H
#define CHAR_ARR_UTILS_H



/**
 * We assume each char array is null-terminated. Otherwise, undefined behavior
 * will occur.
 */

class CharArrUtils
{
public:
    static int concat_char_arr(char *dest,
                                const char *src,
                                int dest_len,
                                int src_len);

private:
}

#endif // CHAR_ARR_UTILS_H