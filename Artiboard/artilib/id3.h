#pragma once
#include <set>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <memory>
#include <forward_list>
#include <iterator>
#include "systemex.h"
#include "log.h"

namespace arti {

template <class T> std::size_t size_of(const std::forward_list<T>& list) {
   return std::distance(list.begin(),list.end());
}

/** This classifier is an implementation of Quinlan's ID3 algorithm.
In order to use it you have to index your attributes and elements attribute values 
classes as integers. Attributes and element indexes are zero based and in sequence
The ID3NameResolver gives names to these indexes.
*/
class ID3NameResolver {
public:
   virtual std::string attribute_name(const size_t a) = 0;
   virtual std::string value_name(const size_t a, const size_t v) = 0;
   virtual std::string class_name(const size_t c) = 0;
   virtual ~ID3NameResolver(){}
};
typedef std::map<int,int> mapii;
/** A node in the classification tree. 
 */
class ID3Node {
public:
   const int level; /** root is zero */
   const int attribute; /** if -1, this is the root */
   const int value; /** if -1, this is the root */
   const int dominant_class; /* dominant found during training */ 
   const bool is_classified_leaf;
   mapii class_count; // Nc 
   mapii value_count; // Nv
   std::forward_list<ID3Node> childs;
   float best_entropy; // why the attribute for the childs was selected
   int test_count;
   int test_errors;
   ID3Node(const int lvl, const int a, const int v, const int c, const bool leaf) 
   : level(lvl), attribute(a), value(v), dominant_class(c), is_classified_leaf(leaf), best_entropy(999.999f)
   , test_count(-1), test_errors(-1) {}
   ID3Node() : ID3Node(0,-1,-1,-1,false){}
   bool is_root() const {return level == 0; }
   bool is_leaf() const {return is_classified_leaf || childs.empty();}
   size_t childs_size() const {return std::distance(childs.begin(), childs.end());}
   size_t size() const {int result = 1; FOR_EACH(c, childs) {result+=c->size();}; return result;} 
   void to_stream(std::ostream& os, ID3NameResolver &r) const;
   bool pruned() const {return is_leaf() && !is_classified_leaf;}
   int pruned_count() const {
  	 if (pruned()) return 1;
  	 int r = 0; for(auto &c:childs)r+=c.pruned_count();return r;}
   int leaf_count() const { if (is_leaf()) return 1; else {int r = 0; for(auto &c:childs)r+=c.leaf_count();return r;}}
   bool is_tested() const {return test_count > -1;}
   float certainty() const {if (test_count <= 0) return 0; else return ((test_count-test_errors)*100)/(test_count*1.0f);}  
   void clear_test_data() {test_count=0;test_errors=0;FOR_EACH(c,childs) c->clear_test_data();}
};

std::ostream& operator<<(std::ostream &os, const ID3Node& v);

/**
 * The ID3Classifier produces an ID3Node that resolves values.
 * A client of this class inherits from it.  The responsibility of the client it to
 * provide the data that must be classified.  It does this by implementing
 * value_of() and class_of(); members that interpret the meaning on the elements
 * that are being classified. These members typically 'lookup' a data structure
 * that is referenced by the client.
 */
class ID3Classifier {
private: 
   ID3Node _root;
public:   
   const size_t mo_cut_off_; // minimal object pruning cut-off
   ID3Classifier(size_t cc = 0) : mo_cut_off_(cc) {}
   /** the value the element has for the given attribute */
   virtual int value_of(const size_t element, const size_t attribute) = 0;
   /** the class of the element */
   virtual int class_of(const size_t element) = 0;
   /** create the classifier root node */
   void train(const size_t elementCount, const size_t attributeCount);
   /** splits elements according to denominated into example and test set.
    * if denominator is 0, these is no split and testing is done using the whole
    * input set
    */
   void train_and_test(const size_t elementCount, const size_t attributeCount, const size_t test_denominator = 0);
   const ID3Node& root() const {return _root;}
   int classify(const size_t element) {return classify(element, _root);}
   /** recalculates the test data in root() */
   void test(const std::forward_list<size_t> &elements);
   virtual ~ID3Classifier() {}
private:
   int classify(const size_t element, const ID3Node &node);
   bool test_classify(const size_t element, ID3Node &node, const int expected_class);
   void train(std::forward_list<size_t> &elements, std::forward_list<size_t> &attributes, ID3Node &parent);
   float entropy_of(const size_t attribute, const std::forward_list<size_t>& elements);
};

}

