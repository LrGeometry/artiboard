#include <set>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include "systemex.h"

namespace arti {

// template <class attribT> class Id3Result {

// };

/**
* Data constraints: 
*  1. every example must have a value for every attribute
*  2. every example must have a classification
*  3. all elements with same values for an attribute must 
*     have the same classification (find_classifiers)
*/
template <class attribT, class elemT, class classT, class valueT> class Classifier {
public:
   virtual valueT value_of(const elemT& e, const attribT &a);
   virtual classT class_of(const elemT &e);

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

   template <class elemItT> float entropy_of(elemItT firstE, elemItT lastE) {
      std::map<classT,float> map;
      static const float log2 = std::log(2.0f);
      float total = 0;
      for(auto e=firstE; e!=lastE; e++) {
         auto c = class_of(*e);
         map[c] = map[c] + 1.0f;
         total += 1.0f;
      }
      float r = 0.0f;
      if (map.size() > 1) {
         FOR_EACH(e,map) {
            const float px = e->second / total;
            r += px * std::log(px) / log2;    
         };
      };
      return -r;   
   }

   template <class elemItT> float subset_entropy_of(const attribT& attrib, elemItT firstE, elemItT lastE) {
      std::map<valueT,std::list<elemT>> subsets;
      float total = 0.0f;
      for(auto e=firstE; e!=lastE; e++) {
         auto value = value_of(*e,attrib);
         subsets[value].push_front(*e);
         total += 1.0f;
      }
      float r = 0.0f;
      if (subsets.size() > 1) {
         FOR_EACH(e,subsets) {
            auto pt = e->second.size() / total;
            auto ht = entropy_of(e->second.begin(),e->second.end());
            r = pt * ht; 
         }
      } 
      return r;
   }

    template <class elemItT> float information_gain_of(const attribT& attrib, elemItT firstE, elemItT lastE) {
      return entropy_of(firstE,lastE) - subset_entropy_of(attrib,firstE,lastE);
   }

   template <class elemItT, class attribItT> void find_classifiers(attribItT firstA, attribItT lastA, 
      elemItT firstE, elemItT lastE, elemItT result) {
      auto pred = [&](elemItT e) {return is_mixed(*e, firstE, lastE);};
      remove_copy_if(firstA, lastA, result, is_mixed);
   }

};

}

