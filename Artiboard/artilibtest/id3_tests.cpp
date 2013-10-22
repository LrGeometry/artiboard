#include <map>
#include <id3.h>
#include <test_util.h>
#include <log.h>

#define TESTDATA id3Data

using namespace arti;

namespace tut {

struct id3Data{
	id3Data() {
	}
};

test_group<id3Data> id3Tests("300 ID3 tests");

typedef std::map<std::string, std::string> attribs_t;
typedef std::string class_t;

struct ElementData {
	attribs_t _attribs;
	class_t _class;
};

std::vector<std::string> attribute_names({"Outlook","Temperature","Humidity","Windy"}); 

struct QuinlanDatabase {
	std::map<int,ElementData> _data;
	std::vector<int> _keys;
	QuinlanDatabase() {
		add(1, std::vector<std::string>({"sunny", "hot", "high", "false"}), "N");
		add(2, std::vector<std::string>({"sunny", "hot", "high", "true"}), "N");
		add(3, std::vector<std::string>({"overcast", "hot", "high", "false"}), "P");
		add(4, std::vector<std::string>({"rain", "mild", "high", "false"}), "P");
		add(5, std::vector<std::string>({"rain", "cool", "normal", "false"}), "P");
		add(6, std::vector<std::string>({"rain", "cool", "normal", "true"}), "N");
		add(7, std::vector<std::string>({"overcast", "cool", "normal", "true"}), "P");
		add(8, std::vector<std::string>({"sunny", "mild", "high", "false"}), "N");
		add(9, std::vector<std::string>({"sunny", "cool", "normal", "false"}), "P");
		add(10, std::vector<std::string>({"rain", "mild", "normal", "false"}), "P");
		add(11, std::vector<std::string>({"sunny", "mild", "normal", "true"}), "P");
		add(12, std::vector<std::string>({"overcast", "mild", "high", "true"}), "P");
		add(13, std::vector<std::string>({"overcast", "hot", "normal", "false"}), "P");
		add(14, std::vector<std::string>({"rain", "mild", "high", "true"}), "N");
	}

	void add(int idx, const std::vector<std::string>& values, const std::string& classV) {
		ElementData v;
		for (int i = 0; i < 4; i++)
			v._attribs[attribute_names[i]] = values[i];
		v._class = classV;
		_data[idx] = v;
		_keys.push_back(idx);
	}
}; 

struct QuinlanClassifier : public Classifier<std::string,int,std::string,std::string> {
	QuinlanDatabase _db;
	
	std::string value_of(const int& e, const std::string &a) override {
		return _db._data[e]._attribs[a];
	};

  std::string class_of(const int &e) override {
  	return _db._data[e]._class;
  };

};

std::ostream& operator<<(std::ostream& os, const QuinlanClassifier::node_t &v) {
   if (v.value().is_leaf()) {
   	os << "<" << v.value().dominant() << ">";
   } else { 
	   os << v.attrib() << v.value() <<  " [";
	   for (auto i = v.children().begin(); i != v.children().end(); i++)
	      os << i->attrib_value() << ":" << *i << ";";
	   os << "]";
	}
	return os;
}


BEGIN(1,"quinlan example classifies correctly") 
	QuinlanClassifier cf;
	auto result = cf.classify(attribute_names.begin(),attribute_names.end(),cf._db._keys.begin(),cf._db._keys.end());
	TRACE << result.attrib() << " = " << result.value();
	ensure_equals("best atribute does not match", result.attrib(),"Outlook");
	ensure_equalsf("IV(Outlook) not correct",result.value().value(),1.57741f);
	ensure_equals(result.children().size(),3);
	TRACE << result;
	ensure_equals(result.size(),7);
END

}

