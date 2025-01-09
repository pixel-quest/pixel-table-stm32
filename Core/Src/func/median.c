#include "main.h"
#include "piramida.h"

int32_t Flt_Filtr_Median(uint32_t *a, uint32_t buf_size, uint32_t buf_median)
{
	uint32_t summ;
    uint8_t count;

    Piramida(a, buf_size);

    summ = 0;
    for (count = buf_median; count < buf_size-buf_median; count++)
        summ += a[count];

    summ /= buf_size-2*buf_median;

    return (int32_t)summ;
}
