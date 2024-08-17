#include "String.hpp"

String::String( void ) : string()
{
}

String::String(const char* str) : string(str)
{
}

String::String(const string str) : string(str)
{
}

String::String(const String& str) : string(str)
{
}

String::~String( void )
{
}

String& String::operator=(const String& str)
{
	string::operator=(str);
	return (*this);
}

bool	String::equal(const String& value) const
{
	if (!compare(value))
		return (true);
	return (false);
}

bool	String::contains(const String& str) const
{
	if (find(str) != npos)
		return (true);
	return (false);
}

String&	String::leftTrim(const String& characters)
{
	if (length() == 0)
		return (*this);
	iterator iterBegin = begin();
	iterator iterEnd = end();

	while (iterBegin < iterEnd)
	{
		if (characters.find(*iterBegin) != npos)
			iterBegin++;
		else
			break ;
	}
	erase(begin(), iterBegin);
	return (*this);
}

String&	String::rightTrim(const String& characters)
{
	if (length() == 0)
		return (*this);
	iterator iterBegin = begin();
	iterator iterEnd = end();

	while (iterBegin < --iterEnd)
	{
		if (characters.find(*iterEnd) != npos)
			erase(iterEnd);
		else
			break ;
	}
	return (*this);
}


String&	String::trim(const String& characters)
{
	if (length() == 0)
		return (*this);
	leftTrim(characters);
	rightTrim(characters);
	return (*this);
}


vector<String> String::split( void )
{
	String tmp(*this);
	iterator	b = tmp.begin();
	iterator	e = tmp.end();
	while (b < e)
	{
		if (*b == ' ' || *b == '\t')
			*b = ' ';
		b++;
	}
	vector<String> vec;
	istringstream str(tmp);
    String s;
    while (getline(str, s, ' '))
	{
		if (s.size() == 0)
			continue ;
        vec.push_back(s);
    }
	return (vec);
}

vector<String> String::split(const char& separator)
{
	vector<String> vec;
	istringstream str(*this);
    String s;
    while (getline(str, s, separator))
	{
		if (s.size() == 0)
			continue ;
        vec.push_back(s);
    }
	return (vec);
}

int	String::countRepeating(const char& c)
{
	int	counter = 0;
	iterator iBeg = begin();
	iterator iEnd = end();
	while (iBeg < iEnd)
	{
		if (*iBeg == c)
			counter++;
		iBeg++;
	}
	return (counter);
}


String	String::convertVectorToString(const vector<String>& strings, unsigned int begin = 0, unsigned int len = 1, char separator = 0)
{
	String dest;
	
	if (len + begin > strings.size())
		len = (unsigned int)strings.size();
	while (begin <= len)
	{
		dest.append(strings[begin]);
		begin++;
		if (separator != 0 && begin <= len)
			dest.append(&separator);
	}
	return (dest);
}

size_t String::memStringSearchLast(string sstring, size_t lentgh)
{	
	for (size_t i = lentgh - 1; i > 0; i--)
	{
		if(sstring[sstring.size() - 1] == (*this)[i])
		{
			size_t j = sstring.size() - 1;
			for (; j > 0 && (*this)[i] == sstring[j]; j-- && i--);
			if(j == 0)
				return i;
		}
	}
	return SIZE_MAX;
}

String String::memSubstr(size_t begin, size_t end)
{
   	String dest;
	for (size_t i = begin; i < end; i++)
		dest.push_back((*this)[i]);
	return dest;
}

size_t String::memStringSearch(string sstring, size_t lentgh)
{
	for (size_t i = 0; i < lentgh; i++)
	{
		if(sstring[0] == (*this)[i])
		{
			size_t j = 0;
			for (; j < sstring.size() && (*this)[i] == sstring[j]; j++ && i++);
			if(j == sstring.size() - 1)
				return (i - (sstring.size() - 1));
		}
	}
	return SIZE_MAX;
}