#pragma once

#include "../../common.h"
#include "../../ITKCommon/ITKCommon.h"
#include "../../MathCore/MathCore.h"

namespace AlgorithmCore
{

	namespace Search
	{

		struct Vec2iWrapper
		{
			MathCore::vec2i value;
			int32_t fScorei; // priority score from this value
			// const AStarMatrix* aStarMatrix;

			Vec2iWrapper(const MathCore::vec2i &value, int32_t fScorei)
			{
				this->value = value;
				this->fScorei = fScorei;
			}
		};

		//// priority queue sorting base on this comparator
		// static inline bool operator<(const Vec2iWrapper& a, const Vec2iWrapper& b) {
		//	return a.fScorei > b.fScorei;
		// }

		// custom comparer
		struct Greater_Vec2iWrapper_vector_heap
		{
			inline bool operator()(const Vec2iWrapper &a, const Vec2iWrapper &b) const
			{
				return a.fScorei > b.fScorei;
			}
		};
		const Greater_Vec2iWrapper_vector_heap greater_Vec2iWrapper_vector_heap;

		// custom comparer
		struct less_Vec2iWrapper_set_comparer
		{
			inline bool operator()(const Vec2iWrapper &a, const Vec2iWrapper &b) const
			{
				if (a.fScorei == b.fScorei)
				{
					if (a.value.x == b.value.x)
						return a.value.y < b.value.y;
					return a.value.x < b.value.x;
				}
				return a.fScorei < b.fScorei;
			}
		};

		// namespace AStarMatrixConnectivity
		// {
		// 	struct _8Connected
		// 	{
		// 		static inline constexpr MathCore::vec2i walk[8] = {
		// 			MathCore::vec2i(-1, -1), MathCore::vec2i(0, -1), MathCore::vec2i(1, -1),
		// 			MathCore::vec2i(-1, 0), /* MathCore::vec2i(0, 0 ),*/ MathCore::vec2i(1, 0),
		// 			MathCore::vec2i(-1, 1), MathCore::vec2i(0, 1), MathCore::vec2i(1, 1)};
		// 	};
		// 	struct _4Connected
		// 	{
		// 		static inline constexpr MathCore::vec2i walk[4] = {
		// 			/*MathCore::vec2i(-1, -1),*/ MathCore::vec2i(0, -1), /*MathCore::vec2i(1, -1),*/
		// 			MathCore::vec2i(-1, 0), /* MathCore::vec2i(0, 0 ),*/ MathCore::vec2i(1, 0),
		// 			/*MathCore::vec2i(-1, 1),*/ MathCore::vec2i(0, 1) /*, MathCore::vec2i(1, 1)*/
		// 		};
		// 	};
		// 	struct Diagonals
		// 	{
		// 		static inline constexpr MathCore::vec2i walk[4] = {
		// 			MathCore::vec2i(-1, -1), /*MathCore::vec2i(0, -1),*/ MathCore::vec2i(1, -1),
		// 			/*MathCore::vec2i(-1, 0),*/ /* MathCore::vec2i(0, 0 ),*/ /*MathCore::vec2i(1, 0),*/
		// 			MathCore::vec2i(-1, 1), /*MathCore::vec2i(0, 1),*/ MathCore::vec2i(1, 1)};
		// 	};
		// };

		const MathCore::vec2i AStarMatrix_8NeighborWalk[] = {
			MathCore::vec2i(-1, -1), MathCore::vec2i(0, -1), MathCore::vec2i(1, -1),
			MathCore::vec2i(-1, 0), /* MathCore::vec2i(0, 0 ),*/ MathCore::vec2i(1, 0),
			MathCore::vec2i(-1, 1), MathCore::vec2i(0, 1), MathCore::vec2i(1, 1)};
		const MathCore::vec2i AStarMatrix_4NeighborWalk[] = {
			/*MathCore::vec2i(-1, -1),*/ MathCore::vec2i(0, -1), /*MathCore::vec2i(1, -1),*/
			MathCore::vec2i(-1, 0), /* MathCore::vec2i(0, 0 ),*/ MathCore::vec2i(1, 0),
			/*MathCore::vec2i(-1, 1),*/ MathCore::vec2i(0, 1) /*, MathCore::vec2i(1, 1)*/};
		const MathCore::vec2i AStarMatrix_Diagonals[] = {
			MathCore::vec2i(-1, -1), /*MathCore::vec2i(0, -1),*/ MathCore::vec2i(1, -1),
			/*MathCore::vec2i(-1, 0),*/ /* MathCore::vec2i(0, 0 ),*/ /*MathCore::vec2i(1, 0),*/
			MathCore::vec2i(-1, 1), /*MathCore::vec2i(0, 1),*/ MathCore::vec2i(1, 1)};

