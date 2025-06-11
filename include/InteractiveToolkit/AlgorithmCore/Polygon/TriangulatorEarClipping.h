#pragma once

#include "../../common.h"
#include "../../MathCore/MathCore.h"
#include "Polygon2D.h"

#include "../../ITKCommon/STL_Tools.h"
#include "./Quadtree2D_IndexedPoints.h"
#include "./FastRemovalCyclicVector.h"

#define EAR_CLIPPING_USE_IndexVector 0
#define EAR_CLIPPING_USE_FastRemovalCyclicVector 1

#define EAR_CLIPPING_CHOOSE EAR_CLIPPING_USE_IndexVector


namespace AlgorithmCore
{

    namespace Polygon
    {
        namespace EarClipping
        {

            static inline bool isEar(std::unique_ptr<Quadtree2D_IndexedPoints> &quadtree, const std::vector<MathCore::vec2f> &vertices, int32_t prev, int32_t curr, int32_t next)
            {
                const auto &a = vertices[prev];
                const auto &b = vertices[curr];
                const auto &c = vertices[next];

                // Check if triangle is counter-clockwise
                float orientation = MathCore::OP<MathCore::vec2f>::orientation(a, b, c);
                if (orientation <= 0)
                    return false;

                if (quadtree != nullptr)
                {
                    MathCore::vec2f box_min = MathCore::OP<MathCore::vec2f>::minimum(a, MathCore::OP<MathCore::vec2f>::minimum(b, c));
                    MathCore::vec2f box_max = MathCore::OP<MathCore::vec2f>::maximum(a, MathCore::OP<MathCore::vec2f>::maximum(b, c));

                    // Query the quadtree for points inside the triangle bounding box
                    const std::vector<size_t> &indices = quadtree->query(box_min, box_max);
                    if (indices.empty())
                        return true; // No points inside the bounding box, so it's an ear

                    // Check if any other vertex is inside this triangle
                    for (size_t i : indices)
                    {
                        if (i == prev || i == curr || i == next)
                            continue;
                        const auto &p = vertices[i];
                        // Remove duplicate points
                        if (p == a || p == b || p == c)
                            continue;
                        if (MathCore::OP<MathCore::vec2f>::point_inside_triangle(p, a, b, c))
                            return false;
                    }
                }
                else
                {
                    // Check if any other vertex is inside this triangle
                    for (size_t i = 0; i < vertices.size(); ++i)
                    {
                        if (i == prev || i == curr || i == next)
                            continue;
                        const auto &p = vertices[i];
                        // Remove duplicate points
                        if (p == a || p == b || p == c)
                            continue;
                        if (MathCore::OP<MathCore::vec2f>::point_inside_triangle(p, a, b, c))
                            return false;
                    }
                }

                return true;
            }
#if EAR_CLIPPING_CHOOSE == EAR_CLIPPING_USE_IndexVector
            static void earClipping(std::vector<int32_t> &ciclic_next_idx_walk, const std::vector<MathCore::vec2f> &vertices, std::vector<uint32_t> &triangles)
            {
                if (vertices.size() < 3)
                    return;

                std::unique_ptr<Quadtree2D_IndexedPoints> quadtree = STL_Tools::make_unique<Quadtree2D_IndexedPoints>(vertices, 16, 10);

                // std::vector<int> indices;
                ciclic_next_idx_walk.clear();
                ciclic_next_idx_walk.reserve(vertices.size());
                // next_idx is a idx list that points to the next valid index in the vertices array
                for (int32_t i = 0; i < (int32_t)vertices.size(); ++i)
                    ciclic_next_idx_walk.push_back((i + 1) % (int32_t)vertices.size());

                int32_t logic_size = static_cast<int32_t>(ciclic_next_idx_walk.size());
                int32_t prev_walker = 0;

                while (logic_size > 3)
                {
                    bool ear_found = false;

                    int32_t count = 0;
                    while (count < logic_size)
                    {

                        const auto &prev = prev_walker;
                        const auto &curr = ciclic_next_idx_walk[prev];
                        const auto &next = ciclic_next_idx_walk[curr];

                        if (isEar(quadtree, vertices, prev, curr, next))
                        {
                            triangles.push_back(prev);
                            triangles.push_back(curr);
                            triangles.push_back(next);

                            // erase current vertex from the walk by linking the previous to the next vertex
                            ciclic_next_idx_walk[prev] = next;
                            logic_size--;
                            
                            ear_found = true;
                            break;
                        }

                        prev_walker = ciclic_next_idx_walk[prev_walker];
                        count++;
                    }

                    // avoid infinite loop if no ear is found
                    if (!ear_found)
                        break;
                }

                // Adicione o último triângulo
                if (logic_size == 3)
                {
                    const auto &prev = prev_walker;
                    const auto &curr = ciclic_next_idx_walk[prev];
                    const auto &next = ciclic_next_idx_walk[curr];

                    triangles.push_back(prev);
                    triangles.push_back(curr);
                    triangles.push_back(next);
                }
            }
#elif EAR_CLIPPING_CHOOSE == EAR_CLIPPING_USE_FastRemovalCyclicVector
            static void earClipping(FastRemovalCyclicVector<int32_t> &indices_aux_buffer, const std::vector<MathCore::vec2f> &vertices, std::vector<uint32_t> &triangles)
            {
                if (vertices.size() < 3)
                    return;

                std::unique_ptr<Quadtree2D_IndexedPoints> quadtree = STL_Tools::make_unique<Quadtree2D_IndexedPoints>(vertices, 16, 10);

                // std::vector<int> indices;
                indices_aux_buffer.resize((uint32_t)vertices.size(), 1);
                // next_idx is a idx list that points to the next valid index in the vertices array
                for (int32_t i = 0; i < (int32_t)vertices.size(); ++i)
                    indices_aux_buffer[i] = i;

                while (indices_aux_buffer.size() > 3)
                {
                    bool ear_found = false;

                    for(auto it=indices_aux_buffer.begin(); it != indices_aux_buffer.end(); it++)
                    {
                        const auto &prev = it.element_back();
                        const auto &curr = *it;
                        const auto &next = it.element_next();

                        if (isEar(quadtree, vertices, prev, curr, next))
                        {
                            triangles.push_back(prev);
                            triangles.push_back(curr);
                            triangles.push_back(next);


                            // erase current vertex from the walk by linking the previous to the next vertex
                            indices_aux_buffer.remove(it);
                            ear_found = true;
                            break;
                        }
                    }

                    // avoid infinite loop if no ear is found
                    if (!ear_found)
                        break;
                }

                // Adicione o último triângulo
                if (indices_aux_buffer.size() == 3)
                {
                    auto it=indices_aux_buffer.begin();

                    const auto &prev = it.element_back();
                    const auto &curr = *it;
                    const auto &next = it.element_next();

                    triangles.push_back(prev);
                    triangles.push_back(curr);
                    triangles.push_back(next);
                }
            }
#else
    #error "Please choose EAR_CLIPPING_CHOOSE to EAR_CLIPPING_USE_IndexVector or EAR_CLIPPING_USE_FastRemovalCyclicVector"
#endif

