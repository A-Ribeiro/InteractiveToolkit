#pragma once
#include <vector>
#include <algorithm>
#include "../../MathCore/MathCore.h"

namespace AlgorithmCore
{
    namespace Polygon
    {

        enum Quadrant
        {
            Quadrant_SW = 0, // South-West
            Quadrant_SE = 1, // South-East
            Quadrant_NW = 2, // North-West
            Quadrant_NE = 3, // North-East
            Quadrant_COUNT = 4
        };

        class Quadtree2D_IndexedPoints
        {
            struct Node
            {
                MathCore::vec2f box_min, box_max, box_center;
                std::vector<size_t> indices;
                std::unique_ptr<Node> children[Quadrant_COUNT];
                int32_t depth;

                Node(const MathCore::vec2f &min_, const MathCore::vec2f &max_, int32_t depth_, int32_t initial_item_count):indices(initial_item_count)
                {
                    indices.clear();
                    box_min = min_;
                    box_max = max_;
                    depth = depth_;
                    box_center = (box_min + box_max) * 0.5f;
                    for (int32_t i = 0; i < Quadrant_COUNT; ++i)
                        children[i] = nullptr;
                }
                ~Node()
                {
                    for (int32_t i = 0; i < Quadrant_COUNT; ++i)
                        children[i] = nullptr;
                }

                Quadrant computeQuadrant(const MathCore::vec2f &p) const
                {
                    //return (Quadrant)(((int)(p.y >= box_center.y) << 1) | (int)(p.x >= box_center.x));

                    if (p.x < box_center.x)
                    {
                        if (p.y < box_center.y)
                            return Quadrant_SW; // SW
                        else
                            return Quadrant_NW; // NW
                    }
                    else
                    {
                        if (p.y < box_center.y)
                            return Quadrant_SE; // SE
                        else
                            return Quadrant_NE; // NE
                    }
                }

                void subdivide(const std::vector<MathCore::vec2f> &points, int32_t initial_index_count)
                {
                    using namespace MathCore;
                    children[Quadrant_SW] = STL_Tools::make_unique<Node>(box_min, box_center, depth + 1, initial_index_count);
                    children[Quadrant_SE] = STL_Tools::make_unique<Node>(vec2f(box_center.x, box_min.y), vec2f(box_max.x, box_center.y), depth + 1, initial_index_count);
                    children[Quadrant_NW] = STL_Tools::make_unique<Node>(vec2f(box_min.x, box_center.y), vec2f(box_center.x, box_max.y), depth + 1, initial_index_count);
                    children[Quadrant_NE] = STL_Tools::make_unique<Node>(box_center, box_max, depth + 1, initial_index_count);

                    if (indices.empty())
                        return; // No indices to distribute

                    for (size_t idx : indices)
                    {
                        Quadrant quad = computeQuadrant(points[idx]);
                        children[quad]->indices.push_back(idx);
                    }
                    // do not need to clear indices,
                    // as we are moving them to the children nodes
                    // the query is done only in the Leaf nodes
                    indices.clear();
                }

                void insert_point(const std::vector<MathCore::vec2f> &points, size_t idx, int32_t maxDepth, int32_t point_count_threshold_to_subdivide)
                {
                    bool has_children = this->hasChildren();
                    // do not have children, so we can insert the point here
                    if (!has_children && (depth >= maxDepth || indices.size() < point_count_threshold_to_subdivide))
                    {
                        indices.push_back(idx);
                        return;
                    }
                    // If the node is a leaf and has no children, subdivide it
                    //if (isLeaf())
                    if (!has_children)
                        subdivide(points, point_count_threshold_to_subdivide);
                    Quadrant quad = computeQuadrant(points[idx]);
                    children[quad]->insert_point(points, idx, maxDepth, point_count_threshold_to_subdivide);
                }

                bool contains(const MathCore::vec2f &p) const
                {
                    return (p.x >= box_min.x && p.x <= box_max.x &&
                            p.y >= box_min.y && p.y <= box_max.y);
                }

                bool intersects(const MathCore::vec2f &min, const MathCore::vec2f &max) const
                {
                    return !(box_max.x < min.x || box_min.x > max.x ||
                             box_max.y < min.y || box_min.y > max.y);
                }

                bool intersects(const Node &other) const
                {
                    return !(box_max.x < other.box_min.x || box_min.x > other.box_max.x ||
                             box_max.y < other.box_min.y || box_min.y > other.box_max.y);
                }

                bool isLeaf() const
                {
                    return children[0] == nullptr && children[1] == nullptr &&
                           children[2] == nullptr && children[3] == nullptr;
                }

                bool hasChildren() const
                {
                    return children[0] != nullptr || children[1] != nullptr ||
                           children[2] != nullptr || children[3] != nullptr;
                }

                void query(const std::vector<MathCore::vec2f> &points,
                           const MathCore::vec2f &min, const MathCore::vec2f &max,
                           std::vector<size_t> &result) const
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
                        for (size_t idx : indices)
                        {
                            const auto &p = points[idx];
                            // Check if the point is within the query box
                            if (contains(p))
                                result.push_back(idx);
                        }
                    }
                }
            };

            std::unique_ptr<Node> root;
            const std::vector<MathCore::vec2f> &points;
            int32_t maxDepth, minPointThresholdToSubdivide;

        public:
            std::vector<size_t> last_query;

            Quadtree2D_IndexedPoints(const std::vector<MathCore::vec2f> &pts, int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16)
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
                for (size_t i = 0; i < points.size(); ++i)
                    root->insert_point(points, i, maxDepth, minPointThresholdToSubdivide);
            }

            ~Quadtree2D_IndexedPoints()
            {
                root.reset();
            }

            std::vector<size_t> &query(const MathCore::vec2f &min, const MathCore::vec2f &max)
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