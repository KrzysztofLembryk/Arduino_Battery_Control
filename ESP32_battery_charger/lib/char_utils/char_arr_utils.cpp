#include "char_arr_utils.h"
#include "constants.h"
#include <Arduino.h>

int CharArrUtils::concat_char_arr(char *dest,
                                   const char *src,
                                   int dest_len,
                                   int src_len)
{
   int null_terminator_idx = NULL_TERMINATOR_NOT_PRESENT;

   for (int i = 0; i < dest_len; i++) 
   {
        if (dest[i] == NULL_TERMINATOR)
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
}