            static void connectHole(std::vector<MathCore::vec2f> &connection_sequence_aux_buffer, std::vector<MathCore::vec2f> &outline, const std::vector<MathCore::vec2f> &hole)
            {
                // Encontre o ponto mais à direita do buraco
                size_t rightmost = 0;
                for (size_t i = 1; i < hole.size(); ++i)
                    if (hole[i].x > hole[rightmost].x)
                        rightmost = i;

                // Encontre o ponto visível mais próximo no contorno externo
                size_t connection_point = 0;
                float min_sqr_dist = MathCore::FloatTypeInfo<float>::max;
                for (size_t i = 0; i < outline.size(); ++i)
                {
                    if (outline[i].x >= hole[rightmost].x)
                    {
                        float sqr_dist = MathCore::OP<MathCore::vec2f>::sqrDistance(outline[i], hole[rightmost]);
                        if (sqr_dist < min_sqr_dist)
                        {
                            min_sqr_dist = sqr_dist;
                            connection_point = i;
                        }
                    }
                }
                if (min_sqr_dist == MathCore::FloatTypeInfo<float>::max)
                {
                    for (size_t i = 0; i < outline.size(); ++i)
                    {
                        float sqr_dist = MathCore::OP<MathCore::vec2f>::sqrDistance(outline[i], hole[rightmost]);
                        if (sqr_dist < min_sqr_dist)
                        {
                            min_sqr_dist = sqr_dist;
                            connection_point = i;
                        }
                    }
                }

                // std::vector<MathCore::vec2f> connection_sequence;
                connection_sequence_aux_buffer.clear();
                connection_sequence_aux_buffer.reserve(hole.size() + 2); // +2 for the connection point and the rightmost point
                for (size_t i = 0; i < hole.size(); ++i)
                    connection_sequence_aux_buffer.push_back(hole[(rightmost + i) % hole.size()]);

                // insert the initial hole point at the end of the sequence
                connection_sequence_aux_buffer.push_back(hole[rightmost]);
                // insert the connection point at the end of the sequence, so the polygon is closed
                connection_sequence_aux_buffer.push_back(outline[connection_point]);

                // Insere a sequência no outline após o ponto de conexão
                auto insertion_point = outline.begin() + connection_point + 1;

                outline.insert(insertion_point, connection_sequence_aux_buffer.begin(), connection_sequence_aux_buffer.end());
            }

