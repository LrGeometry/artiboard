#include <iterator>
#include "id3.h"
std::default_random_engine generator;

namespace {
	void fill(std::forward_list<size_t>& list, const size_t count) {
		for (size_t i=0; i<count;i++)
			list.push_front(i);
	}

	void fill_split(std::forward_list<size_t>& listA, std::forward_list<size_t>& listB, const int count, const int denominator) {
		for (int i=0; i<count;i++) {
			if (i%denominator == 0)
				listB.push_front(i);
			else
				listA.push_front(i);
		}
	}

	float log2(const float v) {
		const float lg = std::log(2.0f);
		return std::log(v) / lg;
	}
}

namespace arti {

	void ElementIndexList::fill(const size_t count) {
		::fill(*this,count);
	}

	void ElementIndexList::collect_random_subset(element_index_list_t &result, const size_t count) const {
		const auto s = size();
		CHECK(count < s);
		double p = (count * 1.0)/(s*1.0);
		static std::uniform_real_distribution<double> distribution(0.0,1.0);
		auto it = begin();
		std::set<size_t> rs;
		while (rs.size() != count) {
				if (distribution(generator) <= p) {
					rs.insert(*it);
				}
				it++;
				if (it == end())
					it = begin();
		}
		result.insert_after(result.before_begin(), rs.begin(), rs.end());
	}

	bool ElementIndexList::contains(const size_t e) const {
		return (std::find(begin(),end(),e) != end());
	}

	void ElementIndexList::prepend(const element_index_list_t &elems) {
		insert_after(before_begin(), elems.begin(), elems.end());
	}

	std::ostream& operator<<(std::ostream &os, const ID3Node& v) {
		std::string space;
		for (int i = 0; i < v.level;i++) space += ".";
		os << std::endl << space << v.attribute << "=" << v.value << ":" << v.dominant_class;
		if (!v.childs.empty()) {
			FOR_EACH(c,v.childs) {os<<*c;};
		}
		return os;
	}

  void ID3Node::to_stream(std::ostream& os,ID3NameResolver &r) const {
  	if (is_root()) {
			os << "tree size is " << size() << " with " << pruned_count() << " pruned nodes" 
				<< " and " << leaf_count() << " leaf nodes " << std::endl;
			if (is_tested())
				os << test_count << " elements were tested" << std::endl;	
  		os << "Root";
  	} else {
	  	std::string space;
			for (int i = 0; i < level;i++) space += ".";
			os << std::endl << space << r.attribute_name(attribute) << "=" 
					<< r.value_name(attribute,value) << ":" 
					<< r.class_name(dominant_class) << (pruned()?"!":"");
			if (!is_leaf()) os << "(" << best_entropy << ")";
		}
		if (is_tested()) {
			os << " " << certainty() << "% ";
		}
		if (!childs.empty()) {
			FOR_EACH(c,childs) {c->to_stream(os,r);};
		}
	}

  int ID3Classifier::classify(const size_t element, const ID3Node &node) {
		if (!node.is_leaf()) {
			const int val = value_of(element, node.childs.begin()->attribute);
			FOR_EACH(c, node.childs) 
				if (c->value == val)
					return classify(element, *c);
		}
		return node.dominant_class;
	}

  bool ID3Classifier::test_classify(const size_t element, ID3Node &node, const int expected_class) {
		node.test_count++;
		if (!node.is_leaf()) {
			const int val = value_of(element, node.childs.begin()->attribute);
			FOR_EACH(c, node.childs) 
				if (c->value == val) {
					const bool is_correct = test_classify(element, *c, expected_class);
					if (!is_correct)
						node.test_errors++;
					return is_correct;
				}
		} 
		const bool is_correct = expected_class == node.dominant_class;
		if (!is_correct)
			node.test_errors++;
		return is_correct;
}

	void ID3Classifier::train(const size_t elementCount, const size_t attributeCount) {
		ENSURE(_root.is_leaf(),"classifier has already been trained");
		std::forward_list<size_t> elems,attribs;
		fill(elems,elementCount);
		fill(attribs,attributeCount);
		train(elems,attribs,_root);
	}

  void ID3Classifier::train(std::forward_list<size_t> &elements, const size_t attributeCount) {
		ENSURE(_root.is_leaf(),"classifier has already been trained");
		std::forward_list<size_t> attribs;
		fill(attribs,attributeCount);
		train(elements,attribs,_root);
  }


