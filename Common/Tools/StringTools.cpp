#include "StringTools.hpp"
#include <Windows.h>
#include <wincrypt.h>
#include <iostream>
#include <fstream>

void StringTools::copyString(char *src, char *dest, size_t size) {
	std::memset(dest, '\0', size);
	std::memcpy(dest, src, size);
}
std::string StringTools::UrlEncode(const std::string& szToEncode)
{
	std::string src = szToEncode;
	char hex[] = "0123456789ABCDEF";
	std::string dst;

	for (size_t i = 0; i < src.size(); ++i)
	{
		unsigned char cc = src[i];
		if (isascii(cc))
		{
			if (cc == ' ')
			{
				dst += "%20";
			}
			else
				dst += cc;
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(src[i]);
			dst += '%';
			dst += hex[c / 16];
			dst += hex[c % 16];
		}
	}
	return dst;
}


std::string StringTools::UrlDecode(const std::string& szToDecode) {
	std::string result;
	int hex = 0;
	for (size_t i = 0; i < szToDecode.length(); ++i)
	{
		switch (szToDecode[i])
		{
		case '+':
			result += ' ';
			break;
		case '%':
			if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
			{
				std::string hexStr = szToDecode.substr(i + 1, 2);
				hex = strtol(hexStr.c_str(), 0, 16);

				if (!((hex >= 48 && hex <= 57) ||	//0-9
					(hex >=97 && hex <= 122) ||	//a-z
					(hex >=65 && hex <= 90) ||	//A-Z
					hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
					|| hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
					|| hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
					))
				{
					result += char(hex);
					i += 2;
				}
				else result += '%';
			}else {
				result += '%';
			}
			break;
		default:
			result += szToDecode[i];
			break;
		}
	}
	return result;
}

std::string StringTools::integerToString(int value) {
	std::stringstream result;

	result << value;
	return result.str();
}

std::string StringTools::stringToMd5(std::string value) {
	HCRYPTPROV CryptProv; 
	HCRYPTHASH CryptHash; 
	BYTE BytesHash[33];//!
	DWORD dwHashLen;
	std::string final;

	if (CryptAcquireContext(&CryptProv, 
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
	{
		if (CryptCreateHash(CryptProv, CALG_MD5, 0, 0, &CryptHash)) 
		{
			if (CryptHashData(CryptHash, (BYTE*)value.c_str(), value.length(), 0))
			{
				if (CryptGetHashParam(CryptHash, HP_HASHVAL, BytesHash, &dwHashLen, 0)) 
				{
					final.clear();
					std::string hexcharset = "0123456789ABCDEF";
					for(int j = 0; j < 16; j++)
					{
						final += hexcharset.substr(((BytesHash[j] >> 4) & 0xF),1);
						final += hexcharset.substr(((BytesHash[j]) & 0x0F),1);
					}
				}
			}
		}
	}

	CryptDestroyHash(CryptHash); 
	CryptReleaseContext(CryptProv, 0); 
	return final; 
}