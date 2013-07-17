
#pragma once
#include <vector>
#include <array>
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
				
				explicit CVector(const Allocator& alloc = Allocator()) : 
					std::vector<T, Allocator>(alloc)
				{
					
				}
				
				explicit CVector(std::size_t n, const T& value= T(), const Allocator& alloc = Allocator()) :
					std::vector<T, Allocator>(n, value, alloc)
				{
				
				}
		
				template <class InputIterator>
				CVector(InputIterator first, InputIterator last, const Allocator& alloc = Allocator()) :
					std::vector<T, Allocator>(first, last, alloc)
				{
					
				}
				
				CVector(const std::vector<T,Allocator>& x) :
					std::vector<T, Allocator>(x)
				{
					
				}
				
				// you get size by this.size()
				const T *toArray()
				{
					//std::vector<T, Allocator> *vec = this;
					//const T* arr = &((*vec)[0]);
					//return arr;
					return &((*this)[0]);
				}
				
				void append(const T &item)
				{
					// this-> is necessary, otherwise error
					this->reserve(this->size() + sizeof(T));
					this->push_back(item);
				}
				
				void append(const std::vector<T, Allocator> &toAppend)
				{
					// this-> is necessary, otherwise error
					this->reserve(this->size() + toAppend.size());
					this->insert(this->end(), toAppend.begin(), toAppend.end());
				}
				
				void append(const std::string &toAppend)
				{
					// this-> is necessary, otherwise error
					this->reserve(this->size() + toAppend.size());
					this->insert(this->end(), toAppend.begin(), toAppend.end());
				}
				
				void append(const std::wstring &toAppend)
				{
					// this-> is necessary, otherwise error
					this->reserve(this->size() + toAppend.size());
					this->insert(this->end(), toAppend.begin(), toAppend.end());
				}
				
				void prepend(const T &item)
				{
					this->reserve(sizeof(T) + this.size());
					this->insert(this->begin(), item);
				}
				
				void prepend(const std::vector<T, Allocator> &toPrepend)
				{
					this->reserve(toPrepend.size() + this->size());
					this->insert(this->begin(), toPrepend.begin(), toPrepend.end());
				}
				
				void prepend(const std::string &toPrepend)
				{
					this->reserve(toPrepend.size() + this->size());
					this->insert(this->begin(), toPrepend.begin(), toPrepend.end());
				}
				
				void prepend(const std::wstring &toPrepend)
				{
					this->reserve(toPrepend.size() + this->size());
					this->insert(this->begin(), toPrepend.begin(), toPrepend.end());
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
				
                // ===== "SPECIALIZATION" to usigned char ======
				static std::string bytes2string(const CVector<unsigned char> &in)
				{
					return std::string(&in[0], &in[0] + in.size());
				}
				
				/*
				std::string toString()
				{
					std::string str(&this[0], &this[0] + this.size());
					return str;
				}
				* */

				static CVector<unsigned char> array2bytes(const unsigned char *in, int len)
				{
					CVector<unsigned char> ret;
					ret.resize(len);
					copy(in, in + len, ret.begin());
					return ret;
				}
				
				// returns without \0
				static CVector<unsigned char> string2bytes(const char *str)
				{
					CVector<unsigned char> out;
					out.resize(strlen(str)); // important, otherwise SIGSEGV
					copy(str, str + strlen(str), out.begin());
					return out;
				}
				
				static void printBytes(const CVector<unsigned char> &in)
				{
					CVector<unsigned char>::const_iterator from = in.begin();
					CVector<unsigned char>::const_iterator to = in.end();
					for ( ; from!=to; ++from ) printf("%02X", *from);
				}

				static void printBytesAsString(const CVector<unsigned char> &in)
				{
					std::vector<unsigned char>::const_iterator from = in.begin();
					std::vector<unsigned char>::const_iterator to = in.end();
					for ( ; from!=to; ++from ) printf("%c", *from);
				}
				
				static std::string WStringToString(std::wstring &s)
				{
					std::string temp(s.length(), ' ');
					std::copy(s.begin(), s.end(), temp.begin());
					return temp; 
				}
		};
		
		typedef San2::Utils::CVector<unsigned char> bytes;
		
		template <class T, std::size_t N>
		static San2::Utils::CVector<T> arr2vec(const std::array<T, N> &arr) // TODO: why static outside class? typo?
		{
			return San2::Utils::CVector<T> (arr.data(), arr.data() + N);
		}
	}

}
