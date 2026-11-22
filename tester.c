#include "bcalc.h"

int main(int argc, char **argv) {
    String string = input_string();
    StringSeek string_seek = construct_string_seek(string);

    long double value = eval_expression(&string_seek);

    printf("value: %Lf\n", value);

    free_string(&string);

    return 0;
}