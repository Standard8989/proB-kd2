#include "bcalc.h"

// StringSeek construct_string_seek();
// String construct_string_from_char(char *string);

// bool test_eval_natural(StringSeek *test_input, unsigned long long right_value, int test_num) {

// }

int main(int argc, char **argv) {
    String string = input_string();
    StringSeek string_seek = construct_string_seek(string);

    long double value = eval_constant(&string_seek);

    printf("value: %Lf\n", value);

    free_string(&string);

    return 0;
}