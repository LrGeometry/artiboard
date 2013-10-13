#include <set>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include "systemex.h"

namespace arti {
/**
* Generalisation Based on Quinlan_1986
*/
template <class attribT, class classT> class Id3Node {
   Id3Node(const attribT& attrib) : _attrib(attrib), _children(), _class(), _is_leaf(false) {}
   const attribT& value() const {return _attrib;}
   std::list<Id3Node>& children() {return _children;}
   const classT& classication() const {return _class;}
   bool is_leaf() const {return _is_leaf;}
   private: 
      bool _is_leaf;
      attribT _attrib;
      classT _class;
      std::list<Id3Node> _children;
};


template <class classT> class EntropyMeasure {
   public:
      std::map<classT,int>& counts() {return _counts;}
      const std::map<classT,int>& counts() const {return _counts;}
      void set_value(float v) {_value = v;}
      const float value() const {return _value;}
   private:
      std::map<classT,int> _counts;
      float _value;
};


template <class pairT> bool is_better(const pairT& a, const pairT& b) {return a.second.value() < b.second.value();}

template <class attribT, class elemT, class classT, class valueT> class Classifier {
public:
   typedef EntropyMeasure<classT> entropy_t;
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

   template <class elemItT> entropy_t entropy_of(elemItT firstE, elemItT lastE) {
      entropy_t result;
      static const float log2 = std::log(2.0f);
      float total = 0;
      for(auto e=firstE; e!=lastE; e++) {
         auto c = class_of(*e);
         result.counts()[c] = result.counts()[c] + 1.0f;
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

   template <class elemItT> entropy_t subset_entropy_of(const attribT& attrib, elemItT firstE, elemItT lastE) {
      entropy_t result;
      float total = 0.0f;
      std::map<valueT,std::list<elemT>> values;
      for(auto e=firstE; e!=lastE; e++) {
         auto value = value_of(*e,attrib);
         values[value].push_front(*e);
         total += 1.0f;
      }
      float r = 0.0f;
      if (values.size() > 1) {
         // all examples do not have the same class
         FOR_EACH(e,values) {
            auto pt = e->second.size() / total;
            auto ht = entropy_of(e->second.begin(),e->second.end()).value();
            r += pt * ht; 
         }
      }
      result.set_value(r);
      return result;
   }

   template <class elemItT> float information_gain_of(const attribT& attrib, elemItT firstE, elemItT lastE) {
      return entropy_of(firstE,lastE).value() - subset_entropy_of(attrib,firstE,lastE).value();
   }

   /** Eliminating those with entropy of zero */
   template <class elemItT, class attribItT> attrib_entropy_list collect_entropies(attribItT firstA, attribItT lastA, 
      elemItT firstE, elemItT lastE) {
      attrib_entropy_list result;
      for(auto a = firstA; a != lastA; a++) {
         auto sse = subset_entropy_of(*a,firstE,lastE);
         if (sse.value() != 0) 
            result.emplace_front(attrib_entropy(*a,sse));   
      }
      return result;
   }

   /** sorted with best entropy values on top, eliminating those with entropy of zero */
   template <class elemItT, class attribItT> attrib_entropy_list collect_entropies_sorted(attribItT firstA, attribItT lastA, 
      elemItT firstE, elemItT lastE) {
      auto result = collect_entropies(firstA,lastA,firstE,lastE);
      result.sort(is_better<attrib_entropy>);
      return result;
   }


};

}

