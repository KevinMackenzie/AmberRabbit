#ifndef QSE_STRINGS_HPP
#define QSE_STRINGS_HPP


#define MAX_DIGITS_IN_INT 12  // max number of digits in an int (-2147483647 = 11 digits, +1 for the '\0')
typedef std::vector<string> StringVec;

// Removes characters up to the first '\n'
extern QSE_API void RemoveFirstLine(std::wstring &src, std::wstring &result);

// Removes leading white space
extern QSE_API void TrimLeft(std::wstring &s);

// Counts the number of lines in a block of text
extern QSE_API int CountLines(const std::wstring &s);

// Does a classic * & ? pattern match on a file name - this is case sensitive!
extern QSE_API BOOL WildcardMatch(const char *pat, const char *str);

// converts a regular string to a wide string
extern QSE_API void StringToWideString(const string& source, std::wstring& outDest);

extern QSE_API std::string ConcatStringLog(std::string str1, std::string str2);
#define ConcatString ConcatStringLog

extern QSE_API HRESULT AnsiToWideCch(WCHAR* dest, const CHAR* src, int charCount);
extern QSE_API HRESULT WideToAnsiCch(CHAR* dest, const WCHAR* src, int charCount);
extern QSE_API HRESULT GenericToAnsiCch(CHAR* dest, const TCHAR* src, int charCount);
extern QSE_API HRESULT GenericToWideCch(WCHAR* dest, const TCHAR* src, int charCount);
extern QSE_API HRESULT AnsiToGenericCch(TCHAR* dest, const CHAR* src, int charCount);
extern QSE_API HRESULT WideToGenericCch(TCHAR* dest, const WCHAR* src, int charCount);

extern QSE_API string ToStr(int num, int base = 10);
extern QSE_API string ToStr(unsigned int num, int base = 10);
extern QSE_API string ToStr(unsigned long num, int base = 10);
extern QSE_API string ToStr(float num);
extern QSE_API string ToStr(double num);
extern QSE_API string ToStr(bool val);
extern QSE_API string ToStr(const glm::vec3& vec);

extern QSE_API string ws2s(const std::wstring& s);
extern QSE_API std::wstring s2ws(const string &s);

// Splits a string by the delimeter into a vector of strings.  For example, say you have the following string:
// string test("one,two,three");
// You could call Split() like this:
// Split(test, outVec, ',');
// outVec will have the following values:
// "one", "two", "three"
void Split(const string& str, StringVec& vec, char delimiter);


#pragma warning(push)
#pragma warning(disable : 4311)

// A hashed string.  It retains the initial (ANSI) string in addition to the hash value for easy reference.

// class HashedString				- Chapter 10, page 274

class HashedString
{
public:
	explicit HashedString( char const * const pIdentString )
		: m_ident( hash_name( pIdentString ) )
	, m_identStr( pIdentString )
	{
	}

	unsigned long getHashValue( void ) const
	{

		return reinterpret_cast<unsigned long>( m_ident );
	}

	const string & getStr() const
	{
		return m_identStr;
	}

	static
	void * hash_name( char const *  pIdentStr );

	bool operator< ( HashedString const & o ) const
	{
		bool r = ( getHashValue() < o.getHashValue() );
		return r;
	}

	bool operator== ( HashedString const & o ) const
	{
		bool r = ( getHashValue() == o.getHashValue() );
		return r;
	}

private:

	// note: m_ident is stored as a void* not an int, so that in
	// the debugger it will show up as hex-values instead of
	// integer values. This is a bit more representative of what
	// we're doing here and makes it easy to allow external code
	// to assign event types as desired.

	void *             m_ident;
	string		   m_identStr;
};
//Remove the warning for warning #4311...
#pragma warning(pop)



#endif