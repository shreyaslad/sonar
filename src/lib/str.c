#include <str.h>

void itoa(long int n, char* str) {
    long int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void htoa(long int n, char* str) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    long int tmp;
    long int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
}

void utoa(long unsigned int n, char* str) {
    uint64_t i;
    i = 0;
    if (n == 0) {
        str[i++] = '0';
    }
    while (n > 0) {
        str[i++] = (n % 10) + '0';
        n /= 10;
    }

    str[i] = '\0';

    reverse(str);
}

void reverse(char* s) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int strlen(char* s) {
    int i = 0;
    while (s[i] != '\0')
        ++i;

    return i;
}

char* strcat(char* dest, char* src) {
    char* ptr = dest + strlen(dest);

    while (*src != '\0') {
        *ptr++ = *src++;
    }

    *ptr = '\0';

    return dest;
}

char* strcpy(char* dest, const char* src) {
    size_t i;

    for (i = 0; src[i]; i++) {
        dest[i] = src[i];
    }

    dest[i] = 0;

    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }

    return dest;
}

int strcmp(const char* str1, const char* str2) {
    for (size_t i = 0; str1[i] == str2[i]; i++) {
        if ((!str1[i]) && (!str2[i])) {
            return 0;
        }
    }

    return 1;
}

int strncmp(const char* str1, const char* str2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (str1[i] != str2[i]) {
            return 1;
        }
    }

    return 0;
}

char* strtok(char* str, const char* delimiter) {
    static char* buffer;

    if (str != NULL) {
        buffer = str;
    }

    if (buffer[0] == '\0') {
        return NULL;
    }

    char* ret = buffer;

    for (char* b = buffer; *b != '\0'; b++) {
        for (const char* d = delimiter; *d != '\0'; d++) {
            if (*b == *d) {
                *b = '\0';
                buffer = b + 1;

                if (b == ret) {
                    ret++;
                    continue;
                }

                return ret;
            }
        }
    }

    return ret;
}

char* strchr(const char* str, int c) {
    size_t i;

    for (i = 0; str[i]; i++) {
        if (str[i] == c) {
            return (char*)&str[i];
        }
    }

    if (!c) {
        return (char*)&str[i];
    }

    return NULL;
}

// this function has the potential to cause a buffer overflow
// use it sparingly
void append(char* s, char c) {
    int len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}
