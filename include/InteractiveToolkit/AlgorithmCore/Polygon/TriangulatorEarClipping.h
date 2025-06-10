#pragma once

#include "../../common.h"
#include "../../MathCore/MathCore.h"
#include "Polygon2D.h"

#include "../../ITKCommon/STL_Tools.h"

namespace AlgorithmCore
{

    namespace Polygon
    {
        namespace EarClipping
        {

            static inline bool isEar(const std::vector<MathCore::vec2f> &vertices, int prev, int curr, int next)
            {
                const auto &a = vertices[prev];
                const auto &b = vertices[curr];
                const auto &c = vertices[next];

                // Check if triangle is counter-clockwise
                float orientation = MathCore::OP<MathCore::vec2f>::orientation(a, b, c);
                if (orientation <= 0)
                    return false;

                // Check if any other vertex is inside this triangle
                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    if (i == prev || i == curr || i == next)
                        continue;

                    const auto &p = vertices[i];

                    // MathCore::OP<MathCore::vec2f>::sqrDistance(p, a) < 1e-10f) ||
                    // MathCore::OP<MathCore::vec2f>::sqrDistance(p, b) < 1e-10f) ||
                    // MathCore::OP<MathCore::vec2f>::sqrDistance(p, c) < 1e-10f) ||
                    // Remove duplicate points
                    if (p == a || p == b || p == c)
                        continue;
                    if (MathCore::OP<MathCore::vec2f>::point_inside_triangle(p, a, b, c))
                        return false;
                }
                return true;
            }

            static void earClipping(const std::vector<MathCore::vec2f> &vertices, std::vector<uint32_t> &triangles)
            {
                if (vertices.size() < 3)
                    return;

                std::vector<int> indices;
                // Inicializa os índices com os indices dos vértices em sequência
                for (int i = 0; i < vertices.size(); ++i)
                    indices.push_back(i);

                size_t safety_counter = 0;
                size_t max_iterations = indices.size() * indices.size(); // Previne loop infinito

                while (indices.size() > 3 && safety_counter < max_iterations)
                {
                    bool ear_found = false;
                    for (size_t i = 0; i < indices.size(); ++i)
                    {
                        int prev = indices[(i - 1 + indices.size()) % indices.size()];
                        int curr = indices[i];
                        int next = indices[(i + 1) % indices.size()];
                        if (isEar(vertices, prev, curr, next))
                        {
                            triangles.push_back(prev);
                            triangles.push_back(curr);
                            triangles.push_back(next);

                            indices.erase(indices.begin() + i);
                            ear_found = true;
                            break;
                        }
                    }
                    if (!ear_found)
                        break; // Evita loop infinito
                    safety_counter++;
                }

                // Adicione o último triângulo
                if (indices.size() == 3)
                {
                    triangles.push_back(indices[0]);
                    triangles.push_back(indices[1]);
                    triangles.push_back(indices[2]);
                }
            }

            static void connectHole(std::vector<MathCore::vec2f> &outline, const std::vector<MathCore::vec2f> &hole)
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

                std::vector<MathCore::vec2f> connection_sequence;
                connection_sequence.reserve(hole.size() + 2); // +2 for the connection point and the rightmost point
                for (size_t i = 0; i < hole.size(); ++i)
                    connection_sequence.push_back(hole[(rightmost + i) % hole.size()]);

                // insert the initial hole point at the end of the sequence
                connection_sequence.push_back(hole[rightmost]);
                // insert the connection point at the end of the sequence, so the polygon is closed
                connection_sequence.push_back(outline[connection_point]);

                // Insere a sequência no outline após o ponto de conexão
                auto insertion_point = outline.begin() + connection_point + 1;

                outline.insert(insertion_point, connection_sequence.begin(), connection_sequence.end());
            }

            static void removeDuplicateVertices(std::vector<MathCore::vec2f> &vertices, float epsilon = MathCore::EPSILON<float>::high_precision)
            {
                if (vertices.size() < 2)
                    return;

                std::vector<MathCore::vec2f> filtered;
                filtered.reserve(vertices.size());

                float sqr_epsilon = epsilon * epsilon;
                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    if (i == 0)
                        // Always keep the first vertex
                        filtered.push_back(vertices[i]);
                    else if (MathCore::OP<MathCore::vec2f>::sqrDistance(filtered.back(), vertices[i]) >= sqr_epsilon)
                        // Check against the last added vertex
                        filtered.push_back(vertices[i]);
                }

                // Check if the last and first vertices are duplicates (closing the polygon)
                if (filtered.size() > 2 &&
                    MathCore::OP<MathCore::vec2f>::sqrDistance(filtered.back(), filtered.front()) < sqr_epsilon)
                    filtered.pop_back();

                vertices = std::move(filtered);
            }

            static void removeColinearVertices(std::vector<MathCore::vec2f> &vertices, float epsilon = MathCore::EPSILON<float>::high_precision)
            {
                if (vertices.size() < 3)
                    return;
                std::vector<MathCore::vec2f> filtered;
                filtered.reserve(vertices.size());

                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    const auto &prev = vertices[(i + vertices.size() - 1) % vertices.size()];
                    const auto &curr = vertices[i];
                    const auto &next = vertices[(i + 1) % vertices.size()];
                    // Check if the current point is collinear with the previous and next points
                    float orient = MathCore::OP<MathCore::vec2f>::orientation(prev, curr, next);
                    if (MathCore::OP<float>::abs(orient) > epsilon)
                        filtered.push_back(curr);
                }
                vertices = std::move(filtered);
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

                // connecting the holes to the outlines
                for (const auto &outline : sampled_contours)
                {
                    if (outline.is_hole || outline.vertex.size() < 3)
                        continue; // Skip holes for now
                    // here the contour is an outline
                    std::vector<MathCore::vec2f> combined_vertices = outline.vertex;
                    for (const auto &hole : sampled_contours)
                    {
                        if (!hole.is_hole || hole.vertex.size() < 3)
                            continue; // Skip outlines
                        // check if the hole vertice is inside the current outline
                        if (Polygon2DUtils::checkPointInside(combined_vertices, hole.vertex[0]))
                            // Connect the hole to the outline
                            connectHole(combined_vertices, hole.vertex);
                    }

                    removeDuplicateVertices(combined_vertices);
                    if (combined_vertices.size() < 3)
                        continue; // Not enough vertices to form a polygon

                    removeColinearVertices(combined_vertices);
                    if (combined_vertices.size() < 3)
                        continue; // Not enough vertices to form a polygon

                    std::vector<uint32_t> triangles_aux;
                    // Triangulate the combined vertices
                    earClipping(combined_vertices, triangles_aux);
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