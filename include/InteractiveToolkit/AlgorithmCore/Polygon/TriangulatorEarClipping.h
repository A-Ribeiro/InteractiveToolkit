#pragma once

#include "../../common.h"
#include "../../MathCore/MathCore.h"
#include "Polygon2D.h"

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
                    if (MathCore::OP<MathCore::vec2f>::point_inside_triangle(vertices[i], a, b, c))
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

                while (indices.size() > 3)
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

                // Insira o buraco no contorno
                auto insertion_point = outline.begin() + connection_point + 1;
                // outline[connection_point + 1] = hole[rightmost] ...
                // Insere todos os pontos do buraco no contorno
                outline.insert(insertion_point, hole.begin() + rightmost, hole.end());
                outline.insert(insertion_point + (hole.size() - rightmost), hole.begin(), hole.begin() + rightmost + 1);
                // Insere o ponto de conexão do contorno externo para fechar o polígono
                outline.insert(insertion_point + hole.size() + 1, outline[connection_point]);
            }

            struct ContourSampled
            {
                std::vector<MathCore::vec2f> vertex;
                bool is_hole; ///< true if this is a hole, false if it is an outline
            };

            static void triangulate(const std::vector<ContourSampled> &sampled_contours,
                                    std::vector<MathCore::vec3f> *vertices,
                                    std::vector<uint32_t> *triangles)
            {
                vertices->clear();
                triangles->clear();
                if (sampled_contours.empty())
                    return;

                std::vector<MathCore::vec2f> combined_vertices;
                std::vector<uint32_t> triangles_aux;

                // connecting the holes to the outlines
                for (const auto &outline : sampled_contours)
                {
                    if (outline.is_hole || outline.vertex.size() < 3)
                        continue; // Skip holes for now
                    // here the contour is an outline
                    combined_vertices = outline.vertex;
                    for (const auto &hole : sampled_contours)
                    {
                        if (!hole.is_hole || hole.vertex.size() < 3)
                            continue; // Skip outlines
                        // check if the hole vertice is inside the current outline
                        if (Polygon2DUtils::checkPointInside(combined_vertices, hole.vertex[0]))
                            // Connect the hole to the outline
                            connectHole(combined_vertices, hole.vertex);
                    }

                    triangles_aux.clear();
                    // Triangulate the combined vertices
                    earClipping(combined_vertices, triangles_aux);

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