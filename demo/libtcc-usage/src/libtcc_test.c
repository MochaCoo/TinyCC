/*
 * Simple Test program for libtcc
 *
 * libtcc can be useful to use tcc as a "backend" for a code generator.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libtcc.h"

static void recv_tcc_error(void* opaque, const char* msg) {
  printf(opaque, msg);
}

/* this function is called by the generated code */
int add(int a, int b) {
  return a + b;
}

/* this strinc is referenced by the generated code */
const char hello[] = "Hello World!";

// clang-format off
char first_c_source_code[] =
"#include <tcclib.h>\n" /* include the "Simple libc header for TCC" */
"extern int add(int a, int b);\n"
"#ifdef _WIN32\n" /* dynamically linked data needs 'dllimport' */
" __attribute__((dllimport))\n"
"#endif\n"
"extern const char hello[];\n"

"extern void func(const char*);\n"
"extern const char func_hello[];\n"

"int fib(int n)\n"
"{\n"
"    if (n <= 2)\n"
"        return 1;\n"
"    else\n"
"        return fib(n-1) + fib(n-2);\n"
"}\n"
"\n"
"int foo(int n)\n"
"{\n"
"    func(func_hello);"
"    printf(\"%s\\n\", hello);\n"
"    printf(\"fib(%d) = %d\\n\", n, fib(n));\n"
"    printf(\"add(%d, %d) = %d\\n\", n, 2 * n, add(n, 2 * n));\n"
"    return 0;\n"
"}\n";

char second_c_source_code[] =
"#include <tcclib.h>\n"
"const char func_hello[] = \"hello from func\\n\";\n"
"void func(const char* p)\n"
"{\n"
"    printf(p);\n"
"    return;"
"}\n";
// clang-format on

int main(int argc, char** argv) {
  TCCState* s;
  int i;
  int (*func)(int);

  s = tcc_new();
  if (!s) {
    fprintf(stderr, "Could not create tcc state\n");
    exit(1);
  }

  /* if tcclib.h and libtcc1.a are not installed, where can we find them */
  for (i = 1; i < argc; ++i) {
    char* a = argv[i];
    if (a[0] == '-') {
      if (a[1] == 'B')
        tcc_set_lib_path(s, a + 2);
      else if (a[1] == 'I')
        tcc_add_include_path(s, a + 2);
      else if (a[1] == 'L')
        tcc_add_library_path(s, a + 2);
    }
  }

  /* MUST BE CALLED before any compilation */
  tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

  tcc_set_error_func(s, "error 1: %s", recv_tcc_error);
  if (tcc_compile_string(s, first_c_source_code) == -1)
    return 1;

  tcc_set_error_func(s, "error 2: %s", recv_tcc_error);
  if (tcc_compile_string(s, second_c_source_code) == -1)
    return 1;

  tcc_set_error_func(s, NULL, NULL);

  /* as a test, we add symbols that the compiled program can use.
     You may also open a dll with tcc_add_dll() and use symbols from that */
  tcc_add_symbol(s, "add", add);
  tcc_add_symbol(s, "hello", hello);

  /* relocate the code */
  if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
    return 1;

  /* get entry symbol */
  func = tcc_get_symbol(s, "foo");
  if (!func)
    return 1;

  /* run the code */
  func(32);

  /* delete the state */
  tcc_delete(s);

  return 0;
}