            static void removeDuplicateVertices(std::vector<MathCore::vec2f> &filtered_aux_buffer, std::vector<MathCore::vec2f> &vertices, float epsilon = MathCore::EPSILON<float>::high_precision)
            {
                if (vertices.size() < 2)
                    return;

                // std::vector<MathCore::vec2f> filtered;
                filtered_aux_buffer.clear();
                filtered_aux_buffer.reserve(vertices.size());

                float sqr_epsilon = epsilon * epsilon;
                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    if (i == 0)
                        // Always keep the first vertex
                        filtered_aux_buffer.push_back(vertices[i]);
                    else if (MathCore::OP<MathCore::vec2f>::sqrDistance(filtered_aux_buffer.back(), vertices[i]) >= sqr_epsilon)
                        // Check against the last added vertex
                        filtered_aux_buffer.push_back(vertices[i]);
                }

                // Check if the last and first vertices are duplicates (closing the polygon)
                if (filtered_aux_buffer.size() > 2 &&
                    MathCore::OP<MathCore::vec2f>::sqrDistance(filtered_aux_buffer.back(), filtered_aux_buffer.front()) < sqr_epsilon)
                    filtered_aux_buffer.pop_back();

                // vertices = std::move(filtered);
                vertices.swap(filtered_aux_buffer);
            }

            static void removeColinearVertices(std::vector<MathCore::vec2f> &filtered_aux_buffer, std::vector<MathCore::vec2f> &vertices, float epsilon = MathCore::EPSILON<float>::high_precision)
            {
                if (vertices.size() < 3)
                    return;
                // std::vector<MathCore::vec2f> filtered;
                filtered_aux_buffer.clear();
                filtered_aux_buffer.reserve(vertices.size());

                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    const auto &prev = vertices[(i + vertices.size() - 1) % vertices.size()];
                    const auto &curr = vertices[i];
                    const auto &next = vertices[(i + 1) % vertices.size()];
                    // Check if the current point is collinear with the previous and next points
                    float orient = MathCore::OP<MathCore::vec2f>::orientation(prev, curr, next);
                    if (MathCore::OP<float>::abs(orient) > epsilon)
                        filtered_aux_buffer.push_back(curr);
                }
                // vertices = std::move(filtered);
                vertices.swap(filtered_aux_buffer);
            }

