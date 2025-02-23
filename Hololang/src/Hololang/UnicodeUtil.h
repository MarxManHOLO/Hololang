#ifndef UNICODEUTIL_H
#define UNICODEUTIL_H

namespace Hololang {	

	
	inline bool IsUTF8IntraChar(const unsigned char charByte)
	{
		
		return 0x80 == (charByte & 0xC0);
	}

	
	void AdvanceUTF8(unsigned char **c, const unsigned char *maxc, int count);

	
	void BackupUTF8(unsigned char **c, const unsigned char *minc, int count);

	
	long UTF8Encode(unsigned long uniChar, unsigned char *outBuf);

	
	unsigned long UTF8Decode(unsigned char *inBuf);

	
	unsigned long UTF8DecodeAndAdvance(unsigned char **inBuf);

	

	unsigned long UnicodeCharToUpper(unsigned long low);
	unsigned long UnicodeCharToLower(unsigned long upper);
	void UTF8ToUpper(unsigned char *utf8String, unsigned long byteCount,
					 unsigned char **outBuf, unsigned long *outByteCount);
	void UTF8ToLower(unsigned char *utf8String, unsigned long byteCount,
					 unsigned char **outBuf, unsigned long *outByteCount);
	void UTF8Capitalize(unsigned char *utf8String, unsigned long byteCount,
					 unsigned char **outBuf, unsigned long *outByteCount);
	bool UTF8IsCaseless(unsigned char *utf8String, unsigned long byteCount);				
	unsigned short *UCS2ToUpper(unsigned short *ucs2String, unsigned long byteCount);
	unsigned short *UCS2ToLower(unsigned short *ucs2String, unsigned long byteCount);
	unsigned short *UCS2Capitalize(unsigned short *ucs2String, unsigned long byteCount);

	
	inline long UnicodeCharCompare(unsigned long leftChar, unsigned long rightChar,
								   bool ignoreCase=false)
	{
		if (leftChar == rightChar) return 0;
		if (ignoreCase) {
			leftChar = UnicodeCharToUpper( leftChar );
			rightChar = UnicodeCharToUpper( rightChar );
		}
		if (leftChar < rightChar) return -1;
		if (leftChar > rightChar) return 1;
		return 0;
	}

	// String comparisons

	long UTF8StringCompare(unsigned char *leftBuf, unsigned long leftByteCount,
						   unsigned char *rightBuf, unsigned long rightByteCount,
						   bool ignoreCase=false);

	long UCS2StringCompare(unsigned short *leftBuf, unsigned long leftByteCount,
						   unsigned short *rightBuf, unsigned long rightByteCount,
						   bool ignoreCase=false);

	
	inline bool UnicodeCharIsWhitespace(unsigned long uniChar)
	{

		// From the list of whitespace on the Unicode webpage, found
		// at: <http://www.unicode.org/Public/UNIDATA/PropList.txt>
		//
		// 0009..000D    ; White_Space # Cc   [5] <control-0009>..<control-000D>
		// 0020          ; White_Space # Zs       SPACE
		// 0085          ; White_Space # Cc       <control-0085>
		// 00A0          ; White_Space # Zs       NO-BREAK SPACE
		// 1680          ; White_Space # Zs       OGHAM SPACE MARK
		// 180E          ; White_Space # Zs       MONGOLIAN VOWEL SEPARATOR
		// 2000..200A    ; White_Space # Zs  [11] EN QUAD..HAIR SPACE
		// 2028          ; White_Space # Zl       LINE SEPARATOR
		// 2029          ; White_Space # Zp       PARAGRAPH SEPARATOR
		// 202F          ; White_Space # Zs       NARROW NO-BREAK SPACE
		// 205F          ; White_Space # Zs       MEDIUM MATHEMATICAL SPACE
		// 3000          ; White_Space # Zs       IDEOGRAPHIC SPACE
		
		return ((uniChar >= 0x9 && uniChar <= 0xD) ||
			(uniChar >= 0x2000 && uniChar <= 0x200A) ||
			uniChar == 0x20 || uniChar == 0x85 ||
			uniChar == 0xA0 || uniChar == 0x1680 ||
			uniChar == 0x180E || uniChar == 0x2028 ||
			uniChar == 0x2029 || uniChar == 0x202F ||
			uniChar == 0x205F || uniChar == 0x3000);		
	}
}
	
#endif 
