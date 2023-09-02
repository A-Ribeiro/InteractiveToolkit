#pragma once

#include "../../common.h"

namespace AlgorithmCore
{

	namespace PatternMatch
	{
		static ITK_INLINE int32_t optimized_int_minimum(int32_t a, int32_t b, int32_t c)
		{
#if defined(ITK_SSE2)
			__m128i _a = _mm_set1_epi32(a);
			__m128i _b = _mm_set1_epi32(b);
			__m128i _c = _mm_set1_epi32(c);

			__m128i _min = _mm_min_epi32(_a, _b);
			_min = _mm_min_epi32(_min, _c);

			return _mm_i32_(_min, 0);
#elif defined(ITK_NEON)

			int32x2_t _a = (int32x2_t){a};
			int32x2_t _b = (int32x2_t){b};
			int32x2_t _c = (int32x2_t){c};

			int32x2_t _min = vmin_s32(_a, _b);
			_min = vmin_s32(_min, _c);

			return _min[0];
#else
			int32_t min = (a < b) ? a : b;
			min = (min < c) ? min : c;
			return min;
#endif
		}

		int32_t edit_distance(const char* s, const char* t)
		{
			int32_t s_len = (int32_t)strlen(s);
			int32_t t_len = (int32_t)strlen(t);

			if (s_len < t_len)
				return edit_distance(t, s);

			int32_t result = 0;
			if (strcmp(s, t) == 0)
				return 0;
			else if (s_len == 0)
				return t_len;
			else if (t_len == 0)
				return s_len;

			std::vector<int32_t> row_prev(t_len + 1);
			std::vector<int32_t> row_curr(t_len + 1);

			for (int32_t i = 0; i < row_prev.size(); i++)
				row_prev[i] = i;

			for (int32_t i = 0; i < s_len; i++)
			{
				row_curr[0] = i + 1;
				for (int32_t j = 0; j < t_len; j++)
				{
					//row_prev and row_curr are 1 char longer: using J+1
					int32_t insertions = row_prev[j + 1] + 1;
					int32_t deletions = row_curr[j] + 1;
					int32_t substitutions = row_prev[j] + (s[i] != t[j]);
					row_curr[j + 1] = optimized_int_minimum(insertions, deletions, substitutions);
				}
				memcpy(&row_prev[0], &row_curr[0], sizeof(int32_t) * row_prev.size());
			}

			return row_curr[t_len];
		}
	}
}