#pragma once

#include "../../common.h"
#include "../../ITKCommon/Matrix.h"
#include "../../ITKCommon/Random.h"
#include "../../CollisionCore/CollisionCore.h"

namespace AlgorithmCore
{

	namespace Procedural
	{

		namespace RoguelikeMatrix
		{

			enum class Element : uint8_t
			{
				NonWalkPlace,
				WalkPlace,
				Edge
			};

			class Area
			{
			public:
				MathCore::vec2i min;
				MathCore::vec2i max;

				Area()
				{
				}

				Area(const MathCore::vec2i &min, const MathCore::vec2i &max)
				{
					this->min = MathCore::OP<MathCore::vec2i>::minimum(min, max);
					this->max = MathCore::OP<MathCore::vec2i>::maximum(min, max);
				}

				MathCore::vec2i origin() const
				{
					return min;
				}

				MathCore::vec2i size() const
				{
					return max - min + 1;
				}

				// ignoring walls
				static bool internalOverlapsX(const Area &a, const Area &b)
				{
					if (a.max.x < b.min.x || a.min.x > b.max.x)
						return false;
					return true;
				}
				// ignoring walls
				static bool internalOverlapsY(const Area &a, const Area &b)
				{
					if (a.max.y < b.min.y || a.min.y > b.max.y)
						return false;
					return true;
				}

				// considering walls
				static bool externalOverlaps(const Area &a, const Area &b)
				{
					if (a.max.x < b.min.x - 1 || a.min.x > b.max.x + 1)
						return false;
					if (a.max.y < b.min.y - 1 || a.min.y > b.max.y + 1)
						return false;
					return true;
				}

				static Area fromRandomRange(
					const MathCore::vec2i &random_min_size,
					const MathCore::vec2i &random_max_size)
				{
					MathCore::vec2i min(0, 0);
					MathCore::vec2i max(
						ITKCommon::Random::Instance()->getRange(random_min_size.x, random_max_size.x),
						ITKCommon::Random::Instance()->getRange(random_min_size.y, random_max_size.y));
					return Area(min, max);
				}

				void moveMinToPoint(const MathCore::vec2i &_min)
				{
					max = max - min + _min;
					min = _min;
				}

				MathCore::vec2i randomPoint() const
				{
					return MathCore::vec2i(
						ITKCommon::Random::Instance()->getRange(min.x, max.x),
						ITKCommon::Random::Instance()->getRange(min.y, max.y));
				}
			};

			enum class ConnectionDrawingOrder : uint8_t
			{
				Iterate_X_First = 0,
				Iterate_y_First = 1,
				StraitLine = 2
			};

			class Connection
			{
			public:
				MathCore::vec2i a;
				MathCore::vec2i b;

				ConnectionDrawingOrder drawingOrder;

				Connection firstConnection() const
				{
					if (drawingOrder == ConnectionDrawingOrder::Iterate_X_First)
					{
						Connection result;
						result.a = a;
						result.b.x = a.x;
						result.b.y = b.y;
						return result;
					}
					else if (drawingOrder == ConnectionDrawingOrder::Iterate_y_First)
					{
						Connection result;
						result.a = a;
						result.b.x = b.x;
						result.b.y = a.y;
						return result;
					}
					return *this;
				}

				Connection secondConnection() const
				{
					if (drawingOrder == ConnectionDrawingOrder::Iterate_X_First)
					{
						Connection result;
						result.a.x = a.x;
						result.a.y = b.y;
						result.b = b;
						return result;
					}
					else if (drawingOrder == ConnectionDrawingOrder::Iterate_y_First)
					{
						Connection result;
						result.a.x = b.x;
						result.a.y = a.y;
						result.b = b;
						return result;
					}
					return *this;
				}

				Connection()
				{
					drawingOrder = ConnectionDrawingOrder::StraitLine;
				}

				Connection(const MathCore::vec2i &_a, const MathCore::vec2i &_b)
				{
					drawingOrder = (ConnectionDrawingOrder)ITKCommon::Random::Instance()->getRange(0, 1);

					a = _a;
					b = _b;
				}

