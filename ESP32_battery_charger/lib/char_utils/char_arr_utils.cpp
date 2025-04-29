#include "char_arr_utils.h"
#include "../../include/error_constants.h"
#include <Arduino.h>

/**
 * We assume each src char array is null-terminated, and given char arr len 
 * is in fact it's length. Otherwise, undefined behavior will occur.
 * --> src arr is appended to dest arr.
 * 
 * Returns:
 * - ERROR_DEST_SIZE_TO_SMALL - when src1 + src2 too long for char dest[]
 * - SUCCESS - otherwise
 */
int CharArrUtils::concat_char_arr(char dest[],
                                  const char *src1,
                                  const char *src2,
                                  int dest_size)
{
    if (dest_size < strlen(src1) + 1)
    {
        Serial.println("src1 too long for dest");
        CharArrUtils::clear_arr(dest, dest_size);

        return ERROR_DEST_SIZE_TO_SMALL;
    }

    strncpy(dest, src1, dest_size);

    if (dest_size < strlen(src1) + strlen(src2) + 1)
    {
        Serial.println("src1 + src2 too long for dest");
        CharArrUtils::clear_arr(dest, dest_size);

        return ERROR_DEST_SIZE_TO_SMALL;
    }

    strncat(dest, src2, dest_size - strlen(dest));

    return SUCCESS;
}

void CharArrUtils::clear_arr(char *arr, int arr_size)
{
    for (int i = 0; i < arr_size; i++)
        arr[i] = '\0';
}