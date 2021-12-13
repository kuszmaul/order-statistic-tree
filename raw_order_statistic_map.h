#ifndef NET_BANDAID_BDN_CACHELIB_RAW_ORDER_STATISTIC_MAP_H_
#define NET_BANDAID_BDN_CACHELIB_RAW_ORDER_STATISTIC_MAP_H_

#include <utility>

#include "net/bandaid/bdn/cachelib/raw_order_statistic_set.h"  // IWYU pragma: export

namespace cachelib {
namespace cachelib_internal {

template <class Key, class T, class Compare, class Derived>
class RawMapNode
    : public cachelib_internal::RawNode<Key, std::pair<const Key, T>, Compare,
                                        Derived> {
  using Base = typename RawMapNode::RawNode;

 public:
  using value_type = std::pair<const Key, T>;
  explicit RawMapNode(value_type v) : Base(std::move(v)) {}
  static const Key &key(const value_type &value) { return value.first; }
  void SetMappedValue(const value_type &k) { this->value_.second = k.second; }
};

template <class Key, class T, class Compare, class Node>
class RawOrderStatisticMap : public cachelib_internal::RawOrderStatisticSet<
                                 Key, std::pair<const Key, T>, Compare, Node> {
  using Base = typename RawOrderStatisticMap::RawOrderStatisticSet;

 protected:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = typename Base::size_type;
  using difference_type = typename Base::difference_type;
  using key_compare = Compare;
  struct value_compare {
    bool operator()(const value_type &lhs, const value_type &rhs) const {
      return key_compare()(lhs.first, rhs.first);
    }
  };
  value_compare value_comp() const { return value_compare(); }
  // TODO(bradleybear): Add allocator_type
  using reference = value_type &;
  using const_reference = const reference;
  // TODO(bradleybear): Add pointer and const_pointer
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using const_reverse_iterator = typename Base::const_reverse_iterator;

  // Inserts or assigns as does map::insert_or_assign().  If k isn't in the map,
  // the inserts it and return an iterator pointing at the newly inserted value,
  // and true.  If k is in the map, then assigns it, and returns the iterator
  // and false.
  //
  // Rationale: For std::map, the insert_or_assign() method takes templated
  // rvalue references.  For now, we'll just keep it simple here and take
  // ownership of the `k` and `v`.
  std::pair<iterator, bool> insert_or_assign(Key k, T v) {
    auto [new_root, inserted_at_idx, inserted_at_node, did_insert] =
        Node::Insert(std::move(this->root_), {std::move(k), std::move(v)}, 0,
                     true, this->lessthan_);
    this->root_ = std::move(new_root);
    return {iterator(this, inserted_at_idx, inserted_at_node), did_insert};
  }
};

}  // namespace cachelib_internal
}  // namespace cachelib

#endif  // NET_BANDAID_BDN_CACHELIB_RAW_ORDER_STATISTIC_MAP_H_
