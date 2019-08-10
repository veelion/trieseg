/*

*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "utf8.h"

unsigned int utf8_decode(const char *s, int *pi) {
  unsigned int c;
  int i = 0;
  /* one digit utf-8 */
  if ((s[i] & 128)== 0 ) {
    c = (unsigned int) s[i];
    i += 1;
  } else if ((s[i] & 224)== 192 ) { /* 110xxxxx & 111xxxxx == 110xxxxx */
    c = (( (unsigned int) s[i] & 31 ) << 6) +
      ( (unsigned int) s[i+1] & 63 );
    i += 2;
  } else if ((s[i] & 240)== 224 ) { /* 1110xxxx & 1111xxxx == 1110xxxx */
    c = ( ( (unsigned int) s[i] & 15 ) << 12 ) +
      ( ( (unsigned int) s[i+1] & 63 ) << 6 ) +
      ( (unsigned int) s[i+2] & 63 );
    i += 3;
  } else if ((s[i] & 248)== 240 ) { /* 11110xxx & 11111xxx == 11110xxx */
    c =  ( ( (unsigned int) s[i] & 7 ) << 18 ) +
      ( ( (unsigned int) s[i+1] & 63 ) << 12 ) +
      ( ( (unsigned int) s[i+2] & 63 ) << 6 ) +
      ( (unsigned int) s[i+3] & 63 );
    i+= 4;
  } else if ((s[i] & 252)== 248 ) { /* 111110xx & 111111xx == 111110xx */
    c = ( ( (unsigned int) s[i] & 3 ) << 24 ) +
      ( ( (unsigned int) s[i+1] & 63 ) << 18 ) +
      ( ( (unsigned int) s[i+2] & 63 ) << 12 ) +
      ( ( (unsigned int) s[i+3] & 63 ) << 6 ) +
      ( (unsigned int) s[i+4] & 63 );
    i += 5;
  } else if ((s[i] & 254)== 252 ) { /* 1111110x & 1111111x == 1111110x */
    c = ( ( (unsigned int) s[i] & 1 ) << 30 ) +
      ( ( (unsigned int) s[i+1] & 63 ) << 24 ) +
      ( ( (unsigned int) s[i+2] & 63 ) << 18 ) +
      ( ( (unsigned int) s[i+3] & 63 ) << 12 ) +
      ( ( (unsigned int) s[i+4] & 63 ) << 6 ) +
      ( (unsigned int) s[i+5] & 63 );
    i += 6;
  } else {
    c = '?';
    i++;
  }
  *pi = i;
  return c;
}