				Connection(const Area &areaA, const Area &areaB)
				{
					a = areaA.randomPoint();
					b = areaB.randomPoint();

					if (Area::internalOverlapsY(areaA, areaB))
					{
						int32_t _all_y[4] = {
							areaA.min.y, areaA.max.y,
							areaB.min.y, areaB.max.y};
						std::sort(std::begin(_all_y), std::end(_all_y));
						int32_t _min = _all_y[1];
						int32_t _max = _all_y[2];
						a.y = b.y = ITKCommon::Random::Instance()->getRange(_min, _max);
						drawingOrder = ConnectionDrawingOrder::StraitLine;
					}
					else if (Area::internalOverlapsX(areaA, areaB))
					{
						int32_t _all_x[4] = {
							areaA.min.x, areaA.max.x,
							areaB.min.x, areaB.max.x};
						std::sort(std::begin(_all_x), std::end(_all_x));
						int32_t _min = _all_x[1];
						int32_t _max = _all_x[2];
						a.x = b.x = ITKCommon::Random::Instance()->getRange(_min, _max);
						drawingOrder = ConnectionDrawingOrder::StraitLine;
					}
					else
						drawingOrder = (ConnectionDrawingOrder)ITKCommon::Random::Instance()->getRange(0, 1);
				}
			};

			template <typename T>
			class World
			{
			public:
				ITKCommon::Matrix<Element> element_matrix;
				// need to set this before begin gen algorithm
				std::map<Element, T> element_map;
				ITKCommon::Matrix<T> final_matrix;
				
				std::vector<Area> areas;
				std::vector<Connection> connections;

				World()
				{
				}

