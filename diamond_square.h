#pragma once
#ifndef __DIAMOND_SQUARE_H__
#define __DIAMOND_SQUARE_H__
#include <cfloat> // FLT_MAX
#include <cmath> // std::nextafter
#include <random>

//control debug output
#if 0
#include <iostream>
#define DS_DEBUG(x,y)	std::cout << "data[" << (x) << "][" << (y) << "]=" << data[(x)][(y)] << std::endl
#else
#define DS_DEBUG(x,y)
#endif

namespace ds
{
	inline static bool isPowerOf2(unsigned int v)
	{
		if (v != 0 && (v&(v - 1)) == 0)
			return true;
		else
			return false;
	}

	inline static unsigned int round_to_next_power2_(unsigned int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v ++;
		return v;
	}

	inline static float clamp(float lo, float hi, float v)
	{
		if (v > hi)
			return hi;
		else if (v < lo)
			return lo;
		else
			return v;
	}

	/*produce [0.0, 1.0]*/
	class Daimond_Square
	{
	public:
		unsigned int size_;
		float RNG_multiplier_;
		
		float ** data{ nullptr };
		Daimond_Square(unsigned int size, float Init_RNG_factor, float RNG_multiplier)
			: gen(rd())
		{
			generate(size, Init_RNG_factor, RNG_multiplier);
		}
		~Daimond_Square()
		{
			release();
		}
		Daimond_Square(const Daimond_Square& other) = delete;
		Daimond_Square(Daimond_Square&& other) noexcept = delete;
		Daimond_Square& operator=(const Daimond_Square& other) = delete;
		Daimond_Square& operator=(Daimond_Square&& other) noexcept = delete;
		void generate(unsigned int size, float Init_RNG_factor, float RNG_multiplier)
		{
			if (size == 0)
				return;

			release();
			alloc(size);
			RNG_multiplier_ = RNG_multiplier;
			//init 4 corners
			std::uniform_real_distribution<float> dis(0.0f, std::nextafter(1.0f, FLT_MAX));
			data[0][0] = dis(gen);
			DS_DEBUG(0, 0);
			data[0][array_size_ - 1] = dis(gen);
			DS_DEBUG(0, array_size_ - 1);
			data[array_size_ - 1][0] = dis(gen);
			DS_DEBUG(array_size_ - 1, 0);
			data[array_size_ - 1][array_size_ - 1] = dis(gen);
			DS_DEBUG(array_size_ - 1, array_size_ - 1);

			diamond_step(0, array_size_ - 1, 0, array_size_ - 1, Init_RNG_factor);
		}

	private:
		std::random_device rd;
		std::mt19937 gen;
		unsigned int array_size_;

		void release() 
		{
			if (data != nullptr)
			{
				for (unsigned int i = 0; i < array_size_; ++i)
				{
					delete[] data[i];
				}
				delete[] data;
			}
		}

		void alloc(unsigned int size)
		{
			size_ = size;
			if (isPowerOf2(size - 1))
				array_size_ = size;
			else
				array_size_ = round_to_next_power2_(size) + 1;
			data = new float *[array_size_];
			for (unsigned int i = 0; i < array_size_; ++i)
			{
				data[i] = new float[array_size_];
			}
		}

		void diamond_step(unsigned int x_start, unsigned int x_end, unsigned int y_start, unsigned int y_end, float RNG_factor)
		{
			unsigned int x_center = x_start + (x_end - x_start) / 2;
			unsigned int y_center = y_start + (y_end - y_start) / 2;

			if (x_center == x_start || y_center == y_start)
				return;

			std::uniform_real_distribution<float> dis(-1.0f, std::nextafter(1.0f, FLT_MAX));
			float avg = (data[x_start][y_start] + data[x_start][y_end] + data[x_end][y_start] + data[x_end][y_end]) / 4.0f;
			data[x_center][y_center] = clamp(0.0f, 1.0f, avg + dis(gen) * RNG_factor);
			DS_DEBUG(x_center, y_center);

			square_step(x_start, x_end, y_start, y_end, RNG_factor);

			diamond_step(x_start, x_center, y_start, y_center, RNG_factor * RNG_multiplier_);
			diamond_step(x_center, x_end, y_start, y_center, RNG_factor * RNG_multiplier_);
			diamond_step(x_start, x_center, y_center, y_end, RNG_factor * RNG_multiplier_);
			diamond_step(x_center, x_end, y_center, y_end, RNG_factor * RNG_multiplier_);
		}

		void square_step(unsigned int x_start, unsigned int x_end, unsigned int y_start, unsigned int y_end, float RNG_factor)
		{
			unsigned int x_center = x_start + (x_end - x_start) / 2;
			unsigned int y_center = y_start + (y_end - y_start) / 2;

			if (x_center == x_start || y_center == y_start)
				return;

			std::uniform_real_distribution<float> dis(-1.0f, std::nextafter(1.0f, FLT_MAX));
			float avg = (data[x_start][y_start] + data[x_center][y_center] + data[x_end][y_start]) / 3.0f;
			data[x_center][y_start] = clamp(0.0f, 1.0f, avg + dis(gen) * RNG_factor);
			DS_DEBUG(x_center, y_start);

			avg = (data[x_start][y_end] + data[x_center][y_center] + data[x_end][y_end]) / 3.0f;
			data[x_center][y_end] = clamp(0.0f, 1.0f, avg + dis(gen) * RNG_factor);
			DS_DEBUG(x_center, y_end);

			avg = (data[x_start][y_start] + data[x_center][y_center] + data[x_start][y_end]) / 3.0f;
			data[x_start][y_center] = clamp(0.0f, 1.0f, avg + dis(gen) * RNG_factor);
			DS_DEBUG(x_start, y_center);

			avg = (data[x_end][y_start] + data[x_center][y_center] + data[x_end][y_end]) / 3.0f;
			data[x_end][y_center] = clamp(0.0f, 1.0f, avg + dis(gen) * RNG_factor);
			DS_DEBUG(x_end, y_center);
		}
	};

