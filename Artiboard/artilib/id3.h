#include <set>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <log.h>
#include "systemex.h"

namespace arti {
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
      Id3Node(const attribT& attrib, const measure_t& v) 
         : _attrib(attrib), _value(v), _children(), _is_root(true), _attrib_value() {}
      Id3Node(const attribT& attrib, const valueT& av, const measure_t& v) 
         : _attrib(attrib), _value(v), _children(), _is_root(false), _attrib_value(av) {}
      const attribT& attrib() const {return _attrib;}
      const measure_t& value() const {return _value;} 
      std::list<Id3Node>& children() {return _children;}
      const std::list<Id3Node>& children() const {return _children;}
      const bool is_root() const {return _is_root;}
      const valueT& attrib_value() const {return _attrib_value;}
      const int size() const {int r = 1;for (auto c = _children.begin(); c != _children.end(); c++) r += c->size();return r;}
   private:
      const attribT _attrib;
      const measure_t _value;
      std::list<Id3Node> _children;
      const bool _is_root; 
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

   /** sorted with best entropy values on top, eliminating those with entropy of zero */
   template <class elemItT, class attribItT> attrib_entropy_list collect_entropies_sorted(attribItT firstA, attribItT lastA, 
      elemItT firstE, elemItT lastE) {
      attrib_entropy_list result;
      collect_entropies(firstA,lastA,firstE,lastE, result);
      result.sort(is_better<attrib_entropy>);
      return result;
   }

   template <class elemItT, class attribItT> node_t classify(const attribItT firstA, const attribItT lastA, const elemItT firstE, const elemItT lastE) {
      attrib_entropy_list entrops;
      collect_entropies(firstA,lastA,firstE,lastE, entrops);
      ENSURE(entrops.size() > 0, "classification is not possible");
      const auto selected = std::max_element(entrops.begin(), entrops.end(), is_better<attrib_entropy>);
      node_t root(selected->first, selected->second);
      classify_down(firstA,lastA,firstE,lastE,root);
      return root;
   }
private:
   template <class elemItT, class attribItT> void classify_down(const attribItT firstA, const attribItT lastA,const elemItT firstE, const elemItT lastE, node_t& parent) {
      for (auto v = parent.value().counts().begin(); v != parent.value().counts().end(); v++) {
         std::list<elemT> filtered;
         for(auto e=firstE; e!=lastE; e++) 
            if (value_of(*e,parent.attrib()) == v->first)
               filtered.emplace_front(*e);
         attrib_entropy_list entrops;
         collect_entropies(firstA,lastA,filtered.begin(),filtered.end(),entrops);
         if (entrops.size() > 1) {
            const auto selected = std::max_element(entrops.begin(), entrops.end(), is_better<attrib_entropy>);
            node_t child(selected->first,v->first, selected->second); 
            classify_down(firstA,lastA,filtered.begin(),filtered.end(),child);
            parent.children().push_back(child);
         } 
      }
   }

};

}

