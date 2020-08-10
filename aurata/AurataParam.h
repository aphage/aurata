#pragma once

#include <iostream>

class AurataParam {
public:
	AurataParam() {};
	virtual ~AurataParam() {};
};

class AurataParamInteger : public AurataParam {
public:
	int64_t v;

	AurataParamInteger(int64_t v) :v(v) {}
	virtual ~AurataParamInteger() {}
};

class AurataParamDouble : public AurataParam {
public:
	double v;

	AurataParamDouble(double v) :v(v) {}
	virtual ~AurataParamDouble() {}
};

class AurataParamBlob : public AurataParam {
public:
	std::string v;

	AurataParamBlob(std::string v) :v(v) {}
	virtual ~AurataParamBlob() {}
};

class AurataParamText : public AurataParam {
public:
	std::string v;

	AurataParamText(std::string v) :v(v) {}
	virtual ~AurataParamText() {}
};

class AurataParamNull : public AurataParam {
public:
	AurataParamNull() {}
	virtual ~AurataParamNull() {}
};