				void generate(const MathCore::vec2i &matrix_size,
							  int max_areas,
							  const MathCore::vec2i &_area_size_min,
							  const MathCore::vec2i &_area_size_max,
							  int random_connections = 0,
							  bool allow_overlap = false)
				{
					element_matrix.setSize(matrix_size);

					Area worldLimit(MathCore::vec2i(1, 1), MathCore::vec2i(matrix_size.x - 2, matrix_size.y - 2));
					MathCore::vec2i world_size = worldLimit.size();

					MathCore::vec2i area_size_min = MathCore::OP<MathCore::vec2i>::minimum(_area_size_min, world_size);
					MathCore::vec2i area_size_max = MathCore::OP<MathCore::vec2i>::minimum(_area_size_max, world_size);

					//std::vector<Area> areas;
					areas.clear();

					// generate areas
					for (int i = 0; i < max_areas; i++)
					{
						// generate random new area
						Area newArea = Area::fromRandomRange(area_size_min, area_size_max);
						MathCore::vec2i newArea_size = newArea.size();
						MathCore::vec2i random_range_size = world_size - newArea_size;
						MathCore::vec2i random_origin = MathCore::vec2i(
															ITKCommon::Random::Instance()->getRange(0, random_range_size.x),
															ITKCommon::Random::Instance()->getRange(0, random_range_size.y)) +
														worldLimit.origin();
						newArea.moveMinToPoint(random_origin);

						if (allow_overlap)
							areas.push_back(newArea);
						else
						{
							bool overlap = false;
							for (const auto &_a : areas)
							{
								if (Area::externalOverlaps(newArea, _a))
								{
									overlap = true;
									break;
								}
							}
							int max_tries = 100;
							while (overlap && max_tries > 0)
							{
								// generate random new area
								newArea = Area::fromRandomRange(area_size_min, area_size_max);
								newArea_size = newArea.size();
								random_range_size = world_size - newArea_size;
								random_origin = MathCore::vec2i(
													ITKCommon::Random::Instance()->getRange(0, random_range_size.x),
													ITKCommon::Random::Instance()->getRange(0, random_range_size.y)) +
												worldLimit.origin();
								newArea.moveMinToPoint(random_origin);

								overlap = false;
								for (const auto &_a : areas)
								{
									if (Area::externalOverlaps(newArea, _a))
									{
										overlap = true;
										break;
									}
								}
								max_tries--;
							}
							if (!overlap)
								areas.push_back(newArea);
						}
					}

					element_matrix.clear(Element::NonWalkPlace);
					// paint areas
					for (const auto &area : areas)
					{
						for (int y = area.min.y; y <= area.max.y; y++)
							for (int x = area.min.x; x <= area.max.x; x++)
								element_matrix[y][x] = Element::WalkPlace;
					}

					// connect areas
					//std::vector<Connection> connections;
					connections.clear();

					for (int i = 0; i < (int)areas.size() - 1; i++)
					{
						const Area &areaA = areas[i];
						const Area &areaB = areas[i + 1];
						Connection conn(areaA, areaB);
						if (conn.drawingOrder == ConnectionDrawingOrder::StraitLine)
							connections.push_back(conn);
						else
						{
							connections.push_back(conn.firstConnection());
							connections.push_back(conn.secondConnection());
						}
					}

					// random connections
					for (int i = 0; i < random_connections && areas.size() > 1; i++)
					{
						int a_idx = ITKCommon::Random::Instance()->getRange(0, (int)areas.size() - 1);
						int b_idx = ITKCommon::Random::Instance()->getRange(0, (int)areas.size() - 1);
						while (b_idx == a_idx)
							b_idx = ITKCommon::Random::Instance()->getRange(0, (int)areas.size() - 1);

						const Area &areaA = areas[a_idx];
						const Area &areaB = areas[b_idx];
						Connection conn(areaA, areaB);
						if (conn.drawingOrder == ConnectionDrawingOrder::StraitLine)
							connections.push_back(conn);
						else
						{
							connections.push_back(conn.firstConnection());
							connections.push_back(conn.secondConnection());
						}
					}

					// draw connections
					struct BresenhamDraw
					{
						ITKCommon::Matrix<Element> &element_matrix;
						inline void draw_pixel(int x, int y) noexcept
						{
							element_matrix[y][x] = Element::WalkPlace;
						}
					} DrawStructure{element_matrix};

					for (const auto &conn : connections)
					{
						if (conn.drawingOrder == ConnectionDrawingOrder::StraitLine)
							AlgorithmCore::Rasterization::BresenhamIterator::Raster(DrawStructure, conn.a, conn.b);
						else
						{
							Connection connA = conn.firstConnection();
							Connection connB = conn.secondConnection();
							AlgorithmCore::Rasterization::BresenhamIterator::Raster(DrawStructure, connA.a, connA.b);
							AlgorithmCore::Rasterization::BresenhamIterator::Raster(DrawStructure, connB.a, connB.b);
						}
					}

					// compute walls
					for (int row = worldLimit.min.y; row <= worldLimit.max.y; row++)
					{
						for (int col = worldLimit.min.x; col <= worldLimit.max.x; col++)
						{
							if (element_matrix[row][col] == Element::WalkPlace)
							{
								for (int i = -1; i <= 1; i++)
								{
									for (int j = -1; j <= 1; j++)
									{
										if (i == 0 && j == 0)
											continue;
										if (element_matrix[row + i][col + j] == Element::NonWalkPlace)
											element_matrix[row + i][col + j] = Element::Edge;
									}
								}
							}
						}
					}

					// create the final matrix
					final_matrix.setSize(element_matrix.size);
					for (int row_num = 0; row_num < final_matrix.size.height; row_num++)
					{
						for (int col_num = 0; col_num < final_matrix.size.width; col_num++)
						{
							Element element = element_matrix[row_num][col_num];
							final_matrix[row_num][col_num] = element_map[element];
						}
					}
				}


				MathCore::vec2i randomAreaPoint(){
					int area_idx = ITKCommon::Random::Instance()->getRange(0, (int)areas.size() - 1);
					return areas[area_idx].randomPoint();
				}


			};

		}

	}
}
