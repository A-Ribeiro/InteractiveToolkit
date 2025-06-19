#pragma once
#include <vector>
#include <algorithm>
#include "../../MathCore/MathCore.h"

namespace AlgorithmCore
{
    namespace Polygon
    {

#define QuadrantBitSet(x_positive_east_bit, y_positive_north_bit) \
    (Quadrant)(((int)(y_positive_north_bit) << 1) | (int)(x_positive_east_bit))
        enum Quadrant
        {
            Quadrant_bit_E = 0x01, // East
            Quadrant_bit_N = 0x10, // North

            Quadrant_SW = 0b000, // South-West
            Quadrant_SE = 0b001, // South-East
            Quadrant_NW = 0b010, // North-West
            Quadrant_NE = 0b011, // North-East
            Quadrant_COUNT = 0b100
        };

        class Quadtree2D_IndexedPoints
        {
            struct Node
            {
                MathCore::vec2f box_min, box_max, box_center;
                std::vector<uint32_t> indices;
                std::unique_ptr<Node> children[Quadrant_COUNT];
                int32_t depth;

                ITK_INLINE Node(const MathCore::vec2f &min_, const MathCore::vec2f &max_, int32_t depth_, int32_t initial_item_count) : indices(initial_item_count)
                {
                    indices.clear();
                    box_min = min_;
                    box_max = max_;
                    depth = depth_;
                    box_center = (box_min + box_max) * 0.5f;
                }
                ~Node()
                {
                }

                ITK_INLINE Quadrant computeQuadrant(const MathCore::vec2f &p) const
                {
                    return QuadrantBitSet(p.x >= box_center.x, p.y >= box_center.y);

                    // if (p.x < box_center.x)
                    // {
                    //     if (p.y < box_center.y)
                    //         return Quadrant_SW; // SW
                    //     else
                    //         return Quadrant_NW; // NW
                    // }
                    // else
                    // {
                    //     if (p.y < box_center.y)
                    //         return Quadrant_SE; // SE
                    //     else
                    //         return Quadrant_NE; // NE
                    // }
                }

                ITK_INLINE void subdivide(const std::vector<MathCore::vec2f> &points, int32_t initial_index_count)
                {
                    using namespace MathCore;
                    int32_t depth_plus_one = depth + 1;
                    children[Quadrant_SW] = STL_Tools::make_unique<Node>(box_min, box_center, depth_plus_one, initial_index_count);
                    children[Quadrant_SE] = STL_Tools::make_unique<Node>(vec2f(box_center.x, box_min.y), vec2f(box_max.x, box_center.y), depth_plus_one, initial_index_count);
                    children[Quadrant_NW] = STL_Tools::make_unique<Node>(vec2f(box_min.x, box_center.y), vec2f(box_center.x, box_max.y), depth_plus_one, initial_index_count);
                    children[Quadrant_NE] = STL_Tools::make_unique<Node>(box_center, box_max, depth_plus_one, initial_index_count);

                    if (indices.empty())
                        return; // No indices to distribute

                    for (auto idx : indices)
                    {
                        Quadrant quad = computeQuadrant(points[idx]);
                        children[quad]->indices.push_back(idx);
                    }
                    // do not need to clear indices,
                    // as we are moving them to the children nodes
                    // the query is done only in the Leaf nodes
                    indices.clear();
                }

                // void insert_point(const std::vector<MathCore::vec2f> &points, uint32_t idx, int32_t maxDepth, int32_t point_count_threshold_to_subdivide)
                // {
                //     bool has_children = this->hasChildren();
                //     // do not have children, so we can insert the point here
                //     if (!has_children && (depth >= maxDepth || indices.size() < point_count_threshold_to_subdivide))
                //     {
                //         indices.push_back(idx);
                //         return;
                //     }
                //     // If the node is a leaf and has no children, subdivide it
                //     //if (isLeaf())
                //     if (!has_children)
                //         subdivide(points, point_count_threshold_to_subdivide);
                //     Quadrant quad = computeQuadrant(points[idx]);
                //     children[quad]->insert_point(points, idx, maxDepth, point_count_threshold_to_subdivide);
                // }

