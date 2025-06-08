#pragma once

#include "../../common.h"
#include "../../MathCore/MathCore.h"

namespace AlgorithmCore
{

    namespace Polygon
    {
        namespace Polygon2DUtils
        {
            /// \brief Check if a point is inside a polygon
            ///
            /// This function checks if a point is inside a polygon using the ray-casting algorithm.
            ///
            /// \param point The point to check.
            /// \param polygon The polygon represented as a vector of points.
            /// \return True if the point is inside the polygon, false otherwise.
            template <typename PointType = MathCore::vec2f>
            static inline bool checkPointInside(const std::vector<PointType> &polygon, const PointType &point)
            {
                using float_type = typename PointType::element_type;

                bool inside = false;
                size_t count = polygon.size();
                if (count < 3)
                    return false; // A polygon must have at least 3 points
                size_t prev_i = count - 1;
                for (size_t i = 0; i < count; i++)
                {
                    const PointType &pt_curr = polygon[i];
                    const PointType &pt_prev = polygon[prev_i];

                    // check if the point is within the y-bounds of the polygon edge
                    if ((pt_curr.y > point.y) != (pt_prev.y > point.y))
                    {
                        PointType curr_to_prev = pt_prev - pt_curr;
                        PointType curr_to_point = point - pt_curr;
                        // Calculate the x-coordinate of the intersection point
                        float_type inclination = curr_to_prev.x / curr_to_prev.y;
                        float_type x = pt_curr.x + curr_to_point.y * inclination;
                        // Check if the point is to the left of the intersection point
                        if (point.x < x)
                            inside = !inside; // Toggle the inside state
                    }
                    prev_i = i;
                }
                return inside;
            }

            /// \brief Calculate the area of a polygon with a sign
            /// ///
            /// This function calculates the area of a polygon using the shoelace formula.
            /// Positive area → counterclockwise (Outline)
            /// Negative area → clockwise (Hole)
            ///
            /// \param polygon The polygon represented as a vector of points.
            /// \return The signed area of the polygon.
            template <typename PointType = MathCore::vec2f>
            static inline typename PointType::element_type signedArea(const std::vector<PointType> &polygon)
            {
                using float_type = typename PointType::element_type;

                float_type area = (float_type)0;
                size_t count = polygon.size();
                if (count < 3)
                    return false; // A polygon must have at least 3 points
                size_t prev_i = count - 1;
                for (size_t i = 0; i < count; i++)
                {
                    const PointType &pt_curr = polygon[i];
                    const PointType &pt_prev = polygon[prev_i];
                    // Calculate the area using the shoelace formula
                    area += pt_prev.x * pt_curr.y - pt_curr.x * pt_prev.y;
                    prev_i = i;
                }
                return area * (float_type)0.5;
            }

            /// \brief Calculate the perimeter of a polygon
            /// ///
            /// This function calculates the perimeter of a polygon by summing the distances between consecutive points.
            /// /// \param polygon The polygon represented as a vector of points.
            /// \return The perimeter of the polygon.
            template <typename PointType = MathCore::vec2f>
            static inline typename PointType::element_type perimeter(const std::vector<PointType> &polygon)
            {
                using float_type = typename PointType::element_type;

                float_type perimeter = (float_type)0;
                size_t count = polygon.size();
                size_t prev_i = count - 1;
                for (size_t i = 0; i < count; i++)
                {
                    perimeter += MathCore::OP<MathCore::vec2f>::distance(polygon[i], polygon[prev_i]);
                    prev_i = i;
                }
                return perimeter;
            }

            template <typename PointType = MathCore::vec2f>
            static inline void addPoint(std::vector<PointType> &polygon, const PointType &point)
            {
                polygon.push_back(point);
            }

