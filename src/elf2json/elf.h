
#ifndef _ELF_H_
#define _ELF_H_

#define E_MAG0 0
#define E_MAG1 1
#define E_MAG2 2
#define E_MAG3 3
#define E_CLASS 4
#define E_DATA 5

#define ELFCLASS32 1
#define LE 1

typedef struct { 
    unsigned char ident[16];
    unsigned short type;
    unsigned short mashine;
    unsigned int version;
    unsigned int entry;
    unsigned int phoff;
    unsigned int shoff;
    unsigned int flags;
    unsigned short ehsize;
    unsigned short phentsize;
    unsigned short phnum;
    unsigned short shentsize;
    unsigned short shnum;
    unsigned short shstrndx;
} e_header_t;


typedef struct {
    unsigned int sh_name;
    unsigned int sh_type;
    unsigned int sh_flags;
    unsigned int sh_addr;
    unsigned int sh_offset;
    unsigned int sh_size;
    unsigned int sh_link;
    unsigned int sh_info;
    unsigned int sh_addralign;
    unsigned int sh_entsize;
} e_section_header_t;


typedef struct {
    e_header_t header;
    e_section_header_t *sections;
    char *section_names;
    unsigned int *offset_names;
    FILE *fd;
} elf_t;


elf_t * elf_open(char *name_file);
char *elf_section_get_list(elf_t *elf_p);
unsigned int elf_get_numb_sect(elf_t * elf_p);
unsigned char *elf_get_section(elf_t *elf_p, char *name);
unsigned char *elf_section_get_content(elf_t *elf_p, unsigned int ind);
void elf_close(elf_t *elf_p);

#endif /*!_ELF_H_*/

