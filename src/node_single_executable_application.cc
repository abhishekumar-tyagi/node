#include "node_single_executable_application.h"
#if defined(__POSIX__) && !defined(_AIX) && !defined(__APPLE__)
#include <link.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#endif  // defined(__POSIX__) && !defined(_AIX) && !defined(__APPLE__)

namespace node {
namespace single_executable_application {

#define MAGIC_HEADER "NODEJSSEA"
#define VERSION_CHARS "00000001"
#define FLAG_CHARS "00000000"
#define ARGC_OFFSET                                                            \
  strlen(MAGIC_HEADER) + strlen(VERSION_CHARS) + strlen(FLAG_CHARS)
#define ARGC_LENGTH 8

#if defined(__POSIX__) && !defined(_AIX) && !defined(__APPLE__)
static int callback(struct dl_phdr_info* info, size_t size, void* data) {
  // look for the segment with the magic number
  for (int index = 0; index < info->dlpi_phnum; index++) {
    if (info->dlpi_phdr[index].p_type == PT_LOAD) {
      char* content = reinterpret_cast<char*>(info->dlpi_addr +
                                              info->dlpi_phdr[index].p_vaddr);
      if (strncmp(MAGIC_HEADER, content, strlen(MAGIC_HEADER)) == 0) {
        *(static_cast<char**>(data)) = content;
        break;
      }
    }
  }
  return 0;
}
#endif  // defined(__POSIX__) && !defined(_AIX) && !defined(__APPLE__)

struct single_executable_replacement_args* CheckForSingleBinary(int argc,
                                                                char** argv) {
  struct single_executable_replacement_args* new_args =
      new single_executable_replacement_args;
  new_args->single_executable_application = false;

  char* single_executable_data = nullptr;
#if defined(__POSIX__) && !defined(_AIX) && !defined(__APPLE__)
  dl_iterate_phdr(callback, static_cast<void*>(&single_executable_data));
#endif  // defined(__POSIX__) && !defined(_AIX) && !defined(__APPLE__)
  if (single_executable_data != nullptr) {
    // get the new arguments info
    std::string argc_string(
        static_cast<char*>(&single_executable_data[ARGC_OFFSET]), ARGC_LENGTH);
    int argument_count = std::stoi(argc_string, 0, 16);
    char* arguments = &(single_executable_data[ARGC_OFFSET + ARGC_LENGTH]);

    // set up new argc count and space for new argv
    new_args->argc = 0;
    new_args->argc = argc + argument_count;
    new_args->argv = new char*[new_args->argc];
    new_args->argv[0] = argv[0];
    int index = 1;

    // copy over the new arguments
    for (int i = 0; i < argument_count; i++) {
      new_args->argv[index++] = arguments;
      int length = strlen(arguments);
      // TODO(mhdawson): add check that we don't overrun the segment
      arguments = arguments + length + 1;
    }

    // TODO(mhdawson): remaining data after arguments in binary data
    // that can be used by the single executable applicaiton.
    // Add a way for the application to get that data.

    // copy over the arguments passed when the executable was started
    for (int i = 1; i < argc; i++) {
      new_args->argv[index++] = argv[i];
    }

    new_args->single_executable_application = true;
  }
  return new_args;
}

}  // namespace single_executable_application
}  // namespace node
