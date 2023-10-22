#ifndef LOCKFREEBST_H
#define LOCKFREEBST_H

#include <exception>
#include <stdexcept>
#include <unistd.h>
#include <cassert>
#include <utility>
#include <atomic>
#include <limits>
#include <stack>

namespace LF {

template <class K, class V>
struct Node {
	const K key;
	const V val;
	Node * parent;

	std::atomic<Node*> left;
	std::atomic<Node*> right;
	static_assert(std::atomic<Node*>::is_always_lock_free);

	std::atomic<uint64_t> epoch;
	static const uint64_t MAX_EPOCH = std::numeric_limits<uint64_t>::max();
	static_assert(std::atomic<uint64_t>::is_always_lock_free);

	Node(K k, V v, uint64_t ep) : key(k), val(v), parent(nullptr), left(nullptr), right(nullptr), epoch(ep) { }

	static bool is_null(Node * p, uint64_t ep)
    {
    	return (p == nullptr) || (p->epoch >= ep);
    }

	enum Direction { LEFT, RIGHT };
	Node * child(Direction d) { return (d == Direction::LEFT ? left : right).load(); }
	Node * other(Direction d) { return (d == Direction::LEFT ? right : left).load(); }
};

template <class K, class V>
struct BSTIterator {

	using node_type = Node<K, V>;
	using node_ptr = node_type*;

	BSTIterator(node_ptr np = nullptr, uint64_t ep = 0) : ptr_(np), epoch_(ep) { }
	~BSTIterator() = default;

	using Direction = typename node_type::Direction;
	BSTIterator inc() { return step_impl(Direction::LEFT); }
	BSTIterator dec() { return step_impl(Direction::RIGHT); }

	node_ptr ptr() const { return ptr_; }
	std::pair<K, V> pair() const
	{
		if (ptr_ != nullptr) { return std::pair<K, V>(ptr_->key, ptr_->val); }
		else { throw std::runtime_error("cannot dereference empty iterator"); }
	}

protected:

	// Implementation modified from:
	// Pfaff, Ben (2004). An Introduction to Binary Search Trees and Balanced Trees.
	// Free Software Foundation, Inc.
	// Comments describe single direction, but implementation is symmetric.
	BSTIterator step_impl(Direction d)
    {
    	assert("can't iterate an empty tree" && ptr_ != nullptr);
    	node_ptr np = ptr_; // New pointer to update.
    	node_ptr chl = np->child(d);
    	
    	// Find left-most node in right subtree
    	if (!node_type::is_null(chl, epoch_)) do { np = chl; chl = chl->other(d); } while (!node_type::is_null(chl, epoch_));
    	// Find parent of root of subtree we are right-most node of
    	else {
    		node_ptr prev;
    		do { 
				prev = np;
				np = np->parent;
				if (np == nullptr) { /* wrap around */
					for (; !node_type::is_null(prev, epoch_); prev = prev->other(d)) { np = prev; } break;
				}
			} while (np->child(d) == prev);
		}

		return BSTIterator(np, epoch_);
    }

private:
	node_ptr ptr_;
	uint64_t epoch_;
};

template <class K, class V>
class BST {
public:
	using key_type = K;
	using value_type = V;
	using node_type = Node<key_type, value_type>; 
	using node_ptr = node_type*;

	using iter = BSTIterator<key_type, value_type>;

	BST() : root_(nullptr) { }
	
	// Not thread-safe
	~BST() { dtor_impl(root_.load()); }

	void dtor_impl(node_ptr root)
	{
		if (root == nullptr) { return; }

		dtor_impl((root->left).load());
		dtor_impl((root->right).load());

		delete root;
	}

	// thread-safe
	// wait-free
	// find insert position
	iter find(double target) const { uint64_t ep = epoch_.load(); return find_impl(target, root_.load(), ep); }
	iter find_impl(double target, node_ptr root, uint64_t ep) const
	{
		node_ptr pre = nullptr;
		
		for (node_ptr cur = root; !node_type::is_null(cur, ep);) {
			pre = cur; // record path

			if (cur->key == target) { break; }
			cur = (cur->key < target) ?  (cur->left).load() : (cur->right).load();
		}

		// create the iterator
		return iter(pre, ep);
	}

	// thread-safe
	// keys must be unique
	iter insert(key_type k, value_type v)
	{
		node_ptr node = new Node(k, v, 0);

		iter loc;
		std::atomic<node_ptr> * edge;

		// start at root
		node_ptr old = root_.load();

		do {
			// find insert location
			loc = find_impl(k, old, node_type::MAX_EPOCH);
			node_ptr target = loc.ptr();
			
			if (target == nullptr) {
				edge = &root_;
			} else {
				key_type key = target->key; 
				
				if (key == k) {
					delete node;
					throw std::runtime_error("duplicate key");
				}

				edge = &((key < k) ? target->left : target->right);
				old = edge->load();
			}

			// Set new node's parent pointer
			node->parent = target;
			node->epoch = epoch_++; // update epoch

		} while(!edge->compare_exchange_weak(old, node)); // update old

		return loc; // Note: this iterator is not epoch-bound
	}

private:
	std::atomic<node_ptr> root_;
	std::atomic<uint64_t> epoch_;
};

}

#endif // LOCKFREEBST_H
