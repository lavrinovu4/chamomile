#include <stdio.h>
#include "elf.h"

int main() {

    elf_t *elf = elf_open("75320.elf");

    if(elf == NULL) {
     return 0;
    }

    unsigned char *data2 = elf_get_section(elf, ".debug_info");
    for(int i = 0; i <= 100; i++) {
        printf("%x\n", *(data2 + i));
    }

    elf_close(elf);

    return 0;
}

