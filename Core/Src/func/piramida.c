#include "main.h"

void swap (uint32_t *a, uint32_t *b)
{
  uint32_t t = *a;
  *a = *b;
  *b = t;
}

void Piramida(uint32_t *a, uint32_t buf_size)
{
  uint32_t i, b, sh=0, n=buf_size;
  while (1)
    {
        b = 0;
        for (i = 0; i < n; ++i)
        {
          if (i*2 + 2 + sh < n)
          {
            if ((a[i+sh] > a[i*2 + 1 + sh]) || (a[i + sh] > a[i*2 + 2 + sh]))
            {
              if (a[i*2+1+sh] < a[i*2+2+sh])
              {
                swap (&a[i+sh], &a[i*2+1+sh]);
                b = 1;
              }
              else if (a[i*2+2+sh] < a[i*2+1+sh])
              {
                swap (&a[i+sh],&a[i*2+2+sh]);
                b = 1;
              }
            }
          }
          else if (i * 2 + 1 + sh < n)
          {
            if (a[i+sh] > a[i*2+1+sh])
            {
              swap (&a[i+sh], &a[i*2+1+sh]);
              b=1;
            }
          }
        }
        if (!b) sh++;
        if (sh+2==n)
          break;
    }
}
