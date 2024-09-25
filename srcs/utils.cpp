#include "utils.hpp"
#include "ServerException.hpp"
#include "Logger.hpp"

#include <climits>
#include <cstdlib>
#include <sstream>
#include <fstream>

namespace ft
{

// Helper function to perform case-insensitive comparison
bool caseInsensitiveFind(const std::string &str, const std::string &substr)
{
    std::string strLower = str;
    std::string substrLower = substr;
    std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
    std::transform(substrLower.begin(), substrLower.end(), substrLower.begin(), ::tolower);
    return strLower.find(substrLower) != std::string::npos;
}

std::string &trim(std::string &str, std::string const &delimiters)
{
	str.erase(0, str.find_first_not_of(delimiters));
	str.erase(str.find_last_not_of(delimiters) + 1);
	return str;
}

template <> std::string toString<bool>(bool const &value)
{
	return value ? "true" : "false";
}

std::vector<std::string> &split(
	std::vector<std::string> &result,
	std::string const		 &str,
	std::string const		 &delimiters
)
{
	size_t start = 0;
	size_t end = str.find_first_of(delimiters);
	while (end != std::string::npos)
	{
		if (end != start)
			result.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find_first_of(delimiters, start);
	}
	if (start < str.length())
		result.push_back(str.substr(start));
	return result;
}

bool isStrOfDigits(std::string const &str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

bool isUShort(std::string const &str)
{
	if (str.length() > 5)
		return false;
	if (!isStrOfDigits(str))
		return false;
	char		 *end;
	unsigned long value = std::strtoul(str.c_str(), &end, 10);
	if (*end != '\0')
		return false;
	return value <= USHRT_MAX;
}

bool strToUShort(std::string const &str, unsigned short &result)
{
	if (str.empty())
		throw ServerException("Empty string passed to stringToULong");

	char		 *end;
	unsigned long value = std::strtoul(str.c_str(), &end, 10);

	if (*end != '\0')
		throw ServerException("Invalid string passed to stringToUint16: %", errno, str);
	if (value == USHRT_MAX || errno == ERANGE)
		throw ServerException("Overflow in stringToUint16: " +  str);
	result = static_cast<unsigned short>(value);
	return true;
}

unsigned short strToUShort(std::string const &str)
{
	if (str.empty())
		throw ServerException("Empty string passed to stringToULong");

	char		 *end;
	unsigned long value = std::strtoul(str.c_str(), &end, 10);

	if (*end != '\0')
		throw ServerException("Invalid string passed to stringToUint16: %", errno, str);
	if (value == USHRT_MAX || errno == ERANGE)
		throw ServerException("Overflow in stringToUint16: " +  str);
	return static_cast<unsigned short>(value);
}

bool isValidIPv4(std::string const &str)
{
	std::istringstream iss(str);
	std::string		   octet;
	int				   num, count = 0;

	while (std::getline(iss, octet, '.'))
	{
		if (octet.empty() || !isStrOfDigits(octet))
			return false;
		std::istringstream octetStream(octet);
		if (!(octetStream >> num) || num < 0 || num > 255)
			return false;
		++count;
	}
	return count == 4;
}

unsigned long stringToULong(std::string const &str)
{
	if (str.empty())
		throw ServerException("Empty string passed to stringToULong");

	char *end;
	unsigned long value = std::strtoul(str.c_str(), &end, 10);

	if (*end != '\0')
		throw ServerException("Invalid string passed to stringToULong: %", errno, str);
	if (value == ULONG_MAX && errno == ERANGE)
		throw ServerException("Overflow in stringToULong: %", errno, str);

	return value;
}

bool isURI(std::string const &str)
{
	if (str.empty())
		return false;
	if (str[0] != '/')
		return false;
	return true;
}

bool isURL(std::string const &str)
{
	if (str.size() < 8)
		return false;
	if (str.substr(0, 7) != "http://" && str.substr(0, 8) != "https://")
		return false;
	if (str.find('.', str.find("://") + 3) == std::string::npos)
		return false;
	if (str.find('.', str.find("www.") + 4) == std::string::npos)
		return false;
	return true;
}

std::map<std::string, std::string> const createMimeTypesMap()
{
    std::map<std::string, std::string> mimeTypes;
    mimeTypes["html"] = "text/html; charset=UTF-8";
    mimeTypes["htm"] = "text/html; charset=UTF-8";
    mimeTypes["css"] = "text/css; charset=UTF-8";
    mimeTypes["js"] = "application/javascript; charset=UTF-8";
    mimeTypes["json"] = "application/json; charset=UTF-8";
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["png"] = "image/png";
    mimeTypes["gif"] = "image/gif";
    mimeTypes["txt"] = "text/plain; charset=UTF-8";
    mimeTypes["xml"] = "application/xml; charset=UTF-8";
    mimeTypes["pdf"] = "application/pdf";
    mimeTypes["zip"] = "application/zip";
    mimeTypes["mp3"] = "audio/mpeg";
    mimeTypes["mp4"] = "video/mp4";
    mimeTypes["avi"] = "video/x-msvideo";
    
    return mimeTypes;
}

std::string getDirectory(const std::string &filepath) {
    std::string::size_type pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return filepath.substr(0, pos);
    }
    return ".";
}

std::string readFile(const std::string &filePath)
{
	std::fstream file(filePath);
	if (!file.is_open())
	{
		Logger::log(Logger::ERROR, true)
			<< "Failed to open file: " << filePath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	if (buffer.str().empty())
	{
		Logger::log(Logger::ERROR, true)
			<< "File is empty or could not be read: " << filePath << std::endl;
	}
	return buffer.str();
}

} // namespace ft
