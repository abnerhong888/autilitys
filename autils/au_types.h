#ifndef __AU_TYPES_H__
#define __AU_TYPES_H__

#include "au_common.h"

#ifdef USE_AU_TYPES

#ifdef USE_AU_EXCEPTION
#include "au_exception.h"
#endif

#include <string>
#include <initializer_list>
#include <cstring>
#include <cmath>

namespace au {
	namespace types {

#define NAME(_type, _n) _type##_n

#define MEMBER2(_type) struct{_type x, y;}; struct{_type a, b;}; struct{_type w, h;}
#define MEMBER3(_type) struct{_type x, y, z;}; struct{_type r, g, b;}
#define MEMBER4(_type) struct{_type x, y, z, w;}; struct{_type r, g, b, a;}; struct{_type rx, ry, rw, rh;}
#define MEMBERS(_type, _n) MEMBER##_n(_type)

#define CONSTRUCT_EMPTY2(_name) AU_INLINE _name() = default;//noexcept: x(0), y(0) {}
#define CONSTRUCT_EMPTY3(_name) AU_INLINE _name() = default;//noexcept: x(0), y(0), z(0) {}
#define CONSTRUCT_EMPTY4(_name) AU_INLINE _name() = default;//noexcept: x(0), y(0), z(0), w(0) {}
#define CONSTRUCTS_EMPTY(_type, _n) CONSTRUCT_EMPTY##_n(NAME(_type, _n))

#define CONSTRUCT2(_type, _name)  AU_INLINE _name(_type _x, _type _y) noexcept: x(_x), y(_y) {}
#define CONSTRUCT3(_type, _name)  AU_INLINE _name(_type _x, _type _y, _type _z) noexcept: x(_x), y(_y), z(_z) {}
#define CONSTRUCT4(_type, _name)  AU_INLINE _name(_type _x, _type _y, _type _z, _type _w) noexcept: x(_x), y(_y), z(_z), w(_w) {}
#define CONSTRUCTS(_type, _n) CONSTRUCT##_n(_type, NAME(_type, _n))

#define CONSTRUCT_TYPE2(_type, _n)  template<typename T> AU_INLINE NAME(_type, _n)(T _x, T _y) noexcept :\
									x(static_cast<_type>(_x)), y(static_cast<_type>(_y)) {}
#define CONSTRUCT_TYPE3(_type, _n)  template<typename T> AU_INLINE NAME(_type, _n)(T _x, T _y, T _z) noexcept :\
									x(static_cast<_type>(_x)), y(static_cast<_type>(_y)), z(static_cast<_type>(_z)) {}
#define CONSTRUCT_TYPE4(_type, _n)  template<typename T> AU_INLINE NAME(_type, _n)(T _x, T _y, T _z, T _w) noexcept :\
									x(static_cast<_type>(_x)), y(static_cast<_type>(_y)), z(static_cast<_type>(_z)), w(static_cast<_type>(_w)) {}
#define CONSTRUCTS_TYPE(_type, _n) CONSTRUCT_TYPE##_n(_type, _n)

#define CONSTRUCTS_SAME_TYPE(_name) AU_INLINE _name(const _name& in) noexcept { *this = in; }
#define CONSTRUCTS_SAME_TYPE_POINTER(_name) AU_INLINE _name(const _name* in) noexcept {*this = *in;}

#define CONSTRUCTS_DIFF_TYPE(_name) template<typename T, AU_ENABLE_IF_NOT(std::is_pointer<T>)>\
		AU_INLINE _name(const T& in) noexcept { *this = convert<type>(in);}

#define CONSTRUCTS_INITIALIZER(_type, _n) AU_INLINE NAME(_type, _n)(std::initializer_list<_type> list){\
		memset(v, 0, sizeof(_type) * channels);	\
		AU_ASSERT(list.size() <= channels, "Initializer size is more than channels.");		\
		std::copy(list.begin(), list.end(), v);	\
	}

#define OPERATOR_EQUAL(_name) AU_INLINE void operator= (const _name& in) noexcept { memcpy(this, &in, sizeof(_name)); }


#define CHECK_STRCUT_ZERO_0() (void)0;

#define CHECK_STRCUT_ZERO_1()						\
	for (int i = 0; i < channels; i++)				\
		AU_ASSERT(p.v[i] != 0, "Divid by zero.");	\

#define CHECK_STRCUT_ZERO(_OP_DEFINE) CHECK_STRCUT_ZERO_##_OP_DEFINE()

#define CHECK_TYPE_ZERO_0() (void)0;

#define CHECK_TYPE_ZERO_1()				\
	AU_ASSERT(p != 0, "Divid by zero.");	\


#define CHECK_TYPE_ZERO(_OP_DEFINE) CHECK_TYPE_ZERO_##_OP_DEFINE()

#define OP_CHECK_DIVIDE 1
#define OP_CHECK_NONE 0

#define STRUCT_OP(_name, _op, _OP_DEFINE)					\
AU_INLINE _name operator _op  (const _name& p) noexcept {	\
		_name r;											\
		CHECK_STRCUT_ZERO(_OP_DEFINE)						\
		for (int i = 0; i < channels; i++) 					\
			r.v[i] = (v[i] _op p.v[i]);						\
		return r;											\
	}														\

#define STRUCT_OP_EQUAL(_name, _op, _OP_DEFINE)			\
AU_INLINE void operator _op  (const _name& p) noexcept {	\
		CHECK_STRCUT_ZERO(_OP_DEFINE)					\
		for (int i = 0; i < channels; i++) 					\
			(v[i] _op p.v[i]);								\
	}														\

#define TYPE_OP(_type, _n, _op, _OP_DEFINE)											\
template<typename T> AU_INLINE NAME(_type, _n) operator _op (const T p) noexcept {	\
		NAME(_type, _n) r;															\
		CHECK_TYPE_ZERO(_OP_DEFINE)													\
		for (int i = 0; i < channels; i++)											\
			r.v[i] = v[i] _op static_cast<_type>(p);								\
		return r;																	\
	}																				\

#define TYPE_OP_EQUAL(_type, _n, _op, _OP_DEFINE)						\
template<typename T> AU_INLINE void operator _op (const T p) noexcept {	\
		CHECK_TYPE_ZERO(_OP_DEFINE)										\
		for (int i = 0; i < channels; i++) 								\
			(v[i] _op static_cast<_type>(p));							\
	}																	\

#define TO_OSTREAM(_name)\
AU_INLINE friend std::ostream& operator<< (std::ostream& o, const _name & t){		\
		o << ((_name)t).to_string();												\
		return o;																	\
}																					\

#define TO_STRING()\
AU_INLINE std::string to_string(){					\
		std::string str = "(";						\
		for (int i = 0; i < channels; i++)			\
			str += au::to_string(v[i]) + ", ";		\
		str = str.substr(0, str.length() - 2);		\
		str += ")\n";								\
		return str;									\
}													\


#define VALUE_TYPES_STRUCT(_type, _n)							\
struct NAME(_type, _n) {										\
	typedef _type value_type;									\
	typedef NAME(_type, _n) type;								\
																\
	enum{ channels = _n };										\
																\
	union{														\
	MEMBERS(_type, _n);											\
	_type v[_n] = { 0 };										\
	};															\
																\
	CONSTRUCTS_EMPTY(_type, _n)									\
	CONSTRUCTS(_type, _n)										\
	CONSTRUCTS_TYPE(_type, _n)									\
																\
	OPERATOR_EQUAL(NAME(_type, _n))								\
	CONSTRUCTS_SAME_TYPE(NAME(_type, _n))						\
	CONSTRUCTS_DIFF_TYPE(NAME(_type, _n))						\
	CONSTRUCTS_SAME_TYPE_POINTER(NAME(_type, _n))				\
																\
	CONSTRUCTS_INITIALIZER(_type, _n)							\
																\
	STRUCT_OP(NAME(_type, _n), +, OP_CHECK_NONE)				\
	STRUCT_OP(NAME(_type, _n), -, OP_CHECK_NONE)				\
	STRUCT_OP(NAME(_type, _n), *, OP_CHECK_NONE)				\
	STRUCT_OP(NAME(_type, _n), /, OP_CHECK_DIVIDE)				\
	STRUCT_OP_EQUAL(NAME(_type, _n), +=, OP_CHECK_NONE)			\
	STRUCT_OP_EQUAL(NAME(_type, _n), -=, OP_CHECK_NONE)			\
	STRUCT_OP_EQUAL(NAME(_type, _n), *=, OP_CHECK_NONE)			\
	STRUCT_OP_EQUAL(NAME(_type, _n), /=, OP_CHECK_DIVIDE)		\
																\
	TYPE_OP(_type, _n, +, OP_CHECK_NONE)						\
	TYPE_OP(_type, _n, -, OP_CHECK_NONE)						\
	TYPE_OP(_type, _n, *, OP_CHECK_NONE)						\
	TYPE_OP(_type, _n, /, OP_CHECK_DIVIDE)						\
	TYPE_OP_EQUAL(_type, _n, +=, OP_CHECK_NONE)					\
	TYPE_OP_EQUAL(_type, _n, -=, OP_CHECK_NONE)					\
	TYPE_OP_EQUAL(_type, _n, *=, OP_CHECK_NONE)					\
	TYPE_OP_EQUAL(_type, _n, /=, OP_CHECK_DIVIDE)				\
																\
	static std::string name(){ return { typeid(type).name() }; }\
	TO_OSTREAM(NAME(_type, _n))									\
	TO_STRING()													\
};																\
typedef NAME(_type, _n)

