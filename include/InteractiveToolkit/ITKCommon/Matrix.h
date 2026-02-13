#pragma once

#include "../common.h"
#include "../MathCore/MathCore.h"

namespace ITKCommon
{

#if !defined(NDEBUG)
#define MATRIX_THROW_OUT_OF_BOUND_EXCEPTION
#endif

#if defined(MATRIX_THROW_OUT_OF_BOUND_EXCEPTION)

	template <typename T>
	class Matrix;

	template <typename _T>
	class Matrix_Proxy
	{
		int32_t width;
		_T *ptr;
		Matrix_Proxy(int32_t width, _T *ptr)
		{
			this->width = width;
			this->ptr = ptr;
		}

	public:
		_T &operator[](int32_t x)
		{
			if (x < 0 || x >= width)
				throw std::runtime_error("x out of bounds exception.");
			return this->ptr[x];
		}
		const _T &operator[](int32_t x) const
		{
			if (x < 0 || x >= width)
				throw std::runtime_error("x out of bounds exception.");
			return this->ptr[x];
		}
		friend class Matrix<_T>;
	};
#endif

	template <typename T>
	class Matrix
	{

	public:
		MathCore::vec2i size;
		T *array;

		Matrix() : array(nullptr), size(0, 0)
		{
		}

		Matrix(int32_t width, int32_t height) : array(nullptr), size(0, 0)
		{
			setSize(MathCore::vec2i(width, height));
		}

		Matrix(const MathCore::vec2i &size) : array(nullptr), size(0, 0)
		{
			setSize(size);
		}

		// copy constructor and assignment
		Matrix(const Matrix &m) : array(nullptr), size(0, 0)
		{
			setSize(m.size);
			memcpy(array, m.array, size.width * size.height * sizeof(T));
		}
		Matrix& operator=(const Matrix &m)
		{
			setSize(m.size);
			memcpy(array, m.array, size.width * size.height * sizeof(T));
			return *this;
		}

		// rvalue copy constructor and assignment
		Matrix(Matrix &&m)
		{
			array = m.array;
			size = m.size;

			m.array = nullptr;
			m.size = MathCore::vec2i(0);
		}
		Matrix& operator=(Matrix &&m)
		{
			array = m.array;
			size = m.size;

			m.array = nullptr;
			m.size = MathCore::vec2i(0);

			return *this;
		}

#if defined(MATRIX_THROW_OUT_OF_BOUND_EXCEPTION)
		Matrix_Proxy<T> operator[](int32_t y)
		{
			if (y < 0 || y >= size.height)
				throw std::runtime_error("y out of bounds exception.");
			return Matrix_Proxy<T>(size.width, &this->array[y * size.width]);
		}

		const Matrix_Proxy<T> operator[](int32_t y) const
		{
			if (y < 0 || y >= size.height)
				throw std::runtime_error("y out of bounds exception.");
			return Matrix_Proxy<T>(size.width, &this->array[y * size.width]);
		}
#else
		T *operator[](int32_t y)
		{
			return &this->array[y * size.width];
		}

		const T *operator[](int32_t y) const
		{
			return &this->array[y * size.width];
		}
#endif

		~Matrix()
		{
			if (array != nullptr)
			{
				delete[] array;
				array = nullptr;
				this->size = MathCore::vec2i(0);
			}
		}

		void setSize(const MathCore::vec2i &size)
		{
			if (this->size == size)
				return;

			if (size.width <= 0 || size.height <= 0)
				throw std::runtime_error("invalid matrix size.");
			// ITK_ABORT(size.width <= 0 || size.height <= 0, "invalid matrix size: %i %i", size.width, size.height);

			if (array != nullptr)
				delete[] array;

			this->size = size;
			array = new T[size.width * size.height];
		}

		void clear(const T &v)
		{
			for (int32_t y = 0; y < size.height; y++)
			{
				for (int32_t x = 0; x < size.width; x++)
				{
					array[x + y * size.width] = v;
				}
			}
		}
	};
}
