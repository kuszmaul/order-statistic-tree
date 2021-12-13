#include "net/bandaid/bdn/cachelib/order_statistic_map.h"

#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <type_traits>
#include <utility>

#include "net/bandaid/bdn/cachelib/order_statistic_test_common.h"
#include "testing/base/public/gmock.h"
#include "testing/base/public/gunit.h"
#include "third_party/absl/strings/str_format.h"
#include "third_party/absl/strings/string_view.h"

namespace cachelib {

using ::testing::AllOf;
using ::testing::Ne;
using ::testing::Pair;
using ::testing::PrintToString;

// Print a map in the same format as gmock prints std::map.
template <class Key, class T, class Compare>
std::ostream& operator<<(std::ostream& out,
                         const OrderStatisticMap<Key, T, Compare>& map) {
  return PrintOst(out, map);
}

// This version doesn't check for past-the-end.
MATCHER_P(IteratorReferences1, matcher,
          absl::StrFormat(
              "%s a reference to a value which %s", negation ? "isn't" : "is",
              ::testing::DescribeMatcher<
                  typename std::remove_reference<arg_type>::type::value_type>(
                  matcher))) {
  return ExplainMatchResult(matcher, *arg, result_listener);
}

template class OrderStatisticMap<size_t, size_t>;
template class OrderStatisticMap<size_t, std::string>;
template class OrderStatisticMap<std::string, size_t>;
template class OrderStatisticMap<std::string, std::string>;

// Make sure that we can build trees when there is no operator> or operator==.
// We should use only operator<.
struct NoEquals {
  explicit NoEquals(int v) : value(v) {}
  int value;
  bool operator<(const NoEquals& other) const { return value < other.value; }
};
struct Nothing {};
template class OrderStatisticMap<NoEquals, Nothing>;

// Checks the `public` types for OrderStatisticMap
TEST(OrderStatisticMapTest, MapPublicTypes) {
  using Map = OrderStatisticMap<size_t, size_t>;
  Map s;
  using Value = std::pair<const size_t, size_t>;
  static_assert(std::is_same_v<size_t, Map::key_type>);
  static_assert(!std::is_same_v<std::string, Map::key_type>);
  static_assert(std::is_same_v<Value, Map::value_type>);
  static_assert(std::is_same_v<size_t, Map::size_type>);
  static_assert(std::is_same_v<Value&, Map::reference>);
  static_assert(std::is_same_v<const Value&, Map::const_reference>);
  static_assert(!is_transparent<std::less<int>>::value);
  static_assert(is_transparent<std::less<>>::value);
  static_assert(is_transparent<Map::key_compare>::value);
  static_assert(
      !is_transparent<OrderStatisticMap<size_t, size_t,
                                        std::less<int>>::key_compare>::value);
  // value_compare is never transparent.
  static_assert(!is_transparent<Map::value_compare>::value);
}

// Checks the `public` types for OrderStatisticMap::iterator
TEST(OrderStatisticMapTest, IteratorPublicTypes) {
  using Map = OrderStatisticMap<size_t, size_t>;
  Map s;
  using It = Map::iterator;
  using Value = std::pair<const size_t, size_t>;
  It it = s.begin();
  EXPECT_THAT(it, IteratorAtEnd(&s));
  static_assert(
      std::is_same_v<It::iterator_category, std::random_access_iterator_tag>);
  static_assert(std::is_same_v<Value, It::value_type>);
  static_assert(std::is_same_v<ptrdiff_t, It::difference_type>);
  static_assert(std::is_same_v<Value*, It::pointer>);
  static_assert(std::is_same_v<Value&, It::reference>);
}

// Checks the `public` types for OrderStatisticMap::reverse_iterator
TEST(OrderStatisticMapTest, ReverseIteratorPublicTypes) {
  using Map = OrderStatisticMap<size_t, size_t>;
  Map s;
  using It = Map::reverse_iterator;
  using Value = std::pair<const size_t, size_t>;
  It it = s.rbegin();
  EXPECT_THAT(it, IteratorAtReverseEnd(&s));
  static_assert(
      std::is_same_v<It::iterator_category, std::random_access_iterator_tag>);
  static_assert(std::is_same_v<Value, It::value_type>);
  static_assert(std::is_same_v<ptrdiff_t, It::difference_type>);
  static_assert(std::is_same_v<Value*, It::pointer>);
  static_assert(std::is_same_v<Value&, It::reference>);
}

// Checks the `public` types for OrderStatisticMap::const_iterator
TEST(OrderStatisticMapTest, ConstIteratorPublicTypes) {
  using Map = OrderStatisticMap<size_t, size_t>;
  Map s;
  using It = Map::const_iterator;
  using Value = const std::pair<const size_t, size_t>;
  It it = s.begin();
  EXPECT_THAT(it, IteratorAtEnd(&s));
  static_assert(
      std::is_same_v<It::iterator_category, std::random_access_iterator_tag>);
  static_assert(std::is_same_v<Value, It::value_type>);
  static_assert(std::is_same_v<ptrdiff_t, It::difference_type>);
  static_assert(std::is_same_v<Value*, It::pointer>);
  static_assert(std::is_same_v<Value&, It::reference>);
}

// Checks the `public` types for OrderStatisticMap::const_reverse_iterator
TEST(OrderStatisticMapTest, ConstReverseIteratorPublicTypes) {
  using Map = OrderStatisticMap<size_t, size_t>;
  Map s;
  using It = Map::const_reverse_iterator;
  using Value = const std::pair<const size_t, size_t>;
  It it = s.rbegin();
  EXPECT_THAT(it, IteratorAtReverseEnd(&s));
  static_assert(
      std::is_same_v<It::iterator_category, std::random_access_iterator_tag>);
  static_assert(std::is_same_v<Value, It::value_type>);
  static_assert(std::is_same_v<ptrdiff_t, It::difference_type>);
  static_assert(std::is_same_v<Value*, It::pointer>);
  static_assert(std::is_same_v<Value&, It::reference>);
}

TEST(OrderStatisticMapTest, Basic) {
  std::map<size_t, size_t> map;
  OrderStatisticMap<size_t, size_t> ost;
  EXPECT_TRUE(ost.empty());
  EXPECT_FALSE(ost.contains(0));
  EXPECT_EQ(ost.crbegin(), ost.crend());
  EXPECT_EQ(ost.rbegin(), ost.rend());
  EXPECT_EQ(map, ost);
  map[1] = 1;
  EXPECT_THAT(ost.insert_or_assign(1, 1),
              Pair(AllOf(IteratorReferences(&ost, Pair(1, 1)), Rank(0)), true));
  EXPECT_FALSE(ost.empty());
  EXPECT_TRUE(ost.contains(1));
  EXPECT_THAT(
      ost.insert(std::pair(1, 2)),
      Pair(AllOf(IteratorReferences(&ost, Pair(1, 1)), Rank(0)), false));
  EXPECT_EQ(map, ost);
  EXPECT_THAT(ost.lower_bound(0), IteratorReferences(&ost, Pair(1, 1)));
  EXPECT_THAT(ost.lower_bound(1), IteratorReferences(&ost, Pair(1, 1)));
  EXPECT_EQ(ost.lower_bound(2), ost.end());
  map[3] = 3;
  EXPECT_THAT(ost.insert_or_assign(3, 2),
              Pair(IteratorReferences(&ost, Pair(3, 2)), true));
  EXPECT_THAT(ost.insert_or_assign(3, 3),
              Pair(IteratorReferences(&ost, Pair(3, 3)), false));
  EXPECT_THAT(ost.insert(std::pair(1, 99)),
              Pair(IteratorReferences(&ost, Pair(1, 1)), false));
  EXPECT_THAT(ost.insert(std::pair(3, 99)),
              Pair(IteratorReferences(&ost, Pair(3, 3)), false));
  // Tree is now (1, 1), (3, 3)
  EXPECT_EQ(ost.size(), 2);
  EXPECT_THAT(ost.find(0), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.find(1), IteratorReferences(&ost, Pair(1, 1)));
  EXPECT_THAT(ost.find(2), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.find(3), IteratorReferences(&ost, Pair(3, 3)));
  EXPECT_THAT(ost.find(4), IteratorAtEnd(&ost));
  EXPECT_NE(ost.select(0), ost.end());
  EXPECT_THAT(ost.select(0), IteratorReferences(&ost, Pair(1, 1)));
  EXPECT_THAT(ost.select(1), IteratorReferences(&ost, Pair(3, 3)));
  EXPECT_THAT(ost.select(2), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.lower_bound(0), IteratorReferences(&ost, Pair(1, 1)));
  EXPECT_THAT(ost.lower_bound(1), IteratorReferences(&ost, Pair(1, 1)));
  EXPECT_THAT(ost.lower_bound(2), IteratorReferences(&ost, Pair(3, 3)));
  EXPECT_THAT(ost.lower_bound(3), IteratorReferences(&ost, Pair(3, 3)));
  EXPECT_EQ(ost.lower_bound(4), ost.end());
  EXPECT_EQ(map, ost);
  {
    std::ostringstream out;
    out << ost;
    EXPECT_EQ(out.str(), "{ (1, 1), (3, 3) }");
  }
  {
    std::ostringstream out;
    ost.PrintStructureForTest(out,
                              PrintToString<std::pair<const size_t, size_t>>);
    EXPECT_EQ(out.str(), "((1, 1):2 () ((3, 3):1 () ()))");
  }

  // erase something not present
  EXPECT_EQ(ost.erase(99), 0);
  EXPECT_EQ(map, ost);

  EXPECT_EQ(ost.erase(0), map.erase(0));
  EXPECT_EQ(map, ost);

  EXPECT_EQ(ost.erase(1), map.erase(1));
  EXPECT_EQ(map, ost);

  EXPECT_EQ(ost.erase(1), map.erase(1));  // erase 1 again (not present)
  EXPECT_EQ(map, ost);

  EXPECT_TRUE(ost.key_comp()(1, 2));
  std::pair<size_t, size_t> a{1, 10};
  std::pair<size_t, size_t> b{2, 0};
  EXPECT_TRUE(ost.value_comp()(a, b));
}

TEST(OrderStatisticMapTest, Erase) {
  std::map<size_t, size_t> map;
  OrderStatisticMap<size_t, size_t> ost;
  map[1] = 1;
  ost.insert_or_assign(1, 1);
  EXPECT_EQ(map.erase(map.find(1)), map.end());
  EXPECT_EQ(ost.erase(ost.find(1)), ost.end());
  EXPECT_EQ(map, ost);
  map[1] = 1;
  ost.insert_or_assign(1, 1);
  map[2] = 2;
  ost.insert_or_assign(2, 2);
  EXPECT_THAT(map.erase(map.find(1)), IteratorReferences(&map, Pair(2, 2)));
  EXPECT_THAT(ost.erase(ost.find(1)), IteratorReferences(&ost, Pair(2, 2)));
  EXPECT_EQ(map, ost);
  EXPECT_EQ(map.erase(map.find(2)), map.end());
  EXPECT_EQ(ost.erase(ost.find(2)), ost.end());
  EXPECT_EQ(map, ost);

  map[1] = 1;
  ost.insert_or_assign(1, 1);
  map[2] = 2;
  ost.insert_or_assign(2, 2);
  map[3] = 3;
  ost.insert_or_assign(3, 3);
  map[4] = 4;
  ost.insert_or_assign(4, 4);
  for (size_t i = 0; i < 4; ++i) {
    EXPECT_EQ(map.erase(--map.end()), map.end());
    EXPECT_EQ(ost.erase(--ost.end()), ost.end());
    EXPECT_EQ(map, ost);
    EXPECT_EQ(ost.size(), 3 - i);
  }
}

// Check that we can pass in strings as strings, string_views, or string
// literals.  Also use a different type for the value so that we can check for
// type errors (key and value type being confused somehow).
TEST(OrderStatisticMapTest, StringInt) {
  std::map<std::string, size_t> map;
  OrderStatisticMap<std::string, size_t> ost;
  EXPECT_EQ(map, ost);
  map["a"] = 0;
  EXPECT_THAT(ost.insert_or_assign(std::string("a"), 0),
              Pair(IteratorReferences(&ost, Pair("a", 0)), true));
  EXPECT_EQ(map, ost);
  map["b"] = 1;
  EXPECT_THAT(ost.insert_or_assign("b", 1),
              Pair(IteratorReferences(&ost, Pair("b", 1)), true));
  map["c"] = 2;
  EXPECT_THAT(ost.insert_or_assign("c", 2),
              Pair(IteratorReferences(&ost, Pair("c", 2)), true));
  EXPECT_EQ(ost.size(), 3u);
  EXPECT_THAT(ost.find(""), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.find("a"), IteratorReferences(&ost, Pair("a", 0)));
  EXPECT_THAT(ost.find(std::string("a")),
              IteratorReferences(&ost, Pair("a", 0)));
  EXPECT_THAT(ost.find(absl::string_view("a")),
              IteratorReferences(&ost, Pair("a", 0)));
  EXPECT_THAT(ost.find("aa"), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.find("b"), IteratorReferences(&ost, Pair("b", 1)));
  EXPECT_THAT(ost.find("bb"), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.find("c"), IteratorReferences(&ost, Pair("c", 2)));
  EXPECT_THAT(ost.find("cc"), IteratorAtEnd(&ost));
  EXPECT_THAT(ost.select(0), IteratorReferences(&ost, Pair("a", 0)));
  EXPECT_THAT(ost.select(1), IteratorReferences(&ost, Pair("b", 1)));
  EXPECT_THAT(ost.select(2), IteratorReferences(&ost, Pair("c", 2)));
  EXPECT_THAT(ost.select(3), IteratorAtEnd(&ost));
  EXPECT_EQ(map, ost);
  EXPECT_EQ(map.erase(std::string("a")), ost.erase("a"));
  EXPECT_EQ(map, ost);
  EXPECT_EQ(ost.erase(absl::string_view("b")), map.erase("b"));
  EXPECT_EQ(map, ost);
  EXPECT_EQ(ost.erase("c"), map.erase("c"));
  EXPECT_EQ(map, ost);
}

TEST(OrderStatisticMapTest, Randomized) {
  RunRandomized<OrderStatisticMap<size_t, size_t>>(
      []([[maybe_unused]] auto& a, [[maybe_unused]] auto& b) {});
}

TEST(OrderStatisticMapTest, Iterator) {
  OrderStatisticMap<size_t, size_t> t;
  EXPECT_EQ(t.begin(), t.end());
  EXPECT_THAT(t.insert_or_assign(0, 0),
              Pair(IteratorReferences(&t, Pair(0, 0)), true));
  EXPECT_THAT(t.insert_or_assign(1, 1),
              Pair(IteratorReferences(&t, Pair(1, 1)), true));
  EXPECT_EQ(t.find(0), t.find(0));
  EXPECT_LE(t.find(0), t.find(0));
  EXPECT_GE(t.find(0), t.find(0));

  EXPECT_LT(t.find(0), t.find(1));
  EXPECT_LE(t.find(0), t.find(1));

  EXPECT_GT(t.find(1), t.find(0));
  EXPECT_GE(t.find(1), t.find(0));

  EXPECT_EQ(t.find(1) - t.find(0), 1);
  EXPECT_EQ(t.find(0) - t.find(1), -1);
  EXPECT_EQ(t.find(0) + 1, t.find(1));
  EXPECT_EQ(t.find(1) - 1, t.find(0));
  EXPECT_THAT(t.find(0), IteratorReferences(&t, Pair(0, 0)));
  EXPECT_THAT(t.find(0)[0], Pair(0, 0));
  EXPECT_THAT(t.find(0)[1], Pair(1, 1));

  EXPECT_LT(t.rbegin(), t.rend());
  EXPECT_EQ(t.rbegin() + 2, t.rend());
  EXPECT_THAT(t.rbegin(), AllOf(Ne(t.rend()), IteratorReferences1(Pair(1, 1))));
  EXPECT_THAT(t.rbegin() + 1,
              AllOf(Ne(t.rend()), IteratorReferences1(Pair(0, 0))));
  EXPECT_EQ(t.rbegin() + 2, t.rend());

  EXPECT_EQ(t.begin(), t.cbegin());
  EXPECT_EQ(t.end(), t.cend());
  EXPECT_EQ(t.rbegin(), t.crbegin());
  EXPECT_EQ(t.rend(), t.crend());
}

TEST(OrderStatisticMapTest, FindNotThere) {
  OrderStatisticMap<size_t, size_t> t;
  t.insert_or_assign(1, 1);
  t.insert_or_assign(3, 3);
  t.insert_or_assign(5, 5);
  EXPECT_EQ(t.find(0), t.end());
  EXPECT_NE(t.find(1), t.end());
  EXPECT_EQ(t.find(2), t.end());
  EXPECT_NE(t.find(3), t.end());
  EXPECT_EQ(t.find(4), t.end());
  EXPECT_NE(t.find(5), t.end());
  EXPECT_EQ(t.find(6), t.end());
  const OrderStatisticMap<size_t, size_t>& tc = t;
  EXPECT_EQ(tc.find(0), tc.end());
  EXPECT_NE(tc.find(1), tc.end());
  EXPECT_EQ(tc.find(2), tc.end());
  EXPECT_NE(tc.find(3), tc.end());
  EXPECT_EQ(tc.find(4), tc.end());
  EXPECT_NE(tc.find(5), tc.end());
  EXPECT_EQ(tc.find(6), tc.end());
}

struct LessNotTransparent {
  bool operator()(const size_t& a, const size_t& b) const { return a < b; }
};

TEST(OrderStatisticMapTest, FindNotThereNotTransparent) {
  // std::map<size_t, size_t, LessNotTransparent> t;
  OrderStatisticMap<size_t, size_t, LessNotTransparent> t;
  t.insert_or_assign(1, 1);
  t.insert_or_assign(3, 3);
  t.insert_or_assign(5, 5);
  EXPECT_EQ(t.find(0), t.end());
  EXPECT_NE(t.find(1), t.end());
  EXPECT_EQ(t.find(2), t.end());
  EXPECT_NE(t.find(3), t.end());
  EXPECT_EQ(t.find(4), t.end());
  EXPECT_NE(t.find(5), t.end());
  EXPECT_EQ(t.find(6), t.end());

  const OrderStatisticMap<size_t, size_t, LessNotTransparent>& tc = t;
  EXPECT_EQ(tc.find(0), tc.end());
  EXPECT_NE(tc.find(1), tc.end());
  EXPECT_EQ(tc.find(2), tc.end());
  EXPECT_NE(tc.find(3), tc.end());
  EXPECT_EQ(tc.find(4), tc.end());
  EXPECT_NE(tc.find(5), tc.end());
  EXPECT_EQ(tc.find(6), tc.end());
}

// Test that we can assign to the second of the value from an iterator.
TEST(AssignSecondTest, Basic) {
  OrderStatisticMap<int, int> t;
  t.insert_or_assign(1, 1);
  t.insert_or_assign(2, 2);
  {
    OrderStatisticMap<int, int>::iterator it = t.find(1);
    EXPECT_NE(it, t.end());
    EXPECT_THAT(*it, Pair(1, 1));
    it->second = 3;
  }
  {
    auto it = t.find(1);
    EXPECT_NE(it, t.end());
    EXPECT_THAT(*it, Pair(1, 3));
  }
  {
    const OrderStatisticMap<int, int>& t2 = t;
    auto it = t2.find(1);
    // This shouldn't compile
    // it->second = 4;
    EXPECT_THAT(it[0], Pair(1, 3));
  }
}
}  // namespace cachelib
