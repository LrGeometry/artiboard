#include <set>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <memory>
#include <log.h>
#include <forward_list>
#include <iterator>
#include "systemex.h"

namespace arti {

template <class T> std::size_t size_of(const std::forward_list<T>& list) {
   return std::distance(list.begin(),list.end());
}

/** This classifier is an implementation of Quinlan's ID3 algorithm.
In order to use it you have to index your attributes, elements attribute values 
classes as ints. Attributes and element indexes are zero based and in sequence
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

/**
* Generalisation Based on Quinlan_1986
*/

template <class valueT, class classT> class EntropyMeasure {
   public:
      typedef std::map<valueT,int> count_map_t;
      typedef std::map<classT,int> class_map_t;
      class_map_t& classes() {return _classes;}
      const class_map_t& classes() const {return _classes;}
      count_map_t& counts() {return _counts;}
      const count_map_t& counts() const {return _counts;}
      void set_value(float v) {_value = v;}
      const float value() const {return _value;}
      void merge_counts(const count_map_t& o) {
         FOR_EACH(e,o)
            _counts[e->first] = _counts[e->first] + e->second;
      }
      bool is_leaf() const {return _classes.size() < 2;}
      const classT& dominant() const {
         ENSURE(_classes.size() > 0,"_classes is empty");
         auto result = _classes.begin();
         FOR_EACH(e,_classes) {
            if (e->second > result->second)
               result = e;
         }
         return result->first;   
      }
   private:
      count_map_t _counts;
      class_map_t _classes;
      float _value;
};


template <class attribT, class valueT, class classT> class Id3Node {
   public:
      typedef EntropyMeasure<valueT,classT> measure_t;
      enum class Id3NodeType {Root, Normal, Leaf};
      Id3Node(attribT * attrib, const measure_t& v) 
         : _attrib(attrib), _value(v),  _type(Id3NodeType::Root)  {}
      Id3Node(attribT * attrib, const valueT& av, const measure_t& v) 
         : _attrib(attrib), _value(v), _type(Id3NodeType::Normal), _attrib_value(av) {}
      Id3Node(const valueT& av) 
         :  _type(Id3NodeType::Leaf), _attrib_value(av), _value() {}

      const attribT& attrib() const {ENSURE(_attrib,"no attribute");return *_attrib;}
      const measure_t& value() const {return _value;} 
      std::list<Id3Node>& children() {return _children;}
      const std::list<Id3Node>& children() const {return _children;}
      const bool is_root() const {return Id3NodeType::Root == _type;}
      const valueT& attrib_value() const {return _attrib_value;}
      const int size() const {int r = 1;for (auto c = _children.begin(); c != _children.end(); c++) r += c->size();return r;}
      bool is_leaf() const {return _value.is_leaf();}
      classT clazz() const {return _value.dominant();}
   private:
      const std::unique_ptr<attribT> _attrib;
      const measure_t _value;
      std::list<Id3Node> _children;
      const Id3NodeType _type; 
      const valueT _attrib_value;
};


template <class T, class U> std::ostream& operator<<(std::ostream& os, const EntropyMeasure<T,U>& v) {
   os << v.value() << " {";
   for (auto i = v.counts().begin(); i != v.counts().end(); i++)
      os << " " << i->first << "=" << i->second;
   os << " }"  << " ";
   os << v.value() << " {";
   for (auto i = v.classes().begin(); i != v.classes().end(); i++)
      os << " " << i->first << "=" << i->second;
   os << " } "  << v.dominant();
   return os;
};

template <class pairT> bool is_better(const pairT& a, const pairT& b) {return a.second.value() < b.second.value();}

template <class attribT, class elemT, class classT, class valueT> class Classifier {
public:
   typedef EntropyMeasure<valueT,classT> entropy_t;
   typedef Id3Node<attribT,valueT, classT> node_t; 
   typedef std::pair<attribT,entropy_t> attrib_entropy;
   typedef std::list<attrib_entropy> attrib_entropy_list;
   virtual valueT value_of(const elemT& e, const attribT &a) = 0;
   virtual classT class_of(const elemT &e) = 0;

   template <class elemItT> bool is_mixed(const attribT& a, 
      elemItT firstE, elemItT lastE) {
      std::map<valueT,classT> map;
      for(auto e=firstE; e != lastE; e++) {
         auto v = value_of(*e,a);
         auto c = class_of(*e);
         auto mv = map.find(v);
         if (mv == map.end())
            map[v] = c;
         else if (! (v == c))
            return true;
      }; 
      return false;
   }

   template <class elemItT> entropy_t entropy_of(const attribT& a, elemItT firstE, elemItT lastE) {
      entropy_t result;
      static const float log2 = std::log(2.0f);
      float total = 0;
      for(auto e=firstE; e!=lastE; e++) {
         auto value = value_of(*e,a);
         result.counts()[value] = result.counts()[value] + 1.0f;
         auto classv = class_of(*e);
         result.classes()[classv] = result.classes()[classv] + 1.0f;
         total += 1.0f;
      }
      float r = 0.0f;
      if (result.counts().size() > 1) {
         FOR_EACH(e,result.counts()) {
            const float px = e->second / total;
            r += px * std::log(px) / log2;    
         };
         // r /= result.counts().size();
      };
      result.set_value(-r);
      return result;   
   }

   /** Eliminating those with entropy of zero */
   template <class elemItT, class attribItT, class entropC> void collect_entropies(const attribItT firstA, const attribItT lastA, 
      const elemItT firstE, const elemItT lastE, entropC& result) {
      for(auto a = firstA; a != lastA; a++) {
         auto sse = entropy_of(*a,firstE,lastE);
         if (sse.value() != 0) {
            result.push_front(attrib_entropy(*a,sse)); 
         }
      }
   }

   template <class elemItT, class entropC> void collect_entropies(const attrib_entropy_list& ptrops, 
      const elemItT firstE, const elemItT lastE, entropC& result) {
      for(auto a = ptrops.begin(); a != ptrops.end(); a++) {
         auto sse = entropy_of(a->first,firstE,lastE);
         if (sse.value() != 0) {
            result.push_front(attrib_entropy(a->first,sse)); 
         }
      }
   }

   /** sorted with best entropy values on top, eliminating those with entropy of zero */
   template <class elemItT, class attribItT> attrib_entropy_list collect_entropies_sorted(attribItT firstA, attribItT lastA, 
      elemItT firstE, elemItT lastE) {
      attrib_entropy_list result;
      collect_entropies(firstA,lastA,firstE,lastE, result);
      result.sort(is_better<attrib_entropy>);
      return result;
   }

   template <class elemItT,class attribItT> const node_t& train(const attribItT firstA, const attribItT lastA, const elemItT firstE, const elemItT lastE) {
      attrib_entropy_list entrops;
      collect_entropies(firstA,lastA,firstE,lastE, entrops);
      ENSURE(entrops.size() > 0, "classification is not possible");
      const auto selected = std::max_element(entrops.begin(), entrops.end(), is_better<attrib_entropy>);
      _root.reset(new node_t(new attribT(selected->first), selected->second));
      train_down(entrops,firstE,lastE,*_root);
      return *_root;
   }

   classT classify(const elemT& e) {
      ENSURE(_root,"classifier has not been trained");
      return classify_down(*_root, e);
   }

private:
   classT classify_down(const node_t& node, const elemT& e) {
      if (node.is_leaf())
         return node.clazz();
      FOR_EACH(c,node.children()) {
         TRACE << e;
         TRACE << node.attrib();
         TRACE << value_of(e,node.attrib());
         TRACE << c->attrib_value();
         if (value_of(e,node.attrib()) == c->attrib_value())
            return classify_down(*c,e);
      }
      throw std::runtime_error("could not classify element");
   }

   template <class elemItT> void train_down(const attrib_entropy_list & ptrops,const elemItT firstE, const elemItT lastE, node_t& parent) {
      if (parent.is_leaf())
         return;
      for (auto v = parent.value().counts().begin(); v != parent.value().counts().end(); v++) {
         std::list<elemT> filtered;
         for(auto e=firstE; e!=lastE; e++) 
            if (value_of(*e,parent.attrib()) == v->first)
               filtered.emplace_front(*e);
         ENSURE(filtered.size() > 0,"no elements match");   
         attrib_entropy_list entrops;
         collect_entropies(ptrops,filtered.begin(),filtered.end(),entrops);
         if (entrops.size() > 0) {
            const auto selected = std::max_element(entrops.begin(), entrops.end(), is_better<attrib_entropy>);
            parent.children().emplace_front(new attribT(selected->first),v->first, selected->second);
            train_down(entrops,filtered.begin(),filtered.end(),*parent.children().begin());
         } else
            parent.children().emplace_front(v->first);
      }
   }
private:
   std::unique_ptr<node_t> _root; 
};

}

