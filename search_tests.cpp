#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/search.h"

using namespace std;
using namespace testing;

// CleanToken tests
TEST(CleanToken, TrimsPunctuation) {
  EXPECT_THAT(cleanToken("!!!test!!!"), Eq("test"));
  EXPECT_THAT(cleanToken("don't"), Eq("don't"));
}

TEST(CleanToken, ReturnsEmptyIfNoLetters) {
  EXPECT_THAT(cleanToken("12345"), Eq(""));
  EXPECT_THAT(cleanToken("..."), Eq(""));
}

TEST(CleanToken, TestsPunctuationMiddleAndEnds) {
  EXPECT_THAT(cleanToken("!don't"), Eq("don't"));
  EXPECT_THAT(cleanToken("don't!"), Eq("don't"));
  EXPECT_THAT(cleanToken("!don't!"), Eq("don't"));
}



// GatherTokens tests
TEST(GatherTokens, HandlesWhitespace) {
  string text = "  test   test  test  ";
  set<string> expected = {"test", "test", "test"};
  EXPECT_THAT(gatherTokens(text), ContainerEq(expected));
}

TEST(GatherTokens, HandlesEdgeCaseSpaces) {
  string text1 = "test      test";
  set<string> expected1 = {"test", "test"};
  EXPECT_THAT(gatherTokens(text1), ContainerEq(expected1));

  string text2 = "  test  ";
  set<string> expected2 = {"test"};
  EXPECT_THAT(gatherTokens(text2), ContainerEq(expected2));

  string text3 = "   test";
  EXPECT_THAT(gatherTokens(text3), ContainerEq(expected2));

  string text4 = "test  ";
  EXPECT_THAT(gatherTokens(text4), ContainerEq(expected2));
}

TEST(GatherTokens, UniquenessAndCleaning) {
  string text = "Test, test., TEST";
  set<string> expected = {"test"};
  EXPECT_THAT(gatherTokens(text), ContainerEq(expected));
}


// BuildIndex tests
TEST(BuildIndex, ProcessesCorrectCount) {
    map<string, set<string>> index;
    int count = buildIndex("testfile.txt", index);
    
    EXPECT_THAT(count, Eq(2));
    EXPECT_THAT(index["hello"].size(), Eq(1));
    EXPECT_THAT(index["world"].size(), Eq(1));
}

// FindQueryMatches tests
TEST(FindQueryMatches, MissingTerms) {
    map<string, set<string>> index = {
      {"tasty", {"url1.com", "url2.com"}},
      {"healthy", {"url2.com", "url3.com"}}
    };
    EXPECT_THAT(findQueryMatches(index, "nonexistent"), IsEmpty());
    EXPECT_THAT(findQueryMatches(index, "tasty +nonexistent"), IsEmpty());
    set<string> expected = {"url1.com", "url2.com"};
    EXPECT_THAT(findQueryMatches(index, "tasty -nonexistent"), ContainerEq(expected));
}

TEST(FindQueryMatches, CompoundQueries) {
    map<string, set<string>> index = {
      {"red", {"u1", "u2"}},
      {"blue", {"u2", "u3"}},
      {"green", {"u1"}}
    };
    set<string> expectedOr = {"u1", "u2", "u3"};
    EXPECT_THAT(findQueryMatches(index, "red blue"), ContainerEq(expectedOr));
    set<string> expectedAnd = {"u1"};
    EXPECT_THAT(findQueryMatches(index, "red blue +green"), ContainerEq(expectedAnd));
}