		template<typename Dst, typename SRC, 
			AU_ENABLE_IF_NOT(std::is_pointer<SRC>), AU_ENABLE_IF_NOT(std::is_pointer<Dst>)>
		AU_INLINE static Dst convert(const SRC& src) {
			Dst r;
			AU_STATIC_ASSERT(static_cast<int>(Dst::channels) == static_cast<int>(SRC::channels), "Channel size is different.");
			for (int i = 0; i < r.channels; i++)
				r.v[i] = static_cast<typename Dst::value_type>(src.v[i]);
			return r;
		}

		typedef short int sint;
		typedef unsigned int uint;
		typedef unsigned short int usint;
		typedef unsigned char uchar;
		typedef unsigned char byte;

		VALUE_TYPES_STRUCT(int, 2) int2;
		VALUE_TYPES_STRUCT(int, 3) int3;
		VALUE_TYPES_STRUCT(int, 4) int4;
		typedef int4 rect;

		VALUE_TYPES_STRUCT(sint, 2) sint2;
		VALUE_TYPES_STRUCT(sint, 3) sint3;
		VALUE_TYPES_STRUCT(sint, 4) sint4;

		VALUE_TYPES_STRUCT(uint, 2) uint2;
		VALUE_TYPES_STRUCT(uint, 3) uint3;
		VALUE_TYPES_STRUCT(uint, 4) uint4;

