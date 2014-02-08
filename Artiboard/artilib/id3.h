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
   virtual const std::string& attribute_name(const int a) = 0;
   virtual const std::string& value_name(const int a, const int v) = 0;
   virtual const std::string& class_name(const int c) = 0;
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
   int pruned_count() const {if (pruned()) return 1; else {int r = 0; FOR_EACH(c,childs) r+=c->pruned_count();return r;};}
   int leaf_count() const {if (is_leaf()) return 1; else {int r = 0; FOR_EACH(c,childs) r+=c->leaf_count();return r;};}
   bool is_tested() const {return test_count > -1;}
   float certainty() const {if (test_count <= 0) return 0; else return ((test_count-test_errors)*100)/(test_count*1.0f);}  
   void clear_test_data() {test_count=0;test_errors=0;FOR_EACH(c,childs) c->clear_test_data();}
};

std::ostream& operator<<(std::ostream &os, const ID3Node& v);

/** The classifier produces a ID3 node, and resolve values.
 */
class ID3Classifier {
private: 
   ID3Node _root;
public:   
   const size_t count_cut; 
   ID3Classifier(size_t cc = 0) : count_cut(cc) {}
   /** the value the element has for the given attribute */
   virtual int value_of(const int element, const int attribute) = 0;
   /** the class of the element */
   virtual int class_of(const int element) = 0;
   /** create the classifier root node */
   void train(const int elementCount, const int attributeCount);
   void train_and_test(const int elementCount, const int attributeCount, const int test_denominator = -1);
   const ID3Node& root() const {return _root;}
   int classify(const int element) {return classify(element, _root);}
   void test(const std::forward_list<int> &elements);
private:
   int classify(const int element, const ID3Node &node);
   bool test_classify(const int element, ID3Node &node, const int expected_class);
   void train(std::forward_list<int> &elements, std::forward_list<int> &attributes, ID3Node &parent);
   float entropy_of(const int attribute, const std::forward_list<int>& elements);
};

}

