// An OrderStatisticMap is like a std::map that allows you to determine the rank
// of a given key (i.e., how many keys less than the given key exist in the
// tree) or to select an element by rank (e.g., to ask for the 143rd item in the
// tree).  It works by maintaining a count of how big each subtree is.
//
// The method names are lower-case, rather than using Google's CamelCase naming
// convention, in order to be more like std::map.
//
// See the raw_order_statistics_set.h for a implementation information.

#ifndef NET_BANDAID_BDN_CACHELIB_ORDER_STATISTIC_MAP_H_
#define NET_BANDAID_BDN_CACHELIB_ORDER_STATISTIC_MAP_H_

#include <functional>
#include <utility>

#include "net/bandaid/bdn/cachelib/raw_order_statistic_map.h"  // IWYU pragma: export

namespace cachelib {
namespace cachelib_internal {

template <class Key, class T, class Compare>
class OstMapNode
    : public cachelib_internal::RawMapNode<Key, T, Compare,
                                           OstMapNode<Key, T, Compare>> {
 private:
  using Base = typename OstMapNode::RawMapNode;

 public:
  using value_type = typename Base::value_type;
  explicit OstMapNode(value_type v) : Base(std::move(v)) {}
};

}  // namespace cachelib_internal

template <class Key, class T, class Compare = std::less<>>
class OrderStatisticMap
    : public cachelib_internal::RawOrderStatisticMap<
          Key, T, Compare, cachelib_internal::OstMapNode<Key, T, Compare>> {
 private:
  using Base = typename OrderStatisticMap::RawOrderStatisticMap;

 public:
  using key_type = typename Base::key_type;
  using mapped_type = typename Base::mapped_type;
  using value_type = typename Base::value_type;
  using size_type = typename Base::size_type;
  using difference_type = typename Base::difference_type;
  using key_compare = typename Base::key_compare;
  using value_compare = typename Base::value_compare;
  // TODO(bradleybear): Add allocator_type
  using reference = value_type &;
  using const_reference = const value_type &;
  // TODO(bradleybear): Add pointer and const_pointer
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using reverse_iterator = typename Base::reverse_iterator;
  using const_reverse_iterator = typename Base::const_reverse_iterator;

  // std::pair<iterator, bool> insert_or_assign(const Key &k, const T &v);
  //
  //   If the map doesn't contain an element with an equivalent key, then
  //   inserts `{k, v}` into the map and returns an iterator to the newly
  //   inserted value and true.  Otherwise assigns `v` to the `mapped_type` of
  //   the existing element and returns an iterator to the elemnt and false.
  using Base::insert_or_assign;

  // See OrderStatisticSet's documentation for the specification of these
  // functions.
  //
  // Note: the extra blank lines are to prevent `hg fix` from reordering the
  // lines.
  using Base::size;

  using Base::empty;

  using Base::begin;

  using Base::cbegin;

  using Base::end;

  using Base::cend;

  using Base::rbegin;

  using Base::crbegin;

  using Base::rend;

  using Base::crend;

  using Base::find;

  using Base::contains;

  using Base::lower_bound;

  using Base::upper_bound;

  using Base::select;

  using Base::clear;

  using Base::insert;

  using Base::erase;

  using Base::key_comp;

  using Base::value_comp;
};

}  // namespace cachelib

#endif  // NET_BANDAID_BDN_CACHELIB_ORDER_STATISTIC_MAP_H_
