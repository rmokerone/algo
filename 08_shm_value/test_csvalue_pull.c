#include "csvalue.h"
#include <stdio.h>
#include <unistd.h>

#define CSVALUE_KEY_ID (10077)

typedef struct 
{
    int inc_count;
    int dec_count;
} st_user_data;


int main(void)
{
    st_user_data value;

    st_csvalue* context = csvalue_open(CSVALUE_KEY_ID, sizeof(st_user_data));

    csvalue_pull(context, &value);
    printf("inc_count=%d\n", value.inc_count);
    printf("dec_count=%d\n", value.dec_count);

    sleep(2);

    csvalue_close(context);
}