	class Plasma_Fractal
	{
	public:
		unsigned int size_;
		float RNG_multiplier_;

		float ** data{ nullptr };
		Plasma_Fractal(unsigned int size, float Init_RNG_factor, float RNG_multiplier)
			: gen(rd())
		{
			generate(size, Init_RNG_factor, RNG_multiplier);
		}
		~Plasma_Fractal()
		{
			release();
		}
		Plasma_Fractal(const Plasma_Fractal& other) = delete;
		Plasma_Fractal(Plasma_Fractal&& other) noexcept = delete;
		Plasma_Fractal& operator=(const Plasma_Fractal& other) = delete;
		Plasma_Fractal& operator=(Plasma_Fractal&& other) noexcept = delete;
		void generate(unsigned int size, float Init_RNG_factor, float RNG_multiplier)
		{
			if (size == 0)
				return;

			release();
			alloc(size);
			RNG_multiplier_ = RNG_multiplier;
			//init 4 corners
			std::uniform_real_distribution<float> dis(0.0f, std::nextafter(1.0f, FLT_MAX));
			data[0][0] = dis(gen);
			DS_DEBUG(0, 0);
			data[0][array_size_ - 1] = dis(gen);
			DS_DEBUG(0, array_size_ - 1);
			data[array_size_ - 1][0] = dis(gen);
			DS_DEBUG(array_size_ - 1, 0);
			data[array_size_ - 1][array_size_ - 1] = dis(gen);
			DS_DEBUG(array_size_ - 1, array_size_ - 1);

			diamond_step(0, array_size_ - 1, 0, array_size_ - 1, Init_RNG_factor);
		}

	private:
		std::random_device rd;
		std::mt19937 gen;
		unsigned int array_size_;

		void release()
		{
			if (data != nullptr)
			{
				for (unsigned int i = 0; i < array_size_; ++i)
				{
					delete[] data[i];
				}
				delete[] data;
			}
		}

		void alloc(unsigned int size)
		{
			size_ = size;
			if (isPowerOf2(size - 1))
				array_size_ = size;
			else
				array_size_ = round_to_next_power2_(size) + 1;
			data = new float *[array_size_];
			for (unsigned int i = 0; i < array_size_; ++i)
			{
				data[i] = new float[array_size_];
			}
		}

		void diamond_step(unsigned int x_start, unsigned int x_end, unsigned int y_start, unsigned int y_end, float RNG_factor)
		{
			unsigned int x_center = x_start + (x_end - x_start) / 2;
			unsigned int y_center = y_start + (y_end - y_start) / 2;

			if (x_center == x_start || y_center == y_start)
				return;

			std::uniform_real_distribution<float> dis(-1.0f, std::nextafter(1.0f, FLT_MAX));
			float avg = (data[x_start][y_start] + data[x_start][y_end] + data[x_end][y_start] + data[x_end][y_end]) / 4.0f;
			data[x_center][y_center] = clamp(0.0f, 1.0f, avg + dis(gen) * RNG_factor);
			DS_DEBUG(x_center, y_center);

			square_step(x_start, x_end, y_start, y_end);

			diamond_step(x_start, x_center, y_start, y_center, RNG_factor * RNG_multiplier_);
			diamond_step(x_center, x_end, y_start, y_center, RNG_factor * RNG_multiplier_);
			diamond_step(x_start, x_center, y_center, y_end, RNG_factor * RNG_multiplier_);
			diamond_step(x_center, x_end, y_center, y_end, RNG_factor * RNG_multiplier_);
		}

		void square_step(unsigned int x_start, unsigned int x_end, unsigned int y_start, unsigned int y_end)
		{
			unsigned int x_center = x_start + (x_end - x_start) / 2;
			unsigned int y_center = y_start + (y_end - y_start) / 2;

			if (x_center == x_start || y_center == y_start)
				return;

			float avg = (data[x_start][y_start] + data[x_end][y_start]) / 2.0f;
			data[x_center][y_start] = clamp(0.0f, 1.0f, avg);
			DS_DEBUG(x_center, y_start);

			avg = (data[x_start][y_end] + data[x_end][y_end]) / 2.0f;
			data[x_center][y_end] = clamp(0.0f, 1.0f, avg);
			DS_DEBUG(x_center, y_end);

			avg = (data[x_start][y_start] + data[x_start][y_end]) / 2.0f;
			data[x_start][y_center] = clamp(0.0f, 1.0f, avg);
			DS_DEBUG(x_start, y_center);

			avg = (data[x_end][y_start] + data[x_end][y_end]) / 2.0f;
			data[x_end][y_center] = clamp(0.0f, 1.0f, avg);
			DS_DEBUG(x_end, y_center);
		}
	};
}	/*namespace ds*/

#endif		/*#ifndef __DIAMOND_SQUARE_H__*/
