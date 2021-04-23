#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char const* argv[]) {
  /*
   *  int result = 0;
   *  for (int i = 0; i < 10; i++) {
   *    result += i;
   *  }
   *  return result;
   */
  static unsigned char machine_code[] = {//  MOV EAX, 0;
                                         0xb8, 0x00, 0x00, 0x00, 0x00,
                                         //  MOV ECX, 0;
                                         0xb9, 0x00, 0x00, 0x00, 0x00,
                                         // LOOP:
                                         //  CMP ECX, 10;
                                         0x83, 0xf9, 0x0a,
                                         //  JGE EXIT;
                                         0x7d, 0x07,
                                         //  ADD EAX, ECX;
                                         0x03, 0xc1,
                                         //  ADD ECX, 2;
                                         0x83, 0xc1, 0x01,
                                         //  JMP LOOP:
                                         0xeb, 0xf4,
                                         // EXIT:
                                         //  RET;
                                         0xc3};

  char* buffer = mmap(0, 4096, PROT_EXEC | PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memcpy(buffer, machine_code, sizeof(machine_code));
  union {
    char* bytes;
    int (*func)(void);
  } converter;
  converter.bytes = buffer;
  printf("%d\n", converter.func());
  return 0;
}
