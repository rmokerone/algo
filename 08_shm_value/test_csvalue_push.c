#include "csvalue.h"

#define CSVALUE_KEY_ID (10077)

typedef struct 
{
    int inc_count;
    int dec_count;
} st_user_data;


int main(void)
{
    st_user_data value;
    value.inc_count = 1234;
    value.dec_count = 47007;

    st_csvalue* context = csvalue_open(CSVALUE_KEY_ID, sizeof(st_user_data));

    csvalue_push(context, &value);

    sleep(2);

    csvalue_close(context);
}