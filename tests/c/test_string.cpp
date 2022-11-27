#include <string.h>

#include <catch2/catch.hpp>

#include "../../c/Gstring.h"

const char *test_str = "Test!";
const char *append_str = " Hello again from the test!";
size_t test_len = strlen(test_str);
size_t append_len = strlen(append_str);

static_assert(GSTRING_SMALL_SIZE == 7,
              "GSTRING_SMALL_SIZE different from expected");

const char *barely_fit_str = "123456";
const char *barely_not_fit_str = "1234567";
size_t barely_fit_len = strlen(barely_fit_str);
size_t barely_not_fit_len = strlen(barely_not_fit_str);


TEST_CASE("Resize from small", "[Gstring]") {
  Gstring str = Gstring_create_len(test_str, test_len);

  // Small array when fits
  REQUIRE(Gstring_smallp(&str));
  REQUIRE(str.len == test_len);
  REQUIRE(str.size == 0);

  // Heap allocation
  Gstring_append_len(&str, append_str, append_len);

  REQUIRE_FALSE(Gstring_smallp(&str));
  REQUIRE(str.size == test_len + append_len + 1);
  REQUIRE(str.len == test_len + append_len);

  // Double at second heap allocation
  size_t before_size = str.size;
  size_t before_len = str.len;

  Gstring_append_len(&str, append_str, append_len);
  REQUIRE(str.size == before_size * 2);
  REQUIRE(str.len == before_len + append_len);

  Gstring_destroy(&str);
}


TEST_CASE("Resize from !small", "[Gstring]") {
  Gstring str = Gstring_create_len(barely_not_fit_str, barely_not_fit_len);

  // Big when string doesn't fit
  REQUIRE_FALSE(Gstring_smallp(&str));
  REQUIRE(str.len == barely_not_fit_len);
  REQUIRE(str.size == barely_not_fit_len + 1);

  // Heap allocation to fit if over double increase
  Gstring_append_len(&str, append_str, append_len);

  REQUIRE_FALSE(Gstring_smallp(&str));
  REQUIRE(str.size == barely_not_fit_len + append_len + 1);
  REQUIRE(str.len == barely_not_fit_len + append_len);

  // Double at the next heap allocation
  size_t before_size = str.size;
  size_t before_len = str.len;

  Gstring_append_len(&str, append_str, append_len);
  REQUIRE(str.size == before_size * 2);
  REQUIRE(str.len == before_len + append_len);

  Gstring_destroy(&str);
}


TEST_CASE("smallp", "[Gstring]") {
  Gstring s = Gstring_create_len(test_str, test_len);
  Gstring small = Gstring_create_len(barely_fit_str, barely_fit_len);
  Gstring not_small = Gstring_create_len(barely_not_fit_str, barely_not_fit_len);

  REQUIRE(Gstring_smallp(&s));
  REQUIRE(Gstring_smallp(&small));
  REQUIRE_FALSE(Gstring_smallp(&not_small));

  Gstring_append_len(&s, append_str, append_len);
  REQUIRE_FALSE(Gstring_smallp(&s));

  Gstring_destroy(&s);
  Gstring_destroy(&small);
  Gstring_destroy(&not_small);
}


TEST_CASE("String content", "[Gstring]") {
  const char *first = "123";
  const char *first2 = "4 6";
  const char *second = " 890";
  const char *third = "ABCD F";

  Gstring s = Gstring_create_len(first, 3);
  CHECK(strcmp(Gstring_get_str(&s), "123") == 0);
  INFO(Gstring_get_str(&s));

  // stay small
  Gstring_append_len(&s, first2, 3);
  CHECK(strcmp(Gstring_get_str(&s), "1234 6") == 0);
  INFO(Gstring_get_str(&s));

  Gstring_append_len(&s, second, 4);
  CHECK(strcmp(Gstring_get_str(&s), "1234 6 890") == 0);
  INFO(Gstring_get_str(&s));

  Gstring_append_len(&s, third, 4);
  CHECK(strcmp(Gstring_get_str(&s), "1234 6 890ABCD") == 0);
  INFO(Gstring_get_str(&s));

  Gstring_destroy(&s);
}


TEST_CASE("Copying", "[Gstring]") {
  Gstring gb;
  Gstring gs;
  {
    const char *test_str = "Herr gott mine lord!";
    Gstring s = Gstring_create_len(test_str, strlen(test_str));
    gb = s;
    gs = Gstring_create_len("Hi", 2);
  }

  CHECK(strcmp(Gstring_get_str(&gb), "Herr gott mine lord!") == 0);
  CHECK(strcmp(Gstring_get_str(&gs), "Hi") == 0);
  CHECK(gb.len == strlen("Herr gott mine lord!"));
  CHECK(gs.len == 2);

  Gstring_destroy(&gb);
  Gstring_destroy(&gs);
}