            struct ContourSampled
            {
                std::vector<MathCore::vec2f> vertex;
                bool is_hole; ///< true if this is a hole, false if it is an outline
            };

            /// \brief Triangulates a set of sampled contours, connecting holes to outlines.
            /// \param sampled_contours The contours to triangulate, each contour can be an outline or a hole.
            /// \param vertices The output vertices of the triangulation.
            /// \param triangles The output triangles of the triangulation, each triangle is represented by 3 indices into the vertices array in CCW winding.
            /// \details This function connects holes to outlines, removes duplicate and collinear vertices, and performs triangulation using the ear clipping method.
            /// \author Alessandro Ribeiro
            /// \note The input contours should be sampled and closed.
            /// \note The function assumes that the contours are in a 2D plane and will set the z-coordinate of the vertices to 0.
            /// \note The function does not handle self-intersecting polygons or complex cases where holes are nested within other holes.
            ///
            static void triangulate(const std::vector<ContourSampled> &sampled_contours,
                                    std::vector<MathCore::vec3f> *vertices,
                                    std::vector<uint32_t> *triangles)
            {
                vertices->clear();
                triangles->clear();
                if (sampled_contours.empty())
                    return;

#if EAR_CLIPPING_CHOOSE == EAR_CLIPPING_USE_IndexVector
                std::vector<int32_t> index_shared_vector;
#elif EAR_CLIPPING_CHOOSE == EAR_CLIPPING_USE_FastRemovalCyclicVector
                FastRemovalCyclicVector<int32_t> index_shared_vector;
#endif
                std::vector<uint32_t> triangles_aux;
                std::vector<MathCore::vec2f> combined_vertices;
                std::vector<MathCore::vec2f> vec2_shared_vector;

                // connecting the holes to the outlines
                for (const auto &outline : sampled_contours)
                {
                    if (outline.is_hole || outline.vertex.size() < 3)
                        continue; // Skip holes for now
                    // here the contour is an outline
                    // std::vector<MathCore::vec2f> combined_vertices = outline.vertex;
                    combined_vertices.assign(outline.vertex.begin(), outline.vertex.end());
                    for (const auto &hole : sampled_contours)
                    {
                        if (!hole.is_hole || hole.vertex.size() < 3)
                            continue; // Skip outlines
                        // check if the hole vertice is inside the current outline
                        if (Polygon2DUtils::checkPointInside(combined_vertices, hole.vertex[0]))
                            // Connect the hole to the outline
                            connectHole(vec2_shared_vector, combined_vertices, hole.vertex);
                    }

                    removeDuplicateVertices(vec2_shared_vector, combined_vertices);
                    if (combined_vertices.size() < 3)
                        continue; // Not enough vertices to form a polygon

                    removeColinearVertices(vec2_shared_vector, combined_vertices);
                    if (combined_vertices.size() < 3)
                        continue; // Not enough vertices to form a polygon

                    // std::vector<uint32_t> triangles_aux;
                    triangles_aux.clear();
                    // Triangulate the combined vertices
                    earClipping(index_shared_vector, combined_vertices, triangles_aux);
                    if (triangles_aux.empty())
                        continue; // No triangles found

                    uint32_t idx_offset = static_cast<uint32_t>(vertices->size());
                    vertices->reserve(vertices->size() + combined_vertices.size());
                    for (const auto &v : combined_vertices)
                        vertices->push_back(MathCore::vec3f(v, 0)); // Converte para vec3f
                    triangles->reserve(triangles->size() + triangles_aux.size());
                    for (const auto &idx : triangles_aux)
                        triangles->push_back(idx + idx_offset); // Adiciona o offset para os índices dos triângulos
                }
            }

        }
    }
}