#include "custom_from_to_ruby.h"
template<>
std::vector<double> from_ruby< std::vector<double> >(Rice::Object obj) {
  Rice::Array arr(obj);
  std::vector<double> vec;
	vec.reserve(arr.size());
	for (Rice::Array::iterator it = arr.begin(); it != arr.end(); ++it) {
		vec.push_back(from_ruby<int>(*it));
	}

  return vec;
}

template<>
std::vector<float> from_ruby< std::vector<float> >(Rice::Object obj) {
  Rice::Array arr(obj);
  std::vector<float> vec;

  for (size_t i=0;i<arr.size();++i) {
      vec.push_back( from_ruby<float>( arr[i] ) );
  }

  return vec;
}

template<>
std::vector<bool> from_ruby< std::vector<bool> >(Rice::Object obj) {
  Rice::Array arr(obj);
  std::vector<bool> vec;

  for (size_t i=0;i<arr.size();++i) {
      vec.push_back( from_ruby<bool>( arr[i] ) );
  }

  return vec;
}

template<>
Rice::Object to_ruby<std::vector<double> >(std::vector<double> const &x) {
	return Rice::Array(x.begin(), x.end());
}

/*template<>
std::map<std::string, std::vector<double> > from_ruby<std::map<std::string, 
    std::vector<double> > >(Rice::Object obj) {
    Rice::Hash h(obj);
    std::map<std::string, std::vector<double> > answer;

    Rice::Hash::const_iterator iter;

    for (iter = h.begin(); iter !=h.end(); ++iter) {
        answer[(*iter).first] = (*iter).second;
    }
 
    return answer;
}*/

template<>
Rice::Object to_ruby<std::map<std::string, std::vector<double> > >(
         std::map<std::string, std::vector<double> > const &m) {

    Rice::Hash h;
    std::map<std::string, std::vector<double> >::const_iterator iter;
    
    for (iter = m.begin(); iter !=m.end(); ++iter) {
        h[Rice::Symbol(iter->first)] = iter->second;
    }
    return h;
}
