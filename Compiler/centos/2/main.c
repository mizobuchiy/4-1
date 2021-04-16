#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char const* argv[]) {
  static unsigned char machine_code[] = {0xb8, 0x01, 0x00, 0x00, 0x00, 0xc3};
  char* buffer = mmap(0, 4096, PROT_EXEC | PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memcpy(buffer, machine_code, sizeof(machine_code));
  union {
    char* bytes;
    int (*func)(void);
  } converter;
  converter.bytes = buffer;
  printf("%x\n", converter.func());
  return 0;
}
