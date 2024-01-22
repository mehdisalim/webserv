#include "GeneralPattern.hpp"

GeneralPattern::GeneralPattern( void )
{

}

GeneralPattern::GeneralPattern(vector<Data> _data) : data(_data)
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
		setData(target.data);
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
	return ("");
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

void	GeneralPattern::execute( void ) const
{
}

void	GeneralPattern::updateData(const Data& _data, size_t pos)
{
	data[pos] = _data;
	// _data.printData(data[pos], "\t");cclea
}