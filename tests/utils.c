#include <criterion/criterion.h>

#include "../src/flatex.h"

Test(utils, test_is_input) {
  char line1[] = "A normal line\n";
  cr_assert(is_input(line1) == NULL);

  char line2[] = "%\\input{path/to/file}\n";
  cr_assert(is_input(line2) == NULL);

  char line3[] = "   %\\input{path/to/file}\n";
  cr_assert(is_input(line3) == NULL);

  char line4[] = "%   \\input{path/to/file}\n";
  cr_assert(is_input(line4) == NULL);

  char line5[] = "\\input{path/to/file}\n";
  cr_assert(*(is_input(line5) - 1) == '{');

  char line6[] = "   \\input{path/to/file}\n";
  cr_assert(*(is_input(line6) - 1) == '{');

  char line7[] = "\\input{path/to/file} % Comment\n";
  cr_assert(*(is_input(line7) - 1) == '{');
}


Test(utils, test_is_input_include) {
  char line1[] = "A normal line\n";
  cr_assert(is_input(line1) == NULL);

  char line2[] = "%\\include{path/to/file}\n";
  cr_assert(is_input(line2) == NULL);

  char line3[] = "   %\\include{path/to/file}\n";
  cr_assert(is_input(line3) == NULL);

  char line4[] = "%   \\include{path/to/file}\n";
  cr_assert(is_input(line4) == NULL);

  char line5[] = "\\include{path/to/file}\n";
  cr_assert(*(is_input(line5) - 1) == '{');

  char line6[] = "   \\include{path/to/file}\n";
  cr_assert(*(is_input(line6) - 1) == '{');

  char line7[] = "\\include{path/to/file} % Comment\n";
  cr_assert(*(is_input(line7) - 1) == '{');
}

Test(utils, test_is_includepdf) {
  char line1[] = "A normal line\n";
  cr_assert(is_includepdf(line1) == NULL);

  char line2[] = "%\\includepdf{path/to/file}\n";
  cr_assert(is_includepdf(line2) == NULL);

  char line3[] = "   %\\includepdf{path/to/file}\n";
  cr_assert(is_includepdf(line3) == NULL);

  char line4[] = "%   \\includepdf{path/to/file}\n";
  cr_assert(is_includepdf(line4) == NULL);

  char line5[] = "\\includepdf{path/to/file}\n";
  cr_assert(*(is_includepdf(line5) - 1) == '{');

  char line6[] = "   \\includepdf{path/to/file}\n";
  cr_assert(*(is_includepdf(line6) - 1) == '{');

  char line7[] = "\\includepdf{path/to/file} % Comment\n";
  cr_assert(*(is_includepdf(line7) - 1) == '{');
}

Test(utils, test_is_includepdf2) {
  char line1[] = "A normal line\n";
  cr_assert(is_includepdf(line1) == NULL);

  char line2[] = "%\\includegraphics[width=0.5\\textwidth]{image}\n";
  cr_assert(is_includepdf(line2) == NULL);

  char line3[] = "   %\\includegraphics[width=0.5\\textwidth]{image}\n";
  cr_assert(is_includepdf(line3) == NULL);

  char line4[] = "%   \\includegraphics[width=0.5\\textwidth]{image}\n";
  cr_assert(is_includepdf(line4) == NULL);

  char line5[] = "\\includegraphics[width=0.5\\textwidth]{image}\n";
  cr_assert(*(is_includepdf(line5) - 1) == '{');

  char line6[] = "   \\includegraphics[width=0.5\\textwidth]{image}\n";
  cr_assert(*(is_includepdf(line6) - 1) == '{');

  char line7[] = "\\includegraphics[width=0.5\\textwidth]{image}\n % Comment\n";
  cr_assert(*(is_includepdf(line7) - 1) == '{');
}
