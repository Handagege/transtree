#ifndef _DEFTYPE_H
#define _DEFTYPE_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

typedef vector<int32_t> ViContainer;
typedef vector<int32_t>::iterator ViIter;

typedef vector<ViContainer> VViContainer;
typedef vector<ViContainer>::iterator VViIter;

typedef vector<float> VfContainer;
typedef vector<float>::iterator VfIter;

typedef vector<VfContainer> VVfContainer;
typedef vector<VfContainer>::iterator VVfIter;

typedef map<int32_t,string> MisContainer;
typedef map<int32_t,string>::iterator MisIter;

typedef map<int32_t,int32_t> MiiContainer;
typedef map<int32_t,int32_t>::iterator MiiIter;

typedef map<int32_t,MiiContainer> MiMiiContainer;
typedef map<int32_t,MiiContainer>::iterator MiMiiIter;

typedef vector<string> VsContainer;
typedef vector<string>::iterator VsIter;

typedef set<int32_t> SiContainer;
typedef set<int32_t>::iterator SiIter;

#endif