		VALUE_TYPES_STRUCT(usint, 2) usint2;
		VALUE_TYPES_STRUCT(usint, 3) usint3;
		VALUE_TYPES_STRUCT(usint, 4) usint4;

		VALUE_TYPES_STRUCT(char, 2) char2;
		VALUE_TYPES_STRUCT(char, 3) char3;
		VALUE_TYPES_STRUCT(char, 4) char4;

		typedef char2 byte2;
		typedef char3 byte3;
		typedef char4 byte4;

		VALUE_TYPES_STRUCT(uchar, 2) uchar2;
		VALUE_TYPES_STRUCT(uchar, 3) uchar3;
		VALUE_TYPES_STRUCT(uchar, 4) uchar4;

		VALUE_TYPES_STRUCT(float, 2) float2;
		VALUE_TYPES_STRUCT(float, 3) float3;
		VALUE_TYPES_STRUCT(float, 4) float4;

		VALUE_TYPES_STRUCT(double, 2) double2;
		VALUE_TYPES_STRUCT(double, 3) double3;
		VALUE_TYPES_STRUCT(double, 4) double4;

		

#undef NAME
#undef MEMBER2
#undef MEMBER3
#undef MEMBER4
#undef MEMBERS
#undef CONSTRUCT_EMPTY2
#undef CONSTRUCT_EMPTY3
#undef CONSTRUCT_EMPTY4
#undef CONSTRUCTS_EMPTY
#undef CONSTRUCT2
#undef CONSTRUCT3
#undef CONSTRUCT4
#undef CONSTRUCTS
#undef CONSTRUCT_TYPE2
#undef CONSTRUCT_TYPE3
#undef CONSTRUCT_TYPE4
#undef CONSTRUCTS_TYPE
#undef CONSTRUCTS_SAME_TYPE
#undef CONSTRUCTS_DIFF_TYPE
#undef CONSTRUCTS_SAME_TYPE_POINTER
#undef CONSTRUCTS_INITIALIZER
#undef OPERATOR_EQUAL
#undef CHECK_STRCUT_ZERO_0
#undef CHECK_STRCUT_ZERO_1
#undef CHECK_STRCUT_ZERO
#undef CHECK_TYPE_ZERO_0
#undef CHECK_TYPE_ZERO_1
#undef CHECK_TYPE_ZERO
#undef STRUCT_OP
#undef STRUCT_OP_EQUAL
#undef OP_CHECK_DIVIDE
#undef OP_CHECK_NONE
#undef TYPE_OP
#undef TYPE_OP_EQUAL
#undef TO_OSTREAM
#undef TO_STRING
#undef VALUE_TYPES_STRUCT


