
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Cin {
  char* (*NextToken)();
  int (*nextint)();
  unsigned int (*nextuint)();
  long (*nextlong)();
  unsigned long (*nextulong)();
  double (*nextdouble)(int acceptAnyDecimalSeparator);
  long double (*nextldouble)(int acceptAnyDecimalSeparator);
  float (*nextfloat)(int acceptAnyDecimalSeparator);
  char* (*nextstring)();
  char (*nextchar)();
};


char* NextToken();
int nextint();
unsigned int nextuint();
long nextlong();
unsigned long nextulong();
double nextdouble(int acceptAnyDecimalSeparator);
long double nextldouble(int acceptAnyDecimalSeparator);
float nextfloat(int acceptAnyDecimalSeparator);
char* nextstring();
char nextchar();

struct Cin cin = {.NextToken = NextToken,
                  .nextint = nextint,
                  .nextuint = nextuint,
                  .nextlong = nextlong,
                  .nextulong = nextulong,
                  .nextdouble = nextdouble,
                  .nextldouble = nextldouble,
                  .nextfloat = nextfloat,
                  .nextstring = nextstring,
                  .nextchar = nextchar};

char* NextToken() {
  char* tokenChars = (char*)malloc(1);
  tokenChars[0] = '\0';
  int tokenSize = 1;
  int tokenFinished = 0;
  int skipWhiteSpaceMode = 1;

  while (!tokenFinished) {
    int nextChar = getchar();

    if (nextChar == EOF) {
      tokenFinished = 1;
    } else {
      char ch = (char)nextChar;

      if (isspace(ch)) {
        if (!skipWhiteSpaceMode) {
          tokenFinished = 1;

          if (ch == '\r' && (strcmp("\r\n", getenv("OS")) == 0)) {
            getchar();  
          }
        }
      } else {
        skipWhiteSpaceMode = 0;
        tokenChars = realloc(tokenChars, tokenSize + 1);
        tokenChars[tokenSize - 1] = ch;
        tokenChars[tokenSize] = '\0';
        tokenSize++;
      }
    }
  }

  return tokenChars;
}

int nextint() {
  char* token = NextToken();
  int result = atoi(token);
  free(token);
  return result;
}

unsigned int nextuint() {
  char* token = NextToken();
  unsigned int result = strtoul(token, NULL, 10);
  free(token);
  return result;
}

long nextlong() {
  char* token = NextToken();
  long result = strtol(token, NULL, 10);
  free(token);
  return result;
}

unsigned long nextulong() {
  char* token = NextToken();
  unsigned long result = strtoul(token, NULL, 10);
  free(token);
  return result;
}

double nextdouble(int acceptAnyDecimalSeparator) {
  char* token = NextToken();

  if (acceptAnyDecimalSeparator) {
    for (int i = 0; token[i] != '\0'; i++) {
      if (token[i] == ',') {
        token[i] = '.'; 
      }
    }

    double result = atof(token);
    free(token);
    return result;
  } else {
    double result = atof(token);
    free(token);
    return result;
  }
}

long double nextldouble(int acceptAnyDecimalSeparator) {
  char* token = NextToken();

  if (acceptAnyDecimalSeparator) {
    for (int i = 0; token[i] != '\0'; i++) {
      if (token[i] == ',') {
        token[i] = '.'; 
      }
    }

    long double result = strtold(token, NULL);
    free(token);
    return result;
  } else {
    long double result = strtold(token, NULL);
    free(token);
    return result;
  }
}

float nextfloat(int acceptAnyDecimalSeparator) {
  char* token = NextToken();

  if (acceptAnyDecimalSeparator) {
    for (int i = 0; token[i] != '\0'; i++) {
      if (token[i] == ',') {
        token[i] = '.';  
      }
    }

    float result = strtof(token, NULL);
    free(token);
    return result;
  } else {
    float result = strtof(token, NULL);
    free(token);
    return result;
  }
}
