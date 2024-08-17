#ifndef	STRING_HPP
#define STRING_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>

using namespace std;


class String : public string
{
	public :
		String( void );
		String(const char *str);
		String(const string str);
		String(const String& str);
		~String( void );
		String&	operator=(const String& str);
		String&	trim(const String& characters);
		String&	leftTrim(const String& characters);
		String&	rightTrim(const String& characters);
		vector<String>	split( void );
		vector<String>	split(const char& separators);
		int		countRepeating(const char& c);
		bool	contains(const String& str) const;
		bool	equal(const String& value) const;
		static String	convertVectorToString(const vector<String>& strings, unsigned int begin/* = 0*/, unsigned int len/* = 1*/, char separator/* = 0*/);
		size_t memStringSearchLast(string sstring, size_t lentgh);
		size_t memStringSearch(string sstring, size_t lentgh);
		String memSubstr(size_t begin, size_t end);
		
		template<typename T>
		static String	toString(const T& value)
		{
			stringstream ss;
			ss << value;
			return (ss.str());
		}
};

unsigned int convertStringToBinary(String str);


#endif