		namespace AStarMatrixConnectivity
		{
			struct _8Connected;
			struct _4Connected;
			struct Diagonals;
		};

		namespace AStarDistanceFnc
		{
			struct Manhattan
			{
				static ITK_INLINE int32_t do_calculation(const MathCore::vec2i &a, const MathCore::vec2i &b) noexcept
				{
					// Manhattan
					int32_t x = MathCore::OP<int32_t>::abs(a.x - b.x);
					int32_t y = MathCore::OP<int32_t>::abs(a.y - b.y);
					return x + y;
				}
			};
			struct Euclides
			{
				static ITK_INLINE int32_t do_calculation(const MathCore::vec2i &a, const MathCore::vec2i &b) noexcept
				{
					// Sqr Euclides
					int32_t x = (a.x - b.x);
					int32_t y = (a.y - b.y);
					return x * x + y * y;
				}
			};
		}

		// Template Struct Example
		// struct Matrix_Map_TileType_Definition {
		// 	using type = TileType;
		// 	static inline bool canWalkOnThisTile(const type& tile) {
		// 		return tile == TileType::Floor;
		// 	}
		// };

		template <typename InputStructure, typename DistanceCalculation = AStarDistanceFnc::Manhattan, typename HeuristicToGoal = AStarDistanceFnc::Manhattan>
		class AStarMatrix
		{
			using InputTileType = typename InputStructure::type;

			struct InternalData
			{
				// bool already_inserted_in_queue;
				bool already_processed;
				int32_t gScorei; // real actual cost from start until N
				int32_t fScorei; // heuristic distance to the goal
				MathCore::vec2i cameFrom;
			};

			const ITKCommon::Matrix<InputTileType> *map;
			ITKCommon::Matrix<InternalData> aux_map;

			std::vector<Vec2iWrapper> openSet;

			// Indexing OPS
			// bool (*canWalkOnTile_ptr)(const InputTileType&);

			inline bool canWalkOnTile(const MathCore::vec2i &i) const
			{
				return InputStructure::canWalkOnThisTile((*map)[i.y][i.x]);
			}

			inline InternalData &data(const MathCore::vec2i &i)
			{
				return aux_map[i.y][i.x];
			}

			inline const InternalData &data(const MathCore::vec2i &i) const
			{
				return aux_map[i.y][i.x];
			}

		public:
			AStarMatrix()
			{ // bool (*checkCanWalkOnTile)(const InputTileType&)) {
				// this->canWalkOnTile_ptr = checkCanWalkOnTile;
				map = nullptr;
			}

			void setInputMatrix(const ITKCommon::Matrix<InputTileType> *mInput)
			{
				map = mInput;
				aux_map.setSize(map->size);
			}

			static inline int32_t distance_calculation(const MathCore::vec2i &a, const MathCore::vec2i &b)
			{
				return DistanceCalculation::do_calculation(a, b);
			}

			static inline int32_t computeHeuristicToGoal(const MathCore::vec2i &i, const MathCore::vec2i &end)
			{
				return HeuristicToGoal::do_calculation(i, end);
			}

			template <
				typename Connectivity = AStarMatrixConnectivity::_8Connected,
				typename std::enable_if<
					std::is_same<Connectivity, AStarMatrixConnectivity::_8Connected>::value,
					bool>::type = true>
			void computePath(const MathCore::vec2i &start, const MathCore::vec2i &end, std::vector<MathCore::vec2i> *result)
			{
				// https://en.wikipedia.org/wiki/A*_search_algorithm

				InternalData clearData;
				// clearData.already_inserted_in_queue = false;
				clearData.already_processed = false;
				clearData.gScorei = INT32_MAX;
				clearData.fScorei = INT32_MAX;
				clearData.cameFrom = MathCore::vec2i(-1, -1);

				aux_map.clear(clearData);

				InternalData &start_node = data(start);

				start_node.gScorei = 0;
				start_node.fScorei = computeHeuristicToGoal(start, end);
				// start_node.already_inserted_in_queue = true;

				openSet.clear();
				openSet.push_back(Vec2iWrapper(start, start_node.fScorei));
				std::push_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);

