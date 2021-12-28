#include "boxed.hpp"
namespace SysY {
  gc_str gc_copy_str(const char *str, size_t len) {
    if (len == 0) {
      len = strlen(str);
    }
    void *mem = GC_MALLOC_ATOMIC((len + 1) * sizeof(char));
    char *ptr = (char *)mem;
    ptr[len] = 0;
    strncpy(ptr, str, len);
    return ptr;
  }
}
