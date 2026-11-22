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

const int STRING_BUFFER = 100;

// 末尾にNULL文字を含めない
typedef struct String String;

// Stringを保持しない
typedef struct StringSeek StringSeek;

String construct_string(int size);
void free_string(String *string);
String input_string();
void output_string(FILE *stream, String string);

StringSeek construct_string_seek(String string);
char getseek(StringSeek *seek);

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

unsigned long long mypow(unsigned long long base, unsigned long long top);

//
// 実装
//

struct String {
    char *data;
    int size;
    int capacity;
};

struct StringSeek {
    char *data;
    int size;
    int seek;
};

String construct_string(int size) {
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
    int i = 0;
    while ((ch = getchar()) != '\n') {
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
    for (int i = 0; i < string.size; i++) {
        fputc(string.data[i], stream);
    }
}

StringSeek construct_string_seek(String string) {
    StringSeek string_seek = {string.data, string.size, string.size - 1};

    return string_seek;
}

char getseek(StringSeek *seek) {
    return seek->data[seek->seek];
}

long double bcalc(String string) {
    StringSeek string_seek = construct_string_seek(string);
    return eval_expression(&string_seek);
}

long double eval_expression(StringSeek *seek) {
    if(seek->seek < -1) {
        // エラー
    }
    else if(seek->seek == -1) {
        return 0;
    }
    long double value = eval_term(seek);

    if(seek->seek < -1) {
        // エラー
    }
    else if (seek->seek == -1) {
        return value;
    }
    else if (getseek(seek) == '+') {
        seek->seek--;
        return eval_expression(seek) + value;
    }
    else if (getseek(seek) == '-') {
        seek->seek--;
        return eval_expression(seek) - value;
    }
    else {
        return value;
    }
}

long double eval_term(StringSeek *seek) {
    long double value = eval_factor(seek);
    if(seek->seek < -1) {
        // エラー
    }
    if (seek->seek == -1) {
        return value;
    }

    switch (getseek(seek)) {
    case '^':
        seek->seek--;

        long double next_value = eval_factor(seek);
        return mypow(next_value, value);

    case '*':
        seek->seek--;

        return eval_term(seek) * value;

    case '/':
        seek->seek--;

        return eval_term(seek) / value;
    default:
        return value;
    }
}

long double eval_factor(StringSeek *seek) {
    if(seek->seek < 0) {
        // エラー
    }

    if (getseek(seek) == ')') {
        seek->seek--;
        long double value = eval_expression(seek);

        if (getseek(seek) != '(') {
            // エラー
        }
        seek->seek--;

        return value;
    }
    else if (isdigit((unsigned char)getseek(seek))) {
        return eval_constant(seek);
    }
    else {
        // エラー
    }
}

long double eval_constant(StringSeek *seek) {
    if (seek->seek < 0) {
        // エラー
    }

    int len;
    long double decimal_part = (long double)eval_natural_with_len(seek, &len);
    if (getseek(seek) == '.') {
        // 実数の場合
        seek->seek--;
        long double integer_part = (long double)eval_natural(seek);
        decimal_part /= mypow(10, len);

        return integer_part + decimal_part;
    }
    else {
        return decimal_part;
    }
}

unsigned long long eval_natural(StringSeek *seek) {
    int len = 0;
    return eval_natural_with_len(seek, &len);
}

unsigned long long eval_natural_with_len(StringSeek *seek, int *len) {
    unsigned long long val = 0;
    char temp;
    *len = 0;
    if(seek->seek < 0) {
        // エラー
    }
    if (!isdigit((unsigned char)getseek(seek))) {
        // エラー
    }

    while (seek->seek >= 0 && isdigit((unsigned char)(temp = getseek(seek)))) {
        val += (temp - '0') * mypow(10, *len);

        seek->seek--;
        (*len)++;
    }

    return val;
}

unsigned long long mypow(unsigned long long base, unsigned long long top) {
    if(top == 0) {
        return 1;
    }
    else if(top == 1) {
        return base;
    }
    else {
        unsigned long long tmp = mypow(base, top / 2);
        return tmp * tmp * ((top % 2 == 1) ? base : 1);
    }
}

#endif