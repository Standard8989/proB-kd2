#ifndef BCALC_H
#define BCALC_H

#include <ctype.h>
#include <limits.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// String周り
//

const size_t STRING_BUFFER = 100;

// 末尾にNULL文字を含めない
typedef struct String String;

// Stringを保持しない
typedef struct StringSeek StringSeek;

String construct_string(size_t size);
void free_string(String *string);
String input_string();
void output_string(FILE *stream, String string);

StringSeek construct_string_seek(String string);
char getseek(StringSeek *seek);

void print_syntax_error(StringSeek *seek);

//
// bcalc部分
//

long double bcalc(String string);

// expression:  式
// term:        項
// factor:      因子
// constant:    値

// 式を評価する
// 文法が正しくない場合はエラー出力しreturn
long double eval_expression(StringSeek *seek);

// 項を評価する
// ex: 2^3, 2*0.45/5, (2+3*(2+3)), 3*(2+3)
// +, -が含まれていた場合はエラー出力しreturn
long double eval_term(StringSeek *seek);

// 式をカッコで囲んだ奴か定数を評価
long double eval_factor(StringSeek *seek);

// 定数を評価する
long double eval_constant(StringSeek *seek);

// 自然数を評価
unsigned long long eval_natural(StringSeek *seek);
unsigned long long eval_natural_with_len(StringSeek *seek, int *len);

long double eval_power(StringSeek *seek);

long double mypow(long double base, unsigned long long top);

//
// 実装
//

struct String {
    char *data;
    size_t size;
    size_t capacity;
};

struct StringSeek {
    char *data;
    size_t size;
    size_t seek;
};

String construct_string(size_t size) {
    String string = {(char *)calloc(size, sizeof(char)), size, size};

    return string;
}

void free_string(String *string) {
    free(string->data);
    string->data = NULL;
    string->size = 0;
    string->capacity = 0;
}

String input_string() {
    String string = {(char *)calloc(STRING_BUFFER, sizeof(char)), 0, STRING_BUFFER};

    int ch;
    size_t i = 0;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        string.data[i++] = (char)ch;
        string.size++;

        if (i >= string.capacity) {
            string.data = (char *)realloc(string.data, string.capacity * 2);
            string.capacity *= 2;
        }
    }

    return string;
}

void output_string(FILE *stream, String string) {
    for (size_t i = 0; i < string.size; i++) {
        fputc(string.data[i], stream);
    }
}

StringSeek construct_string_seek(String string) {
    StringSeek string_seek = {string.data, string.size, 0};

    return string_seek;
}

char getseek(StringSeek *seek) {
    return seek->data[seek->seek];
}

long double bcalc(String string) {
    StringSeek string_seek = construct_string_seek(string);
    long double value = eval_expression(&string_seek);

    if (string_seek.seek != string_seek.size) {
        print_syntax_error(&string_seek);
    }

    return value;
}

long double eval_expression(StringSeek *seek) {
    long double value = eval_term(seek);

    while (true) {
        // チェック
        if (seek->seek == seek->size) {
            break;
        }
        char op = getseek(seek);

        // チェック
        if (!(op == '+' || op == '-')) {
            break;
        }
        seek->seek++;

        long double next = eval_term(seek);

        switch (op) {
        case '+':
            value += next;
            break;

        case '-':
            value -= next;
            break;

        default:
            print_syntax_error(seek);
        }
    }

    return value;
}

long double eval_term(StringSeek *seek) {
    long double value = eval_power(seek);

    while (true) {
        // チェック
        if (seek->seek == seek->size) {
            break;
        }

        char op = getseek(seek);

        // チェック
        if (!(op == '*' || op == '/')) {
            break;
        }
        seek->seek++;

        long double next = eval_power(seek);

        switch (op) {
        case '*':
            value *= next;
            break;

        case '/':
            if (next == 0) {
                fprintf(stdout, "Zero division occurs.\n");
                exit(1);
            }
            value /= next;
            break;

        default:
            print_syntax_error(seek);
        }
    }

    return value;
}

long double eval_factor(StringSeek *seek) {
    if (seek->seek == seek->size) {
        print_syntax_error(seek);
    }

    if (getseek(seek) == '(') {
        seek->seek++;
        long double value = eval_expression(seek);

        if (seek->seek == seek->size || getseek(seek) != ')') {
            print_syntax_error(seek);
        }
        seek->seek++;

        return value;
    }
    else if (isdigit((unsigned char)getseek(seek))) {
        return eval_constant(seek);
    }
    else {
        print_syntax_error(seek);
    }
}

long double eval_constant(StringSeek *seek) {
    if (seek->seek == seek->size) {
        print_syntax_error(seek);
    }

    int len;
    StringSeek copy = *seek;
    unsigned long long integer_part = eval_natural_with_len(seek, &len);
    if(integer_part / (unsigned long long)mypow(10, len - 1) == 0 && (len != 1 || integer_part != 0)) {
        print_syntax_error(&copy);
    }
    if (seek->seek == seek->size) {
        return integer_part;
    }

    if (getseek(seek) == '.') {
        // 実数の場合
        seek->seek++;
        long double decimal_part = (long double)eval_natural_with_len(seek, &len);
        decimal_part /= mypow(10, len);

        return integer_part + decimal_part;
    }
    else {
        return (long double)integer_part;
    }
}

unsigned long long eval_natural(StringSeek *seek) {
    int len;
    return eval_natural_with_len(seek, &len);
}

unsigned long long eval_natural_with_len(StringSeek *seek, int *len) {
    unsigned long long val = 0;
    char temp;
    *len = 0;
    if (seek->seek == seek->size || !isdigit((unsigned char)getseek(seek))) {
        print_syntax_error(seek);
    }

    while (seek->seek < seek->size && isdigit((unsigned char)(temp = getseek(seek)))) {
        val *= 10;
        val += temp - '0';

        seek->seek++;
        (*len)++;
    }

    return val;
}

long double eval_power(StringSeek *seek) {
    long double value = eval_factor(seek);
    if (seek->seek == seek->size) {
        return value;
    }

    if (getseek(seek) == '^') {
        seek->seek++;
        return mypow(value, eval_factor(seek));
    }
    else {
        return value;
    }
}

long double mypow(long double base, unsigned long long top) {
    if (top == 0) {
        return 1;
    }
    else if (top == 1) {
        return base;
    }
    else {
        long double tmp = mypow(base, top / 2);
        return tmp * tmp * ((top % 2 == 1) ? base : 1);
    }
}

void print_syntax_error(StringSeek *seek) {
    long n = seek->seek + 1;
    char *str = "th";

    if (n % 100 < 11 || n % 100 > 13) {
        switch (n % 10) {
        case 1:
            str = "st";
            break;
        case 2:
            str = "nd";
            break;
        case 3:
            str = "rd";
            break;
        default:
            break;
        }
    }

    fprintf(stdout, "Syntax error occurs at the %ld%s character.\n", n, str);
    exit(1);
}

#endif