  void ID3Classifier::train_and_test(const size_t elementCount, const size_t attributeCount, const size_t test_denominator) {
		ENSURE(_root.is_leaf(),"classifier has already been trained");
		std::forward_list<size_t> elems,attribs,test_elems;
		if (test_denominator > 0) {
			fill_split(elems,test_elems,elementCount,test_denominator);
			ENSURE(size_of(test_elems) > 0, "test_denominator identified no test elements");
		} else
			fill(elems,elementCount);
		fill(attribs,attributeCount);
		train(elems,attribs,_root);
		LOG << "Training done for " << size_of(elems);
		if (test_denominator > 0) {
			test(test_elems);
			LOG << "Testing done for " <<  size_of(test_elems);
		} else {
			test(elems);
			LOG << "Testing done for " << size_of(elems);
		}
	}

	void ID3Classifier::train(std::forward_list<size_t> &elements, std::forward_list<size_t> &attributes, ID3Node &parent) {
		ENSURE(size_of(attributes)>0,"there are no attributes to classify");
		if (mo_cut_off_ > 0 && size_of(elements) < mo_cut_off_) return; // minimal object pruning
		// select the best attribute -- that is the one with the lowest entropy
		typedef std::pair<int,float> entropy_t;
		std::vector<entropy_t> entropies(size_of(attributes));
		std::transform(attributes.begin(),attributes.end(),entropies.begin(),[&] (const int a)
			{
				const float e = entropy_of(a,elements);
				return entropy_t(a,e);
			});
//		FOR_EACH(e,entropies) {TRACE << e->first << ":" << e->second;};
		auto selected_it = std::min_element(entropies.begin(),entropies.end(),[] (const entropy_t& a, const entropy_t& b) 
			{return a.second < b.second;});
		const int selected = selected_it->first;
		parent.best_entropy = selected_it->second;
//		if (parent.best_entropy == 0) return;
		// use selected to split elements
		std::map<int,std::forward_list<size_t>> split; // attribute value -> elements
		FOR_EACH(e, elements) split[value_of(*e,selected)].push_front(*e);
		FOR_EACH(s, split) {
			// is there more than one classification
			mapii classes; // class -> count
			FOR_EACH(e, s->second) classes[class_of(*e)]++;
			if (classes.size() == 1) // one classification, create a leaf
				parent.childs.emplace_front(parent.level+1,selected, s->first, classes.begin()->first,true);
			else { // there is more than one classification -- create a non-leaf 
				auto best_class = std::max_element(classes.begin(), classes.end(),[](mapii::const_reference a, mapii::const_reference b) 
					{return a.second < b.second;});
				parent.childs.emplace_front(parent.level+1,selected, s->first,best_class->first,false);
			}
		}	
		// now expand non-leafs
		attributes.remove(selected); // remove processed attribute
		const bool has_attributes = !attributes.empty();
		FOR_EACH(c, parent.childs) {
			if (!c->is_classified_leaf) {
				if (has_attributes)
					train(split[c->value],attributes,*c);
			}
		}
		attributes.push_front(selected); // put attribute back -- parent set remains unchanges
	}

	void ID3Classifier::test(const std::forward_list<size_t> &elements) {
		_root.clear_test_data();
		for (auto e : elements)
			test_classify(e, _root, class_of(e));
	}

 	float ID3Classifier::entropy_of(const size_t attribute, const std::forward_list<size_t>& elements){
 		const float count = size_of(elements);
 		ENSURE(count > 0.0,"elements cannot be empty");
 		float result = 0.0f;
 		mapii values; 
 		std::map<int,mapii> classes;  // attribute value -> class_map
 		for(const auto e : elements) {
 			auto v = value_of(e,attribute);
 			auto c = class_of(e);
			values[v]++;
			classes[v][c]++;
 		} 
 		//TRACE << "attrib " << attribute; 
 		FOR_EACH(p,values) {
 			float ipn = 0.0f;
 			FOR_EACH(c,classes[p->first]) {
 				const float value_count = p->second;
 				const float class_count = c->second;
 				const float pripn = class_count / value_count;
 				const float cipn = -pripn * log2(pripn);
 				ipn += cipn;
 				//TRACE << "value " << p->first << "class " << c->first << " has ipn " << cipn << " and class count " << class_count << " of total" << value_count;
 			} 
 			// TRACE << "value " << p->first << " has ipn " << ipn;
 			const float pr = p->second / count;
 			result += pr * ipn;
 		}
 		// TRACE << result;
 		return result;
 	}
}
