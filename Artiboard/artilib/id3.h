#include <set>
#include <algorithm>
#include <functional>
#include <map>
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
   };

   template <class elemItT, class attribItT> void find_classifiers(attribItT firstA, attribItT lastA, 
      elemItT firstE, elemItT lastE, elemItT result) {
      auto pred = [&](elemItT e) {return is_mixed(*e, firstE, lastE);};
      remove_copy_if(firstA, lastA, result, is_mixed);
   }

};

}

