#ifndef POSTJECT_API_H_
#define POSTJECT_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#elif defined(__linux__)
#include <elf.h>
#include <link.h>
#include <sys/param.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#ifndef POSTJECT_SENTINEL_FUSE
#define POSTJECT_SENTINEL_FUSE \
  "POSTJECT_SENTINEL_fce680ab2cc467b6e072b8b5df1996b2"
#endif

struct postject_options {
  const char* elf_section_name;
  const char* macho_framework_name;
  const char* macho_section_name;
  const char* macho_segment_name;
  const char* pe_resource_name;
};

inline void postject_options_init(struct postject_options* options) {
  options->elf_section_name = NULL;
  options->macho_framework_name = NULL;
  options->macho_section_name = NULL;
  options->macho_segment_name = NULL;
  options->pe_resource_name = NULL;
}

static inline bool postject_has_resource() {
  static const volatile char* sentinel = POSTJECT_SENTINEL_FUSE ":0";
  return sentinel[sizeof(POSTJECT_SENTINEL_FUSE)] == '1';
}

#if defined(__linux__)
struct postject__dl_iterate_phdr_data {
  void* data;
  size_t size;
  const char* elf_section_name;
  size_t elf_section_name_size;
};

static int postject__dl_iterate_phdr_callback(struct dl_phdr_info* info,
                                              size_t size,
                                              void* opaque) {
  struct postject__dl_iterate_phdr_data* data =
      (struct postject__dl_iterate_phdr_data*)opaque;

  // iterate program headers
  for (unsigned i = 0; i < info->dlpi_phnum; i++) {
    // skip everything but notes
    if (info->dlpi_phdr[i].p_type != PT_NOTE)
      continue;

    // note segment starts at base address + segment virtual address
    uintptr_t pos = (info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
    uintptr_t end = (pos + info->dlpi_phdr[i].p_memsz);

    // iterate through segment until we reach the end
    while (pos < end) {
      if (pos + sizeof(ElfW(Nhdr)) > end) {
        break;  // invalid
      }

      ElfW(Nhdr)* note = (ElfW(Nhdr)*)(uintptr_t)pos;
      if (note->n_namesz != 0 && note->n_descsz != 0 &&
          strncmp((char*)(pos + sizeof(ElfW(Nhdr))), data->elf_section_name,
                  data->elf_section_name_size) == 0) {
        data->size = note->n_descsz;
        // advance past note header and aligned name
        // to get to description data
        data->data = (void*)((uintptr_t)note + sizeof(ElfW(Nhdr)) +
                             roundup(note->n_namesz, 4));
        // found the note, so terminate the search by returning 1
        return 1;
      }

      pos += (sizeof(ElfW(Nhdr)) + roundup(note->n_namesz, 4) +
              roundup(note->n_descsz, 4));
    }
  }

  // wasn't able to find the note in the main executable program headers, so
  // terminate the search
  return 1;
}
#endif

static const void* postject_find_resource(
    const char* name,
    size_t* size,
    const struct postject_options* options) {
  // Always zero out the size pointer to start
  if (size != NULL) {
    *size = 0;
  }

#if defined(__APPLE__) && defined(__MACH__)
  char* section_name = NULL;
  const char* segment_name = "__POSTJECT";

  if (options != NULL && options->macho_segment_name != NULL) {
    segment_name = options->macho_segment_name;
  }

  if (options != NULL && options->macho_section_name != NULL) {
    name = options->macho_section_name;
  } else if (strncmp(name, "__", 2) != 0) {
    // Automatically prepend __ to match naming convention
    section_name = (char*)malloc(strlen(name) + 3);
    if (section_name == NULL) {
      return NULL;
    }
    strcpy(section_name, "__");
    strcat(section_name, name);
  }

  unsigned long section_size;
  char* ptr = NULL;
  if (options != NULL && options->macho_framework_name != NULL) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
    ptr = getsectdatafromFramework(options->macho_framework_name, segment_name,
                                   section_name != NULL ? section_name : name,
                                   &section_size);
  } else {
    ptr = getsectdata(segment_name, section_name != NULL ? section_name : name,
                      &section_size);
#ifdef __clang__
#pragma clang diagnostic pop
#endif

    if (ptr != NULL) {
      // Add the "virtual memory address slide" amount to ensure a valid pointer
      // in cases where the virtual memory address have been adjusted by the OS.
      //
      // NOTE - `getsectdataFromFramework` already handles this adjustment for
      //        us, which is why we only do it for `getsectdata`, see:
      //        https://web.archive.org/web/20220613234007/https://opensource.apple.com/source/cctools/cctools-590/libmacho/getsecbyname.c.auto.html
      ptr += _dyld_get_image_vmaddr_slide(0);
    }
  }

  free(section_name);

  if (size != NULL) {
    *size = (size_t)section_size;
  }

  return ptr;
#elif defined(__linux__)

  if (options != NULL && options->elf_section_name != NULL) {
    name = options->elf_section_name;
  }

  struct postject__dl_iterate_phdr_data data;
  data.data = NULL;
  data.size = 0;
  data.elf_section_name = name;
  data.elf_section_name_size = sizeof(name);
  dl_iterate_phdr(postject__dl_iterate_phdr_callback, &data);
  *size = data.size;
  return data.data;
#elif defined(_WIN32)
  void* ptr = NULL;
  char* resource_name = NULL;

  if (options != NULL && options->pe_resource_name != NULL) {
    name = options->pe_resource_name;
  } else {
    // Automatically uppercase the resource name or it won't be found
    resource_name = (char*)malloc(strlen(name) + 1);
    if (resource_name == NULL) {
      return NULL;
    }
    strcpy_s(resource_name, strlen(name) + 1, name);
    CharUpperA(resource_name);  // Uppercases inplace
  }

  HRSRC resource_handle =
      FindResourceA(NULL, resource_name != NULL ? resource_name : name,
                    MAKEINTRESOURCEA(10) /* RT_RCDATA */);

  if (resource_handle) {
    HGLOBAL global_resource_handle = LoadResource(NULL, resource_handle);

    if (global_resource_handle) {
      if (size != NULL) {
        *size = SizeofResource(NULL, resource_handle);
      }

      ptr = LockResource(global_resource_handle);
    }
  }

  free(resource_name);

  return ptr;
#else
  return NULL;
#endif
}

#endif  // POSTJECT_API_H_
