#ifndef __CUSTOM_TO_RUBY_H__
#define __CUSTOM_TO_RUBY_H__
 
#include <rice/Object.hpp>
#include <rice/Array.hpp>
#include <rice/Hash.hpp>
#include <rice/String.hpp>
#include <rice/to_from_ruby.hpp>
#include <vector>
#include <map>
#include <iostream>

template<>
std::vector<double> from_ruby< std::vector<double> >(Rice::Object obj);

template<>
std::vector<float> from_ruby< std::vector<float> >(Rice::Object obj);

template<>
std::vector<bool> from_ruby< std::vector<bool> >(Rice::Object obj);

template<>
Rice::Object to_ruby<std::vector<double> >(std::vector<double> const &v);

/*template<>
std::map<std::string, std::vector<double> > from_ruby<std::map<std::string, std::vector<double> > >(Rice::Object obj);*/
 
template<>
Rice::Object to_ruby<std::map<std::string, std::vector<double> > >(
         std::map<std::string, std::vector<double> > const &m);
#endif