            template <typename PointType = MathCore::vec2f>
            static inline void addBezierQuadratic(std::vector<PointType> &polygon, const PointType &_p0, const PointType &_p1, const PointType &_p2, typename PointType::element_type max_dst)
            {
                using float_type = typename PointType::element_type;
                float_type max_dst_squared = max_dst * max_dst;

                struct itemT
                {
                    PointType p0;
                    PointType p1;
                    PointType p2;
                };
                std::vector<itemT> to_traverse;
                to_traverse.push_back({_p0, _p1, _p2});

                while (to_traverse.size() > 0)
                {
                    auto item = to_traverse.back();
                    to_traverse.pop_back();

                    // another way to compute max_dst based on tolerance
                    // PointType p1_to_p2 = p2 - p1;
                    // PointType p1_to_p0 = p0 - p1;
                    // // losangle pointing to center
                    // float_type length_sqr = MathCore::OP<PointType>::sqrLength(p1_to_p2 + p1_to_p0);

                    // linear distance
                    float_type length_sqr = MathCore::OP<PointType>::sqrLength(item.p2 - item.p0);
                    if (length_sqr <= max_dst_squared)
                        polygon.push_back(item.p2);
                    else
                    {
                        PointType p0_p1_middle = (item.p0 + item.p1) * (float_type)0.5;
                        PointType p1_p2_middle = (item.p1 + item.p2) * (float_type)0.5;
                        PointType second_level_middle = (p0_p1_middle + p1_p2_middle) * (float_type)0.5;
                        // reverse push children
                        to_traverse.push_back({second_level_middle, p1_p2_middle, item.p2});
                        to_traverse.push_back({item.p0, p0_p1_middle, second_level_middle});
                    }
                }
            }

            template <typename PointType = MathCore::vec2f>
            static inline void addBezierCubic(std::vector<PointType> &polygon, const PointType &_p0, const PointType &_p1, const PointType &_p2, const PointType &_p3, typename PointType::element_type max_dst)
            {
                using float_type = typename PointType::element_type;
                float_type max_dst_squared = max_dst * max_dst;

                struct itemT
                {
                    PointType p0;
                    PointType p1;
                    PointType p2;
                    PointType p3;
                };
                std::vector<itemT> to_traverse;
                to_traverse.push_back({_p0, _p1, _p2, _p3});

                while (to_traverse.size() > 0)
                {
                    auto item = to_traverse.back();
                    to_traverse.pop_back();

                    // another way to compute max_dst based on tolerance
                    // PointType p0_p3_center = (p0 + p3) * (float_type)0.5;
                    // PointType center_to_p1 = p1 - p0_p3_center;
                    // PointType center_to_p2 = p2 - p0_p3_center;
                    // // losangle pointing from center
                    // float_type length_sqr = MathCore::OP<PointType>::sqrLength(center_to_p1 + center_to_p2);

                    // linear distance
                    float_type length_sqr = MathCore::OP<PointType>::sqrLength(item.p3 - item.p0);
                    if (length_sqr <= max_dst_squared)
                        polygon.push_back(item.p3);
                    else
                    {
                        PointType p0_p1_middle = (item.p0 + item.p1) * (float_type)0.5;
                        PointType p1_p2_middle = (item.p1 + item.p2) * (float_type)0.5;
                        PointType p2_p3_middle = (item.p2 + item.p3) * (float_type)0.5;
                        PointType second_level_middle_p0_p2 = (p0_p1_middle + p1_p2_middle) * (float_type)0.5;
                        PointType second_level_middle_p1_p3 = (p1_p2_middle + p2_p3_middle) * (float_type)0.5;
                        PointType third_level_middle = (second_level_middle_p0_p2 + second_level_middle_p1_p3) * (float_type)0.5;
                        // reverse push children
                        to_traverse.push_back({third_level_middle, second_level_middle_p1_p3, p2_p3_middle, item.p3});
                        to_traverse.push_back({item.p0, p0_p1_middle, second_level_middle_p0_p2, third_level_middle});
                    }
                }
            }

        }

        enum PointType
        {
            PointTypeVertex = 0,       ///< Vertex point type
            PointTypeControlPoint = 1, ///< Control point type
        };

        /// \brief Closed polygon class
        ///
        /// This class represents a closed polygon.
        ///
        /// A closed polygon is a polygon where the first and last points are the same.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <typename VecType = MathCore::vec2f>
        class Polygon2D
        {
        public:
            struct Point
            {
                PointType pointType;
                VecType vertex;
            };
            using float_type = typename VecType::element_type;

            std::vector<Point> points; // Points of the closed polygon
            float_type signedArea;