				// Connectivity conn_v;
				//  if (connectivity == AStarMatrixConnectivity::_8Connected)
				//  {
				while (openSet.size() > 0)
				{

					std::pop_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);
					MathCore::vec2i current = openSet.back().value;
					openSet.pop_back();

					InternalData &current_data = data(current);

					// skip repeated cells (from the priority queue)
					if (current_data.already_processed)
						continue;
					current_data.already_processed = true;

					if (current == end)
					{
						// construct path reverse order
						// std::list<Vec2i> path;
						std::vector<MathCore::vec2i> path_reverse;
						MathCore::vec2i reverse_iterator = end;
						while (reverse_iterator.x != -1)
						{
							// path.push_front(reverse_iterator);
							path_reverse.push_back(reverse_iterator);
							reverse_iterator = data(reverse_iterator).cameFrom;
						}
						// result->assign(path.begin(), path.end());
						result->assign(path_reverse.rbegin(), path_reverse.rend());
						return;
					}

					// neighbor iteration
					for (const auto &_neighbor : AStarMatrix_8NeighborWalk)
					{

						MathCore::vec2i neighbor = _neighbor + current;
						if (neighbor.x < 0 || neighbor.x >= aux_map.size.width ||
							neighbor.y < 0 || neighbor.y >= aux_map.size.height)
							continue;

						// check if is a floor type
						// if (MapTile(neighbor) != TileType::Floor)
						if (!canWalkOnTile(neighbor))
							continue;

						// continue the algorithm
						int32_t tentative_gScore = current_data.gScorei + distance_calculation(current, neighbor);

						InternalData &neighbor_data = data(neighbor);
						if (tentative_gScore >= neighbor_data.gScorei)
							continue;

						neighbor_data.cameFrom = current;
						neighbor_data.gScorei = tentative_gScore;
						neighbor_data.fScorei = tentative_gScore + computeHeuristicToGoal(neighbor, end);

						// add neighbor to the priority queue, if it does not exists there
						// if (neighbor_data.already_processed)
						// continue;

						neighbor_data.already_processed = false;
						// the fScore of the neighbor was updated... need to insert this node again, or remove the old one
						openSet.push_back(Vec2iWrapper(neighbor, neighbor_data.fScorei));
						std::push_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);
					}
				}
				// not found
				result->clear();
			}

			template <
				typename Connectivity,
				typename std::enable_if<
					std::is_same<Connectivity, AStarMatrixConnectivity::_4Connected>::value,
					bool>::type = true>
			void computePath(const MathCore::vec2i &start, const MathCore::vec2i &end, std::vector<MathCore::vec2i> *result)
			{
				// https://en.wikipedia.org/wiki/A*_search_algorithm

				InternalData clearData;
				// clearData.already_inserted_in_queue = false;
				clearData.already_processed = false;
				clearData.gScorei = INT32_MAX;
				clearData.fScorei = INT32_MAX;
				clearData.cameFrom = MathCore::vec2i(-1, -1);

				aux_map.clear(clearData);

				InternalData &start_node = data(start);

				start_node.gScorei = 0;
				start_node.fScorei = computeHeuristicToGoal(start, end);
				// start_node.already_inserted_in_queue = true;

				openSet.clear();
				openSet.push_back(Vec2iWrapper(start, start_node.fScorei));
				std::push_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);

				// Connectivity conn_v;
				//  if (connectivity == AStarMatrixConnectivity::_8Connected)
				//  {
				while (openSet.size() > 0)
				{

					std::pop_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);
					MathCore::vec2i current = openSet.back().value;
					openSet.pop_back();

					InternalData &current_data = data(current);

					// skip repeated cells (from the priority queue)
					if (current_data.already_processed)
						continue;
					current_data.already_processed = true;

					if (current == end)
					{
						// construct path reverse order
						// std::list<Vec2i> path;
						std::vector<MathCore::vec2i> path_reverse;
						MathCore::vec2i reverse_iterator = end;
						while (reverse_iterator.x != -1)
						{
							// path.push_front(reverse_iterator);
							path_reverse.push_back(reverse_iterator);
							reverse_iterator = data(reverse_iterator).cameFrom;
						}
						// result->assign(path.begin(), path.end());
						result->assign(path_reverse.rbegin(), path_reverse.rend());
						return;
					}

					// neighbor iteration
					for (const auto &_neighbor : AStarMatrix_4NeighborWalk)
					{

						MathCore::vec2i neighbor = _neighbor + current;
						if (neighbor.x < 0 || neighbor.x >= aux_map.size.width ||
							neighbor.y < 0 || neighbor.y >= aux_map.size.height)
							continue;

						// check if is a floor type
						// if (MapTile(neighbor) != TileType::Floor)
						if (!canWalkOnTile(neighbor))
							continue;

						// continue the algorithm
						int32_t tentative_gScore = current_data.gScorei + distance_calculation(current, neighbor);

						InternalData &neighbor_data = data(neighbor);
						if (tentative_gScore >= neighbor_data.gScorei)
							continue;

						neighbor_data.cameFrom = current;
						neighbor_data.gScorei = tentative_gScore;
						neighbor_data.fScorei = tentative_gScore + computeHeuristicToGoal(neighbor, end);

						// add neighbor to the priority queue, if it does not exists there
						// if (neighbor_data.already_processed)
						// continue;

						neighbor_data.already_processed = false;
						// the fScore of the neighbor was updated... need to insert this node again, or remove the old one
						openSet.push_back(Vec2iWrapper(neighbor, neighbor_data.fScorei));
						std::push_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);
					}
				}
				// not found
				result->clear();
			}

			template <
				typename Connectivity,
				typename std::enable_if<
					std::is_same<Connectivity, AStarMatrixConnectivity::Diagonals>::value,
					bool>::type = true>
			void computePath(const MathCore::vec2i &start, const MathCore::vec2i &end, std::vector<MathCore::vec2i> *result)
			{
				// https://en.wikipedia.org/wiki/A*_search_algorithm

				InternalData clearData;
				// clearData.already_inserted_in_queue = false;
				clearData.already_processed = false;
				clearData.gScorei = INT32_MAX;
				clearData.fScorei = INT32_MAX;
				clearData.cameFrom = MathCore::vec2i(-1, -1);

				aux_map.clear(clearData);

				InternalData &start_node = data(start);

				start_node.gScorei = 0;
				start_node.fScorei = computeHeuristicToGoal(start, end);
				// start_node.already_inserted_in_queue = true;

				openSet.clear();
				openSet.push_back(Vec2iWrapper(start, start_node.fScorei));
				std::push_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);

				// Connectivity conn_v;
				//  if (connectivity == AStarMatrixConnectivity::_8Connected)
				//  {
				while (openSet.size() > 0)
				{

					std::pop_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);
					MathCore::vec2i current = openSet.back().value;
					openSet.pop_back();

					InternalData &current_data = data(current);

					// skip repeated cells (from the priority queue)
					if (current_data.already_processed)
						continue;
					current_data.already_processed = true;

					if (current == end)
					{
						// construct path reverse order
						// std::list<Vec2i> path;
						std::vector<MathCore::vec2i> path_reverse;
						MathCore::vec2i reverse_iterator = end;
						while (reverse_iterator.x != -1)
						{
							// path.push_front(reverse_iterator);
							path_reverse.push_back(reverse_iterator);
							reverse_iterator = data(reverse_iterator).cameFrom;
						}
						// result->assign(path.begin(), path.end());
						result->assign(path_reverse.rbegin(), path_reverse.rend());
						return;
					}

					// neighbor iteration
					for (const auto &_neighbor : AStarMatrix_Diagonals)
					{

						MathCore::vec2i neighbor = _neighbor + current;
						if (neighbor.x < 0 || neighbor.x >= aux_map.size.width ||
							neighbor.y < 0 || neighbor.y >= aux_map.size.height)
							continue;

						// check if is a floor type
						// if (MapTile(neighbor) != TileType::Floor)
						if (!canWalkOnTile(neighbor))
							continue;

						// continue the algorithm
						int32_t tentative_gScore = current_data.gScorei + distance_calculation(current, neighbor);

						InternalData &neighbor_data = data(neighbor);
						if (tentative_gScore >= neighbor_data.gScorei)
							continue;

						neighbor_data.cameFrom = current;
						neighbor_data.gScorei = tentative_gScore;
						neighbor_data.fScorei = tentative_gScore + computeHeuristicToGoal(neighbor, end);

						// add neighbor to the priority queue, if it does not exists there
						// if (neighbor_data.already_processed)
						// continue;

						neighbor_data.already_processed = false;
						// the fScore of the neighbor was updated... need to insert this node again, or remove the old one
						openSet.push_back(Vec2iWrapper(neighbor, neighbor_data.fScorei));
						std::push_heap(openSet.begin(), openSet.end(), greater_Vec2iWrapper_vector_heap);
					}
				}
				// not found
				result->clear();
			}
		};

	}
}