                ITK_INLINE bool contains(const MathCore::vec2f &p) const
                {
                    return (p.x >= box_min.x && p.x <= box_max.x &&
                            p.y >= box_min.y && p.y <= box_max.y);
                }

                ITK_INLINE bool intersects(const MathCore::vec2f &min, const MathCore::vec2f &max) const
                {
                    return !(box_max.x < min.x || box_min.x > max.x ||
                             box_max.y < min.y || box_min.y > max.y);
                }

                ITK_INLINE bool intersects(const Node &other) const
                {
                    return !(box_max.x < other.box_min.x || box_min.x > other.box_max.x ||
                             box_max.y < other.box_min.y || box_min.y > other.box_max.y);
                }

                ITK_INLINE bool isLeaf() const
                {
                    return children[0] == nullptr;
                    // return children[0] == nullptr && children[1] == nullptr &&
                    //        children[2] == nullptr && children[3] == nullptr;
                }

                ITK_INLINE bool hasChildren() const
                {
                    return children[0] != nullptr;
                    // return children[0] != nullptr || children[1] != nullptr ||
                    //        children[2] != nullptr || children[3] != nullptr;
                }

                void query(const std::vector<MathCore::vec2f> &points,
                           const MathCore::vec2f &min, const MathCore::vec2f &max,
                           std::vector<uint32_t> &result) const
                {
                    if (!intersects(min, max))
                        return; // No intersection with the query box
                    if (hasChildren())
                    {
                        for (const auto &children : children)
                            children->query(points, min, max, result);
                    }
                    else
                    {
                        for (uint32_t idx : indices)
                        {
                            if (contains(points[idx]))
                                result.push_back(idx);
                        }
                    }
                }
            };

            std::unique_ptr<Node> root;
            const std::vector<MathCore::vec2f> &points;
            int32_t maxDepth, minPointThresholdToSubdivide;

            // fastest way to insert a point index into the quadtree
            ITK_INLINE void insert_point(uint32_t idx)
            {
                auto insert_on_node = root.get();
                while (insert_on_node)
                {
                    bool has_children = insert_on_node->hasChildren();
                    // do not have children, so we can insert the point here
                    if (!has_children && (insert_on_node->depth >= maxDepth || insert_on_node->indices.size() < minPointThresholdToSubdivide))
                    {
                        insert_on_node->indices.push_back(idx);
                        return;
                    }
                    // If the node is a leaf and has no children, subdivide it
                    // if (isLeaf())
                    if (!has_children)
                        insert_on_node->subdivide(points, minPointThresholdToSubdivide);
                    Quadrant quadrant = insert_on_node->computeQuadrant(points[idx]);
                    insert_on_node = insert_on_node->children[quadrant].get();
                }
            }

        public:
            std::vector<uint32_t> last_query;

            ITK_INLINE Quadtree2D_IndexedPoints(const std::vector<MathCore::vec2f> &pts, int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16)
                : points(pts)
            {
                maxDepth = maxDepth_;
                minPointThresholdToSubdivide = minPointThresholdToSubdivide_;

                if (pts.empty())
                {
                    root.reset();
                    return;
                }
                MathCore::vec2f _min = points[0], _max = points[0];
                for (const auto &p : points)
                {
                    _min = MathCore::OP<MathCore::vec2f>::minimum(_min, p);
                    _max = MathCore::OP<MathCore::vec2f>::maximum(_max, p);
                }
                root = STL_Tools::make_unique<Node>(_min, _max, 0, minPointThresholdToSubdivide);
                for (uint32_t i = 0; i < (uint32_t)points.size(); ++i)
                    insert_point(i);
            }

            ~Quadtree2D_IndexedPoints()
            {
            }

            ITK_INLINE std::vector<uint32_t> &query(const MathCore::vec2f &min, const MathCore::vec2f &max)
            {
                last_query.clear();
                if (!root)
                    return last_query; // No points to query

                root->query(points, min, max, last_query);

                return last_query;
            }
        };

    }
}