            Polygon2D() : signedArea(0) {}
            Polygon2D(const Polygon2D &other) : points(other.points), signedArea(other.signedArea) {}
            Polygon2D(Polygon2D &&other) noexcept : points(std::move(other.points)), signedArea(other.signedArea)
            {
                other.signedArea = 0;
            }
            Polygon2D &operator=(const Polygon2D &other)
            {
                if (this != &other)
                {
                    points = other.points;
                    signedArea = other.signedArea;
                }
                return *this;
            }
            Polygon2D &operator=(Polygon2D &&other) noexcept
            {
                if (this != &other)
                {
                    points = std::move(other.points);
                    signedArea = other.signedArea;
                    other.signedArea = 0;
                }
                return *this;
            }

            bool isHole() const
            {
                return signedArea < 0;
            }

            bool isOutline() const
            {
                return signedArea > 0;
            }

            void reverse()
            {
                if (points.empty())
                    return;
                // add the first point to the end to close the polygon
                points.push_back(points[0]);
                std::reverse(points.begin(), points.end());
                // remove the last point, which is the same as the first point
                points.pop_back(); 
                signedArea = -signedArea; // reverse the sign of the area
            }

            void samplePoints(std::vector<VecType> *output, typename VecType::element_type max_dst) const
            {
                std::vector<VecType> aux_pt;
                VecType last_pt;
                for (const auto &point : this->points)
                {
                    if (point.pointType == PointTypeVertex)
                    {
                        if (aux_pt.size() == 2)
                        {
                            Polygon2DUtils::addBezierQuadratic<VecType>(*output, aux_pt[0], aux_pt[1], point.vertex, max_dst);
                            aux_pt.clear();
                        }
                        else if (aux_pt.size() == 3)
                        {
                            Polygon2DUtils::addBezierCubic<VecType>(*output, aux_pt[0], aux_pt[1], aux_pt[2], point.vertex, max_dst);
                            aux_pt.clear();
                        }
                        else
                            Polygon2DUtils::addPoint<VecType>(*output, point.vertex);
                    }
                    else if (point.pointType == PointTypeControlPoint)
                    {
                        if (aux_pt.empty())
                            aux_pt.push_back(last_pt);
                        aux_pt.push_back(point.vertex);
                    }
                    last_pt = point.vertex;
                }

                if (aux_pt.size() == 2)
                {
                    Polygon2DUtils::addBezierQuadratic<VecType>(*output, aux_pt[0], aux_pt[1], this->points[0].vertex, max_dst);
                    aux_pt.clear();
                    output->pop_back(); // remove last point, because it is the same as the first point
                }
                else if (aux_pt.size() == 3)
                {
                    Polygon2DUtils::addBezierCubic<VecType>(*output, aux_pt[0], aux_pt[1], aux_pt[2], this->points[0].vertex, max_dst);
                    aux_pt.clear();
                    output->pop_back(); // remove last point, because it is the same as the first point
                }
            }

            void removeLastPoint()
            {
                if (!points.empty())
                    points.pop_back();
            }
            void computeSignedArea()
            {
                if (points.size() < 3)
                {
                    signedArea = 0;
                    return;
                }    
                std::vector<VecType> points;
                samplePoints(&points, MathCore::FloatTypeInfo<float_type>::max);
                signedArea = Polygon2DUtils::signedArea<VecType>(points);
            }
            void addPoint(const VecType &point)
            {
                points.push_back({PointTypeVertex, point});
            }
            void addControlPoint(const VecType &point)
            {
                points.push_back({PointTypeControlPoint, point});
            }
            void addBezierQuadratic(const VecType &p0, const VecType &p1, const VecType &p2)
            {
                points.push_back({PointTypeVertex, p0});
                points.push_back({PointTypeControlPoint, p1});
                points.push_back({PointTypeVertex, p2});
            }
            void addBezierCubic(const VecType &p0, const VecType &p1, const VecType &p2, const VecType &p3)
            {
                points.push_back({PointTypeVertex, p0});
                points.push_back({PointTypeControlPoint, p1});
                points.push_back({PointTypeControlPoint, p2});
                points.push_back({PointTypeVertex, p3});
            }
        };

    }
}
