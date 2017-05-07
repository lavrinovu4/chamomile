#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

static void *read_block(FILE *fd, unsigned int seek_addr, unsigned int read_size) {

    void *data;
    unsigned int status;

    /* Moving pointer in file */
    status = fseek(fd, seek_addr, SEEK_SET);
    if ( status != 0u) {
        return NULL;
    }

    /* Allocate memory. */
    data = malloc(read_size);
    if( data == NULL ) {
        return NULL;
    }

    /* Read data from elf file */
    status = fread(data, 1, read_size, fd);
    if(status != read_size) { /* Coudn't read all data.  */
        return NULL;
    }

    return data;
}

static int elf_read_header(FILE *fd, elf_t * elf_p) {
    unsigned int status;
    unsigned int size_header = sizeof(e_header_t);
    e_header_t *header_p = &elf_p->header;

    /* Reading header */
    status = fread(header_p, 1, size_header, fd);
    
    /* ========================================================= */
    /* Checking */
    if(status != size_header) { /* Coudn't read all data.  */
        return -1;
    }

    if( (header_p->ident[E_MAG0] != 0x7f) ||
        (header_p->ident[E_MAG1] != 'E')  ||
        (header_p->ident[E_MAG2] != 'L')  ||
        (header_p->ident[E_MAG3] != 'F')  ) {   /* It is not ELF file  */
            return -2;
    }


    if( (header_p->ident[E_CLASS] != ELFCLASS32) ||
            (header_p->ident[E_DATA] != LE) ) { /* Works now only for 32bit and little endian  */
            return -3;

    }

    return 0;
}

static int elf_read_header_section(FILE *fd, elf_t *elf_p) {

    e_header_t *header_p = &elf_p->header;
    unsigned int size_sections = (header_p->shnum + 1) * sizeof(e_section_header_t);
    
    e_section_header_t *sect_p = read_block(fd, header_p->shoff, size_sections);
    if( sect_p == NULL ) {
        return -1;
    }

    elf_p->sections = sect_p;

    return 0;
}

static int elf_prepare_names(elf_t *elf_p) {

    e_section_header_t *sect_p = elf_p->sections;
    elf_p->section_names = elf_section_get_list(elf_p);

    unsigned int numb_section = elf_get_numb_sect(elf_p);
    unsigned int *offset_names = (unsigned int *)malloc(sizeof(unsigned int) * numb_section);
    if(offset_names == NULL) { /* Coudn't allocate memory.  */
        return -1;
    }

    for(int i = 0; i < numb_section; i++) {
        *(offset_names + i) = sect_p->sh_name;
        sect_p++;
    }
    elf_p->offset_names = offset_names;

    return 0;
}

static unsigned int elf_find_ind_name(elf_t *elf_p, char *name) {
    int i;
    unsigned int result;
    unsigned int *offset_names = elf_p->offset_names;
    char *section_names = elf_p->section_names;
    unsigned int numb_section = elf_get_numb_sect(elf_p);

    for(i = 0; i < numb_section; i++) {
        result = strcmp(section_names + *(offset_names + i), name);
        if(result == 0u) {
            break;
        }
    }

    if (( i == numb_section ) && (result != 0u)) {
        return -1;
    }

    return i;
}

elf_t * elf_open(char *name_file) {

    elf_t *elf_p;
    FILE *fd;
    unsigned int status;

    elf_p = (elf_t *)malloc(sizeof(elf_t));
    if(elf_p == NULL) { /* Coudn't allocate memory.  */
        return NULL;
    }

    fd = fopen(name_file, "r");
    if(fd == NULL) {   /* Coudn't open file.  */
        return NULL;
    }

    status = elf_read_header(fd, elf_p);
    if(status != 0u) { /* Coudn't read elf header.  */
        return NULL;
    }

    status = elf_read_header_section(fd, elf_p);
    if(status != 0u) { /* Coudn't read sections header.  */
        return NULL;
    }

    elf_p->fd = fd;
    elf_prepare_names(elf_p);

    return elf_p;
}


char *elf_section_get_list(elf_t *elf_p) {
    return elf_section_get_content(elf_p, elf_p->header.shstrndx);

}

unsigned char *elf_section_get_content(elf_t *elf_p, unsigned int ind) {

    e_section_header_t *sect_p = elf_p->sections + ind;
    unsigned char *data = read_block(elf_p->fd, sect_p->sh_offset, sect_p->sh_size);

    return data;
}

unsigned char *elf_get_section(elf_t *elf_p, char *name) {

    return elf_section_get_content(elf_p, elf_find_ind_name(elf_p, name));

}

unsigned int elf_get_numb_sect(elf_t * elf_p) {
    return elf_p->header.shnum + 1;
}

void elf_close(elf_t *elf_p) {
    fclose(elf_p->fd);
    free(elf_p->sections);
    free(elf_p);
}

