#include "GeneralPattern.hpp"

GeneralPattern::GeneralPattern( void ) : path("/")
{

}

GeneralPattern::GeneralPattern(vector<Data> _data) : data(_data), path("/")
{
}

GeneralPattern::GeneralPattern(const GeneralPattern& copy)
{
	*this = copy;
}


GeneralPattern::~GeneralPattern( void )  throw()
{
}

GeneralPattern&	GeneralPattern::operator=(const GeneralPattern& target)
{
	if (this != &target)
	{
		setData(target.data);
		path = target.path;
	}
	return (*this);
}

GeneralPattern	&GeneralPattern::setData(const vector<Data> &data)
{
	this->data = data;
	return (*this);
}

void	GeneralPattern::addData(const Data& _data)
{
	data.push_back(_data);
}

vector<Data>	GeneralPattern::getData(const String& key) const
{
	vector<Data>	returnData;
	vector<Data>::const_iterator	iterBegin = data.begin();
	vector<Data>::const_iterator	iterEnd = data.end();
	while (iterBegin < iterEnd)
	{
		if (!iterBegin->getKey().compare(key))
			returnData.push_back(*iterBegin);
		iterBegin++;
	}
	return (returnData);
}

const vector<Data>&	GeneralPattern::getAllData( void ) const
{
	return (data);
}



bool	GeneralPattern::isExist(const Data& value)
{
	vector<Data>::const_iterator	iterBegin = data.begin();
	vector<Data>::const_iterator	iterEnd = data.end();
	while (iterBegin < iterEnd)
	{
		if (!iterBegin->getKey().compare(value.getKey()) \
			&& !iterBegin->getValue().compare(value.getValue()))
			return (true);
		iterBegin++;
	}
	return (false);
}

bool	GeneralPattern::empty( void ) const
{
	return (data.empty());
}

String	GeneralPattern::getPath( void ) const
{
	return (path);
}

void	GeneralPattern::printGeneralPattern(const GeneralPattern& model, String &str)
{
	vector<Data>::const_iterator	ibegin = model.getAllData().begin();
	vector<Data>::const_iterator	iend = model.getAllData().end();
	while (ibegin < iend)
	{
		Data::printData(*ibegin, str);
		ibegin++;
	}
}

void	GeneralPattern::printGeneralPattern(const GeneralPattern& model, const String &str)
{
	vector<Data>::const_iterator	ibegin = model.getAllData().begin();
	vector<Data>::const_iterator	iend = model.getAllData().end();
	while (ibegin < iend)
	{
		Data::printData(*ibegin, str);
		ibegin++;
	}
}

void	GeneralPattern::updateData(const Data& _data, size_t pos)
{
	data[pos] = _data;
}

void	GeneralPattern::clearKey(const String & key)
{
	vector<Data>::iterator	iterBegin = data.begin();
	vector<Data>::iterator	iterEnd = data.end();
	long long indx = 0;
	while (iterBegin < iterEnd)
	{
		indx++;
		if (!iterBegin->getKey().compare(key))
		{
			data.erase(iterBegin);
			iterBegin = data.begin() + (indx - 1);
			iterEnd = data.end();
			continue;
		}
		iterBegin++;
	}
}

void	GeneralPattern::setKeyValues(const String &key, const Data& data)
{
	clearKey(key);
	addData(data);
}

void	GeneralPattern::addMultipleData(const vector<Data>& _data)
{
	vector<Data>::const_iterator	iterBegin = _data.begin();
	vector<Data>::const_iterator	iterEnd = _data.end();
	while (iterBegin < iterEnd)
	{
		addData(*iterBegin);
		iterBegin++;
	}
}