		template<typename T, size_t N>
		struct vec 
		{
			typedef T value_type;
			typedef vec<T, N> type;
			enum
			{
				channels = N,
				cols = N,
				rows = 1
			};

			union 
			{
				T v[N] = { 0 };
			} ;

			AU_INLINE vec() noexcept = default;
			AU_INLINE vec(T v0) noexcept;
			AU_INLINE vec(T v0, T v1) noexcept;
			AU_INLINE vec(T v0, T v1, T v2) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3, T v4) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3, T v4, T v5) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8) noexcept;
			AU_INLINE vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9) noexcept;

			AU_INLINE vec(const vec &in) noexcept;
			AU_INLINE vec(const vec* in) noexcept;

			template<typename T2, size_t N2, AU_ENABLE_IF_NOT(std::is_pointer<T2>)>
			AU_INLINE vec(const vec<T2, N2>& in) noexcept {
				*this = convert<type>(in);
			}
			
			AU_INLINE vec(std::initializer_list<T> list) noexcept;
			AU_INLINE vec(const T arr[N], int size);

			AU_INLINE void operator= (const vec& in) noexcept;


			AU_INLINE vec operator+ (const vec& in) noexcept;
			AU_INLINE vec operator- (const vec& in) noexcept;
			AU_INLINE vec operator* (const vec& in) noexcept;
			AU_INLINE vec operator/ (const vec& in) noexcept;

			AU_INLINE void operator+= (const vec& in) noexcept;
			AU_INLINE void operator-= (const vec& in) noexcept;
			AU_INLINE void operator*= (const vec& in) noexcept;
			AU_INLINE void operator/= (const vec& in) noexcept;

			template<typename T2>
			AU_INLINE vec operator+ (const T2& in) noexcept;
			template<typename T2>
			AU_INLINE vec operator- (const T2& in) noexcept;
			template<typename T2>
			AU_INLINE vec operator* (const T2& in) noexcept;
			template<typename T2>
			AU_INLINE vec operator/ (const T2& in) noexcept;

			template<typename T2>
			AU_INLINE void operator+= (const T2& in) noexcept;
			template<typename T2>
			AU_INLINE void operator-= (const T2& in) noexcept;
			template<typename T2>
			AU_INLINE void operator*= (const T2& in) noexcept;
			template<typename T2>
			AU_INLINE void operator/= (const T2& in) noexcept;

			

			AU_INLINE T dot(const vec& in){
				T sum = (T)0;
				for (int i = 0; i < channels; i++)
					sum += v[i] * in.v[i];
				return sum;
			}

			AU_INLINE vec cross(const vec &in) {
				
				AU_STATIC_ASSERT(channels == 3 && in.channels == 3, "Vector cross product only support 3 element.");
				vec r;
				r.v[0] = v[1] * in.v[2] - v[2] * in.v[1];
				r.v[1] = v[2] * in.v[0] - v[0] * in.v[2];
				r.v[2] = v[0] * in.v[1] - v[1] * in.v[0];

				return r;
			}

			AU_INLINE double length() {
				double sum = 0.0;
				for (int i = 0; i < channels; i++)
					sum += static_cast<double>(v[i]) * static_cast<double>(v[i]);

				return std::sqrt(sum);
			}
			AU_INLINE void normalize() {
				double len = length();
				for (int i = 0; i < channels; i++)
					v[i] = static_cast<T>(v[i] / len);

			}


			AU_INLINE friend std::ostream& operator<< (std::ostream& o, const vec& t){
			o << ((vec)t).to_string();
			return o;
			}

			AU_INLINE std::string to_string() {
				std::string str = "[";
				for (int i = 0; i < channels; i++)
					str += au::to_string(v[i]) + ", ";
				str = str.substr(0, str.length() - 2);
				str += "]\n";
				return str;
}		

			AU_INLINE static std::string name() { return { typeid(type).name() }; }
		};

		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0) noexcept {
			AU_STATIC_ASSERT(channels >= 1, "Channels less than 1.");
			v[0] = v0;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1) noexcept {
			AU_STATIC_ASSERT(channels >= 2, "Channels less than 2.");
			v[0] = v0; v[1] = v1;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2) noexcept {
			AU_STATIC_ASSERT(channels >= 3, "Channels less than 3.");
			v[0] = v0; v[1] = v1; v[2] = v2;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3) noexcept {
			AU_STATIC_ASSERT(channels >= 4, "Channels less than 4.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3, T v4) noexcept {
			AU_STATIC_ASSERT(channels >= 5, "Channels less than 5.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; v[4] = v4;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3, T v4, T v5) noexcept {
			AU_STATIC_ASSERT(channels >= 6, "Channels less than 6.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; v[4] = v4; v[5] = v5;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6) noexcept {
			AU_STATIC_ASSERT(channels >= 7, "Channels less than 7.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; v[4] = v4; v[5] = v5; v[6] = v6;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7) noexcept {
			AU_STATIC_ASSERT(channels >= 8, "Channels less than 8.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; v[4] = v4; v[5] = v5; v[6] = v6; v[7] = v7;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8) noexcept {
			AU_STATIC_ASSERT(channels >= 9, "Channels less than 9.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; v[4] = v4; v[5] = v5; v[6] = v6; v[7] = v7; v[8] = v8;
		}
		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9) noexcept {
			AU_STATIC_ASSERT(channels >= 10, "Channels less than 10.");
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; v[4] = v4; v[5] = v5; v[6] = v6; v[7] = v7; v[8] = v8; v[9] = v9;
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(const vec& in) noexcept{
			*this = in;
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(const vec* in) noexcept {
			*this = *in;
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(const T arr[N], int size)
		{
			memset(v, 0, sizeof(T) * channels);
			AU_ASSERT(arr != nullptr, "Array is null pointer.");
			AU_ASSERT(size <= channels, "Initializer size is more than channels.");
			memcpy(v, arr, sizeof(T) * size);
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N>::vec(std::initializer_list<T> list) noexcept {
			memset(v, 0, sizeof(T) * channels);	
			AU_ASSERT(list.size() <= channels, "Initializer size is more than channels.");		
			std::copy(list.begin(), list.end(), v);	
		}

		template<typename T, size_t N> AU_INLINE
		void vec<T, N>::operator= (const vec& in) noexcept {
			memcpy(this, &in, sizeof(type));
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N> vec<T, N>::operator+ (const vec& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] + in.v[i];
			return r;
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N> vec<T, N>::operator- (const vec& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] - in.v[i];
			return r;
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N> vec<T, N>::operator* (const vec& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] * in.v[i];
			return r;
		}

		template<typename T, size_t N> AU_INLINE
		vec<T, N> vec<T, N>::operator/ (const vec& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
			{
				AU_ASSERT(in.v[i] !=0, "Divide by zero.");
				r.v[i] = v[i] / in.v[i];
			}
			return r;
		}

		template<typename T, size_t N> AU_INLINE
		void  vec<T, N>::operator+= (const vec& in) noexcept {
			for (int i = 0; i < channels; i++)
				v[i] += in.v[i];
		}

		template<typename T, size_t N> AU_INLINE
		void  vec<T, N>::operator-= (const vec& in) noexcept {
			for (int i = 0; i < channels; i++)
				v[i] -= in.v[i];
		}

		template<typename T, size_t N> AU_INLINE
		void vec<T, N>::operator*= (const vec& in) noexcept {
			for (int i = 0; i < channels; i++)
				v[i] *= in.v[i];
		}

		template<typename T, size_t N> AU_INLINE
		void vec<T, N>::operator/= (const vec& in) noexcept {
			for (int i = 0; i < channels; i++)
			{
				AU_ASSERT(in.v[i] != 0, "Divide by zero.");
				v[i] /= in.v[i];
			}
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		vec<T, N> vec<T, N>::operator+ (const T2& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] + static_cast<T>(in);
			return r;
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		vec<T, N> vec<T, N>::operator- (const T2& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] - static_cast<T>(in);
			return r;
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		vec<T, N> vec<T, N>::operator* (const T2& in) noexcept {
			vec<T, N> r;
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] * static_cast<T>(in);
			return r;
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		vec<T, N> vec<T, N>::operator/ (const T2& in) noexcept {
			vec<T, N> r;
			AU_ASSERT(in != 0, "Divide by zero.");
			for (int i = 0; i < channels; i++)
				r.v[i] = v[i] / static_cast<T>(in);
			return r;
		}


		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		void vec<T, N>::operator+= (const T2& in) noexcept {
			for (int i = 0; i < channels; i++)
				v[i] += static_cast<T>(in);
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		void vec<T, N>::operator-= (const T2& in) noexcept {
			for (int i = 0; i < channels; i++)
				v[i] -= static_cast<T>(in);
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		void vec<T, N>::operator*= (const T2& in) noexcept {
			for (int i = 0; i < channels; i++)
				v[i] *= static_cast<T>(in);
		}

		template<typename T, size_t N>
		template<typename T2> AU_INLINE
		void vec<T, N>::operator/= (const T2& in) noexcept {
			AU_ASSERT(in != 0, "Divide by zero.");
			for (int i = 0; i < channels; i++)
				v[i] /= static_cast<T>(in);
		}

		typedef struct vec<int, 2> vec2i;
		typedef struct vec<int, 3> vec3i;
		typedef struct vec<int, 4> vec4i;
		typedef struct vec<int, 5> vec5i;
		typedef struct vec<int, 6> vec6i;
		typedef struct vec<int, 7> vec7i;
		typedef struct vec<int, 8> vec8i;
		typedef struct vec<int, 9> vec9i;
		typedef struct vec<int, 10> vec10i;

		typedef struct vec<float, 2> vec2f;
		typedef struct vec<float, 3> vec3f;
		typedef struct vec<float, 4> vec4f;
		typedef struct vec<float, 5> vec5f;
		typedef struct vec<float, 6> vec6f;
		typedef struct vec<float, 7> vec7f;
		typedef struct vec<float, 8> vec8f;
		typedef struct vec<float, 9> vec9f;
		typedef struct vec<float, 10> vec10f;

		typedef struct vec<double, 2> vec2d;
		typedef struct vec<double, 3> vec3d;
		typedef struct vec<double, 4> vec4d;
		typedef struct vec<double, 5> vec5d;
		typedef struct vec<double, 6> vec6d;
		typedef struct vec<double, 7> vec7d;
		typedef struct vec<double, 8> vec8d;
		typedef struct vec<double, 9> vec9d;
		typedef struct vec<double, 10> vec10d;

	}// namespace types	
}//namespace au

#endif // USE_AU_TYPES

#endif // __AU_TYPES_H__