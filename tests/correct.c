#include "libcdict.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Exercise both compressed prefixes and one-byte branch traversal. */
static void check(char const *typed, char const *word,
    uint8_t const *proximity, int expected)
{
  for (int branches = 0; branches <= 1; branches++)
  {
    uint8_t nodes[128] = {0};
    uint8_t freq[] = {15};
    int len = strlen(word);
    if (branches)
    {
      for (int i = 0; i < len; i++)
      {
        /* 8-bit pointers and numbers, one edge, padded to even offsets. */
        nodes[i * 6] = 36;
        nodes[i * 6 + 1] = 1;
        nodes[i * 6 + 2] = word[i];
        nodes[i * 6 + 3] = 6 | (i == len - 1);
      }
    }
    else
    {
      nodes[0] = (len << 1) | 1;
      nodes[3] = ((4 + len + 1) & ~1) | 1;
      memcpy(nodes + 4, word, len);
    }
    cdict_t dict = {.root_node = nodes, .freq = freq};
    int index = -1, cost = -1;
    int n = cdict_correct(&dict, typed, strlen(typed), proximity,
        expected, 16, &index, &cost, 1);
    assert(n == 1 && index == 0 && cost == expected);
    n = cdict_correct(&dict, typed, strlen(typed), proximity,
        expected - 1, 16, &index, &cost, 1);
    assert(n == 0);
  }
}

int main(void)
{
  check("ot", "to", NULL, 1);
  check("fo", "of", NULL, 1);
  check("si", "is", NULL, 1);
  check("ti", "it", NULL, 1);
  check("xbced", "abcde", NULL, 3);
  uint8_t proximity[128 * 128] = {0};
  proximity['s' * 128 + 'a'] = 1;
  check("sot", "ato", proximity, 2);
  check("sot", "ato", NULL, 3);
  puts("Native correction boundary regressions passed (prefix and branch nodes)");
  return 0;
}
