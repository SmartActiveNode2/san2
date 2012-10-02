
#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <stdlib.h>

namespace San2
{	
	namespace Utils
	{
		template < class T, class Allocator = std::allocator<T> >
		class CVector : public std::vector <T, Allocator>
		{
			public:
				/*
				CVector::CVector()
					: std::vector()
				{
					
				}
				*/
				
				// you get size by this.size()
				const T *toArray()
				{
					return &this[0];
				}
				
				void append(const T &item)
				{
					// this-> is necessary, otherwise error
					reserve(this->size() + 1);
					push_back(item);
				}
				
				void append(const std::vector<T, Allocator> &toAppend)
				{
					// this-> is necessary, otherwise error
					reserve(this->size() + toAppend.size());
					insert(this->end(), toAppend.begin(), toAppend.end());
				}
				
				void append(const std::string &toAppend)
				{
					// this-> is necessary, otherwise error
					reserve(this->size() + toAppend.size());
					insert(this->end(), toAppend.begin(), toAppend.end());
				}
				
				void append(const std::wstring &toAppend)
				{
					// this-> is necessary, otherwise error
					reserve(this->size() + toAppend.size());
					insert(this->end(), toAppend.begin(), toAppend.end());
				}
				
				void prepend(const std::vector<T, Allocator> &toPrepend)
				{
					reserve(toPrepend.size() + this->size());
					insert(this->begin(), toPrepend.begin(), toPrepend.end());
				}
				
				void prepend(const std::string &toPrepend)
				{
					reserve(toPrepend.size() + this->size());
					insert(this->begin(), toPrepend.begin(), toPrepend.end());
				}
				
				void prepend(const std::wstring &toPrepend)
				{
					reserve(toPrepend.size() + this->size());
					insert(this->begin(), toPrepend.begin(), toPrepend.end());
				}
				
				CVector<T, Allocator> & operator+=(const T &rhs) 
				{
					this->append(rhs);
					return *this;
				}
								
				CVector<T, Allocator> & operator+=(const std::vector<T, Allocator> &rhs) 
				{
					this->append(rhs);
					return *this;
				}
				
				CVector<T, Allocator> & operator+=(const std::string &rhs) 
				{
					this->append(rhs);
					return *this;
				}
				
				CVector<T, Allocator> & operator+=(const std::wstring &rhs) 
				{
					this->append(rhs);
					return *this;
				}
				
				// Add this instance's value to other, and return a new instance
				// with the result.
				const CVector<T, Allocator> operator+(const T &other) const 
				{
					CVector<T, Allocator> result = *this;   // Make a copy of myself.  Same as MyClass result(*this);
					result += other;            // Use += to add other to the copy.
					return result;              // All done!
				}	
				
				// Add this instance's value to other, and return a new instance
				// with the result.
				const CVector<T, Allocator> operator+(const std::vector<T, Allocator> &other) const 
				{
					CVector<T, Allocator> result = *this;   // Make a copy of myself.  Same as MyClass result(*this);
					result += other;            // Use += to add other to the copy.
					return result;              // All done!
				}			
				
				// Add this instance's value to other, and return a new instance
				// with the result.
				const CVector<T, Allocator> operator+(const std::string &other) const 
				{
					CVector<T, Allocator> result = *this;   // Make a copy of myself.  Same as MyClass result(*this);
					result += other;            // Use += to add other to the copy.
					return result;              // All done!
				}		
				
							// Add this instance's value to other, and return a new instance
				// with the result.
				const CVector<T, Allocator> operator+(const std::wstring &other) const 
				{
					CVector<T, Allocator> result = *this;   // Make a copy of myself.  Same as MyClass result(*this);
					result += other;            // Use += to add other to the copy.
					return result;              // All done!
				}		
				
				const CVector<T, Allocator>& operator=(const std::string &rhs) 
				{
					if (this == (void*)&rhs) return *this;
					const char *s = rhs.c_str();
					this->resize(rhs.size()); // important, otherwise SIGSEGV
					copy(s, s + rhs.size(), this->begin());
					return *this;
				}
				
				
				const CVector<T, Allocator>& operator=(const std::wstring &rhs) 
				{
					if (this == (void*)&rhs) return *this;
					this->resize(rhs.size()); // important, otherwise SIGSEGV
					copy(rhs.begin(), rhs.end(), this->begin());
					return *this;
				}    
		};
	
		typedef CVector<unsigned char> bytes;
		
		bytes array2bytes(const unsigned char *in, int len)
		{
			bytes ret;
			ret.resize(len);
			copy(in, in + len, ret.begin());
			return ret;
		}
		
		// returns without \0
		bytes string2bytes(const char *str)
		{
			bytes out;
			out.resize(strlen(str)); // important, otherwise SIGSEGV
			copy(str, str + strlen(str), out.begin());
			return out;
		}
		
		void printBytes(const bytes &in)
		{
			bytes::const_iterator from = in.begin();
			bytes::const_iterator to = in.end();
			for ( ; from!=to; ++from ) printf("%02X", *from);
		}

		void printBytesAsString(const bytes &in)
		{
			std::vector<unsigned char>::const_iterator from = in.begin();
			std::vector<unsigned char>::const_iterator to = in.end();
			for ( ; from!=to; ++from ) printf("%c", *from);
		}
		
		std::string WStringToString(std::wstring &s)
		{
			std::string temp(s.length(), ' ');
			std::copy(s.begin(), s.end(), temp.begin());
			return temp; 
		}
	}

}
