#include <stdio.h>
#include <string.h>

int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1)
            return x;
    return 1;
}

char* affineEncrypt(char input[], int a, int k, int prime) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        if (input[i] != ' ') {
            input[i] = ((a * (input[i] - ' ')) + k) % prime;
            input[i] += ' ';
        }
    }
    return input;
}

char* affineDecrypt(char input[], int a, int k, int prime) {
    int a_inv = modInverse(a, prime);
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        if (input[i] != ' ') {
            input[i] = (a_inv * (input[i] - ' ' - k + prime)) % prime;
            input[i] += ' ';
        }
    }
    return input;
}

char *enc(char input[])
{
  int a=23;
  int k=7;
  int prime=101;
  int len = strlen(input);
  for (int i = 0; i < len; i++)
  {
    if (input[i] != ' ')
    {
      input[i] = ((a * (input[i] - ' ')) + k) % prime;
      input[i] += ' ';
    }
  }
  return input;
}

char *dec(char input[])
{
  int a=23;
  int k=7;
  int prime=101;
  int a_inv = modInverse(a, prime);
  int len = strlen(input);
  for (int i = 0; i < len; i++)
  {
    if (input[i] != ' ')
    {
      input[i] = (a_inv * (input[i] - ' ' - k + prime)) % prime;
      input[i] += ' ';
    }
  }
  return input;
}

int main() {
    char message[] = "mmm";
    printf(enc(message));

    // Decoding the message
    printf(dec(message));

    return 0;
}
