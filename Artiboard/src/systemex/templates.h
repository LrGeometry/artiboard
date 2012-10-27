#pragma once
#include <iostream>
#include <vector>

// for some (currently unknown to me) reason, this template function
// is not recognised by some cpp files unless I extend the
// std namespace.
namespace std
{
// outputs all items of the vector in a line
	template <typename T>
	ostream& operator <<(ostream &o,const vector<T>& v)
	{
		typename vector<T>::const_iterator it;
		for (it = v.begin(); it != v.end(); it++)
			o <<  *it << ' ';
		o << endl;
		return o;
	}
}

