#ifndef CHAR_ARR_UTILS_H
#define CHAR_ARR_UTILS_H



/**
 * We assume each char array is null-terminated. Otherwise, undefined behavior
 * will occur.
 */
class CharArrUtils
{
public:
    static int concat_char_arr(char dest[],
                                const char *src,
                                const char *src2,
                                int dest_size);

    static void clear_arr(char *arr, int arr_size);
private:
    
};

#endif // CHAR_ARR_UTILS_H