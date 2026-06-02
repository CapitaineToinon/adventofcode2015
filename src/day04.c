#include <math.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>

#define BASE "yzbqklnj"
#define BASE_LEN 8

int main() {
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  char *data = malloc(sizeof(char) * 256);
  unsigned char *digest = OPENSSL_malloc(EVP_MD_size(EVP_md5()));

  int answer = 0;
  bool part_1 = false;
  bool part_2 = false;

  while (true) {
    sprintf(data, "%s%d", BASE, answer);
    int answer_len = (answer == 0) ? 1 : log10(answer) + 1;

    EVP_DigestInit(ctx, EVP_md5());
    EVP_DigestUpdate(ctx, data, BASE_LEN + answer_len);
    unsigned int digest_len = 0;
    EVP_DigestFinal(ctx, digest, &digest_len);

    if (!part_1 &&
        (digest[0] == 0x00 && digest[1] == 0x00 && digest[2] <= 0x0F)) {
      printf("%d\n", answer);
      part_1 = true;
    }

    if (!part_2 &&
        (digest[0] == 0x00 && digest[1] == 0x00 && digest[2] == 0x00)) {
      printf("%d\n", answer);
      part_2 = true;
    }

    if (part_1 && part_2) {
      break;
    }

    answer++;
  }

  free(data);
  free(ctx);
  OPENSSL_free(digest);

  return 0;
}
