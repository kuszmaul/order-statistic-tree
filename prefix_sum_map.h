// The PrefixMap is an extension of OrderStatisticMap that allows you to ask for
// the sum of the first n mapped values.
//
// See the order_statistic_set.h and order_statistic_map.h comments for more
// information.

#ifndef NET_BANDAID_BDN_CACHELIB_PREFIX_SUM_MAP_H_
#define NET_BANDAID_BDN_CACHELIB_PREFIX_SUM_MAP_H_

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

#include "base/logging.h"
#include "net/bandaid/bdn/cachelib/raw_order_statistic_map.h"  // IWYU pragma: export

namespace cachelib {
namespace cachelib_internal {

// A prefix tree that adds up the T's
template <class Key, class T, class Compare = std::less<>>
class PrefixMapNode
    : public cachelib_internal::RawMapNode<Key, T, Compare,
                                           PrefixMapNode<Key, T, Compare>> {
 private:
  using Base = typename PrefixMapNode::RawMapNode;
  using key_compare = typename PrefixMapNode::key_compare;

 public:
  using value_type = typename Base::value_type;

 public:
  explicit PrefixMapNode(value_type v)
      : Base(std::move(v)), sum_(this->value_.second) {}

  // Checks the invariant on the sum (the sum equals the sum of the value and
  // the children's sum).  This should probably be called only in test code.
  void Check(bool recursive, const key_compare &lessthan) {
    const T &nv = this->value_.second;
    T sum = this->left_ ? this->left_->sum_ + nv : nv;
    sum = this->right_ ? sum + this->right_->sum_ : sum;
    CHECK_EQ(sum_, sum)                                          // Crash OK
        << " node=" << this << " leftsum=" << this->left_->sum_  //
        << " rightsum=" << this->right_->sum_ << " this val=" << nv;
    if (recursive) {
      if (this->left_) this->left_->Check(recursive, lessthan);
      if (this->right_) this->right_->Check(recursive, lessthan);
    }
    // Check the base type, but don't do it recursively since that will be
    // handled by the recursive calls just above.
    Base::Check(false, lessthan);
  }

  // Maintains the sum and invokes RecomputeSummary() on the base type (which in
  // this case maintains the rank information).
  void RecomputeSummary() {
    const T &nv = this->value_.second;
    const std::unique_ptr<PrefixMapNode> &left = this->left_;
    const std::unique_ptr<PrefixMapNode> &right = this->right_;
    T leftsum = (left ? left->sum_ + nv : nv);
    sum_ = (right ? leftsum + right->sum_ : leftsum);
    Base::RecomputeSummary();
  }

 public:
  // The query operation: Returns the sum of the all the values with rank < idx.
  static T SumFirstN(const std::unique_ptr<PrefixMapNode> &n, size_t idx) {
    if (n == nullptr) return T{};  // zero
    size_t left_size = PrefixMapNode::Size(n->left_);
    if (left_size >= idx) {
      return SumFirstN(n->left_, idx);
    } else {
      const T &nv = n->value_.second;
      T sum = n->left_ ? n->left_->sum_ + nv : nv;
      return sum + SumFirstN(n->right_, idx - left_size - 1);
    }
  }

 private:
  T sum_;
};

}  // namespace cachelib_internal

template <class Key, class T, class Compare = std::less<>>
class PrefixSumMap
    : public cachelib_internal::RawOrderStatisticMap<
          Key, T, Compare, cachelib_internal::PrefixMapNode<Key, T, Compare>> {
  using Base = typename PrefixSumMap::RawOrderStatisticMap;
  using Node = typename Base::Node;

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

  // The new method supported by PrefixSumMap:
  //
  // Returns the sum of the first n keys in the tree.
  T SumFirstN(size_t n) const { return Node::SumFirstN(this->root_, n); }

  // See OrderStatisticSet's documentation for the specification of these
  // functions.

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

  // See OrderStatisticMap's documentation for the specification of this
  // function.
  using Base::insert_or_assign;
};

}  // namespace cachelib

#endif  // NET_BANDAID_BDN_CACHELIB_PREFIX_SUM_MAP_H_
