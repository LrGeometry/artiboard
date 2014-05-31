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
struct ElementData {
	mapii _attribs; // attrib -> value
	int _class;
};

std::vector<std::string> attribute_names({"Outlook","Temperature","Humidity","Windy"}); 

struct QuinlanDatabase : public ID3NameResolver {
	std::map<int,ElementData> _data;
	std::map<int,std::string> _names;
	QuinlanDatabase() {
		add(0, std::vector<std::string>({"sunny", "hot", "high", "false"}), "N");
		add(1, std::vector<std::string>({"sunny", "hot", "high", "true"}), "N");
		add(2, std::vector<std::string>({"overcast", "hot", "high", "false"}), "P");
		add(3, std::vector<std::string>({"rain", "mild", "high", "false"}), "P");
		add(4, std::vector<std::string>({"rain", "cool", "normal", "false"}), "P");
		add(5, std::vector<std::string>({"rain", "cool", "normal", "true"}), "N");
		add(6, std::vector<std::string>({"overcast", "cool", "normal", "true"}), "P");
		add(7, std::vector<std::string>({"sunny", "mild", "high", "false"}), "N");
		add(8, std::vector<std::string>({"sunny", "cool", "normal", "false"}), "P");
		add(9, std::vector<std::string>({"rain", "mild", "normal", "false"}), "P");
		add(10, std::vector<std::string>({"sunny", "mild", "normal", "true"}), "P");
		add(11, std::vector<std::string>({"overcast", "mild", "high", "true"}), "P");
		add(12, std::vector<std::string>({"overcast", "hot", "normal", "false"}), "P");
		add(13, std::vector<std::string>({"rain", "mild", "high", "true"}), "N");
	}		

   const std::string& value_name(const int a, const int v) override {
   		return _names[v];
   }
   const std::string& attribute_name(const int a) override {
   		return attribute_names[a];
   }
   const std::string& class_name(const int c) override {
   		return _names[c];
   }

	void add(int idx, const std::vector<std::string>& values, const std::string& classV) {
		ElementData v;
		for (int i = 0; i < 4; i++) {
			int vv = values[i][0];
			ENSURE(vv > 0,"attrib value not valid");
			_names[vv] = values[i];
			v._attribs[i] = vv;
		}
		v._class = classV[0];
		_names[v._class] = classV;
		_data[idx] = v;
	}
}; 

struct QuinlanClassifier : public ID3Classifier {
	QuinlanDatabase _db;
	
	int value_of(const int e, const int a) override {
		ENSURE(a < 4, "invalid value for a");
		return _db._data[e]._attribs[a];
	};

  int class_of(const int e) override {
  	return _db._data[e]._class;
  };

};

BEGIN(1,"quinlan example classifies correctly") 
	QuinlanClassifier cf;
	cf.train_and_test(cf._db._data.size(), attribute_names.size());
	const auto &result = cf.root();
	TRACE << result;
	result.to_stream(TRACE,cf._db);
	ensure_equalsf("root must be certain", 100.0f, cf.root().certainty());
	ensure("root must have children", !result.is_leaf());
	ensure_equals("best attribute does not match", cf._db.attribute_name(result.childs.begin()->attribute),"Outlook");
	ensure_equalsf("Outlook entropy is not correct",0.693536f,result.best_entropy);
	ensure_equals(result.childs_size(),3);
	ensure_equals(result.size(),8);
	FOR_EACH(k,cf._db._data) {
		const int e = k->first;
		const int expected = k->second._class;
		ensure_equals(expected,cf.classify(e));
	}
END

}

