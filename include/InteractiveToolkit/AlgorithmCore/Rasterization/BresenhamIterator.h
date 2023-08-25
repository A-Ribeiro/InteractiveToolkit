#pragma once

#include "../../common.h"
#include "../../MathCore/MathCore.h"

namespace AlgorithmCore
{

    namespace Rasterization
    {

        /// \brief Classical line iterator algorithm
        ///
        /// Generate a raster of a line specified by two points.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// bool matrix[height][width];
        /// int x, y, xd yd;
        ///
        /// BresenhamIterator it(x, y, xd, yd);
        ///
        /// it.getXY(&x, &y);
        /// matrix[y][x] = true;
        /// while (it.next(&x, &y)) {
        ///     matrix[y][x] = true;
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        /// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
        class BresenhamIterator
        {

            int dx, dy, sx, sy, error;
            int x0, y0, x1, y1;
            int count, steps;

        public:
            /// \brief Initialize the Bresenham algorithm state
            ///
            /// It is needed to specify the source and target point as integers.
            ///
            /// There are several algorithms to rasterize lines, but the Bresenham
            /// is eficient and it dont uses floating points to compute the intermediate
            /// points to make the line.
            ///
            /// \author Alessandro Ribeiro
            /// \param x origin x
            /// \param y origin y
            /// \param xd target x
            /// \param yd target y
            ///
            ITK_INLINE BresenhamIterator(int _x0, int _y0, int _x1, int _y1)
            {
                x0 = _x0;
                y0 = _y0;
                x1 = _x1;
                y1 = _y1;

                dx = x1 - x0;
                sx = MathCore::OP<int>::sign(dx);
                dx *= sx;

                dy = y1 - y0;
                sy = MathCore::OP<int>::sign(dy);
                dy *= -sy;

                // if (x0 < x1)
                // {
                //     sx = 1;
                //     dx = x1 - x0;
                // }
                // else
                // {
                //     sx = -1;
                //     dx = x0 - x1;
                // }
                // if (y0 < y1)
                // {
                //     sy = 1;
                //     dy = y0 - y1;
                // }
                // else
                // {
                //     sy = -1;
                //     dy = y1 - y0;
                // }

                error = dx + dy;

                count = 0;

                // steps = MathCore::OP<int>::maximum(MathCore::OP<int>::abs(dx), MathCore::OP<int>::abs(dy));
                steps = MathCore::OP<int>::maximum(dx, -dy);
            }
            /// \brief Returns true until the algorithm reachs the target point
            /// \author Alessandro Ribeiro
            /// \return True if the target point is reached by the iteration
            ///
            ITK_INLINE int next()
            {
                int r = count - steps;
                if (r == 0)
                    return 0;

                count += 1;

                int e2 = error << 1;
                int e2_greater_equal_dy_mask = -MathCore::OP<int>::step_cmp_greater_equal(e2, dy);
                error += dy & e2_greater_equal_dy_mask;
                x0 += sx & e2_greater_equal_dy_mask;

                int e2_less_equal_dx_mask = -MathCore::OP<int>::step_cmp_less_equal(e2, dx);
                error += dx & e2_less_equal_dx_mask;
                y0 += sy & e2_less_equal_dx_mask;

                return r;

                // if (x0 == x1 && y0 == y1)
                //    return false;

                // // int e2 = 2 * error;
                // int e2 = error << 1;

                // bool r = false;
                // if (e2 >= dy)
                // {
                //     if (x0 == x1)
                //         return false;
                //     error = error + dy;
                //     x0 = x0 + sx;
                //     r = true;
                // }
                // if (e2 <= dx)
                // {
                //     if (y0 == y1)
                //         return false;
                //     error = error + dx;
                //     y0 = y0 + sy;
                //     r = true;
                // }

                // return r;
            }
            /// \brief Reads the current point of the rasterization state
            /// \author Alessandro Ribeiro
            /// \param x Returns the current x of the iteration
            /// \param y Returns the current y of the iteration
            ///
            ITK_INLINE void getXY(int *x, int *y)
            {
                *x = x0;
                *y = y0;
            }

            /// \brief Returns true until the algorithm reachs the target point.
            /// Reads the current point of the rasterization state
            /// \author Alessandro Ribeiro
            /// \param x Returns the current x of the iteration
            /// \param y Returns the current y of the iteration
            /// \return True if the target point is reached by the iteration
            ///
            ITK_INLINE int next(int *x, int *y)
            {
                int retorno = next();
                *x = x0;
                *y = y0;
                return retorno;
            }

            template <typename T>
            static ITK_INLINE void Raster(T &draw_pt, int x0, int y0, int x1, int y1) noexcept
            {

                int dx, dy, sx, sy, error;
                int steps;

                dx = x1 - x0;
                sx = MathCore::OP<int>::sign(dx);
                dx *= sx;

                dy = y1 - y0;
                sy = MathCore::OP<int>::sign(dy);
                dy *= -sy;

                error = dx + dy;

                // count = 0;
                // steps = MathCore::OP<int>::maximum(MathCore::OP<int>::abs(dx), MathCore::OP<int>::abs(dy));
                steps = MathCore::OP<int>::maximum(dx, -dy);

                draw_pt.draw_pixel(x0, y0);
                for (int count = 0; count < steps; count++)
                {
                    int e2 = error << 1;
                    int e2_greater_equal_dy_mask = -MathCore::OP<int>::step_cmp_greater_equal(e2, dy);
                    error += dy & e2_greater_equal_dy_mask;
                    x0 += sx & e2_greater_equal_dy_mask;

                    int e2_less_equal_dx_mask = -MathCore::OP<int>::step_cmp_less_equal(e2, dx);
                    error += dx & e2_less_equal_dx_mask;
                    y0 += sy & e2_less_equal_dx_mask;

                    draw_pt.draw_pixel(x0, y0);
                }
            }

            /// Example using Raster:
            ///
            /// using namespace MathCore;
            /// using namespace AlgorithmCore::Rasterization;
            ///
            /// Matrix<char> screen(vec2i(11, 11));
            /// struct BresenhamDraw
            /// {
            ///     Matrix<char> &m;
            ///     inline void draw_pixel(int x, int y) noexcept
            ///     {
            ///         m[y][x] = 'o';
            ///     }
            /// } DrawStructure{screen};
            /// BresenhamIterator::Raster(
            ///     DrawStructure,
            ///     vec2i(0, 10),
            ///     vec2i(5, 5)
            /// );
            template <typename T>
            static ITK_INLINE void Raster(T &draw_pt, const MathCore::vec2i &a, const MathCore::vec2i &b) noexcept
            {
                Raster(draw_pt, a.x, a.y, b.x, b.y);
            }
        };

    }

}
