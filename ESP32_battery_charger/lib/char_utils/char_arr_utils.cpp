#include "char_arr_utils.h"
#include "char_utils_constants.h"
#include <Arduino.h>

int find_null_terminator_idx(const char *arr, int len);

/**
 * We assume each char array is null-terminated, and that given char arr len 
 * is in fact it's length. Otherwise, undefined behavior will occur.
 * 
 * src arr is appended to dest arr.
 */
int CharArrUtils::concat_char_arr(char dest[],
                                  const char *src1,
                                  const char *src2,
                                  int dest_size)
{
    // int dest_null_terminator_idx = find_null_terminator_idx(dest, dest_len);

    // if (dest_null_terminator_idx == ERROR_NO_NULL_TERMINATOR)
    //     return dest_null_terminator_idx;
    
    // int src_null_terminator_idx = find_null_terminator_idx(src, src_len);

    // if (src_null_terminator_idx == ERROR_NO_NULL_TERMINATOR)
    //     return src_null_terminator_idx;
    if (dest_size < strlen(src1) + 1)
    {
        Serial.println("src1 too long for dest");
        CharArrUtils::clear_arr(dest, dest_size);
        return -1;
    }

    strncpy(dest, src1, dest_size);

    if (dest_size < strlen(src1) + strlen(src2) + 1)
    {
        Serial.println("src1 + src2 too long for dest");
        CharArrUtils::clear_arr(dest, dest_size);
        return -1;
    }

    strncat(dest, src2, dest_size - strlen(dest));

    return 0;
}

void CharArrUtils::clear_arr(char *arr, int arr_size)
{
    for (int i = 0; i < arr_size; i++)
        arr[i] = '\0';
}



// ------------------HELPER FUNCTIONS------------------

int find_null_terminator_idx(const char *arr, int len)
{
    int null_terminator_idx = NULL_TERMINATOR_NOT_PRESENT;

    for (int i = 0; i < len; i++)
    {
        if (arr[i] == NULL_TERMINATOR)
        {
            null_terminator_idx = i;
            break;
        }
    }

    if (null_terminator_idx == NULL_TERMINATOR_NOT_PRESENT)
    {
        Serial.println("No null terminator");
        return ERROR_NO_NULL_TERMINATOR;
    }

    return null_terminator_idx;
}