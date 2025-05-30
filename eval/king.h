/**
 * @license
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Volker B�hm
 * @copyright Copyright (c) 2021 Volker B�hm
 * @Overview
 * Implements a static class to evaluate knights 
 */

#pragma once

#include <cstdint>
#include "../basics/types.h"
#include "../movegenerator/bitboardmasks.h"
#include "../movegenerator/movegenerator.h"
#include "../basics/evalvalue.h"
#include "../basics/pst.h"
#include "evalresults.h"

using namespace QaplaBasics;
using namespace QaplaMoveGenerator;

namespace ChessEval {


	class King {
	public:

		static EvalValue eval(const MoveGenerator& position, EvalResults& results) {
			return evalColor<WHITE, false>(position, results, nullptr) - evalColor<BLACK, false>(position, results, nullptr);
		}

		static EvalValue evalWithDetails(const MoveGenerator& position, EvalResults& results, std::vector<PieceInfo>& details) {
			return evalColor<WHITE, true>(position, results, &details) - evalColor<BLACK, true>(position, results, &details);
		}

		static IndexLookupMap getIndexLookup() {
			IndexLookupMap indexLookup;
			indexLookup["kPST"] = PST::getPSTLookup(KNIGHT);
			return indexLookup;
		}

		/*
		 * Computes the minimum abstract distance between a king and any pawn.
		 *
		 * The distance is calculated using precomputed bitboard masks: king_distance_masks[square][i],
		 * where each mask is a union of concentric zones around the given square.
		 * Each mask[i] fully includes all closer distances.
		 *
		 * The function checks the pawn bitboard against the distance masks using a grouped form
		 * of binary search over distance zones to efficiently find the closest pawn.
		 *
		 * @param kingSquare The square of the king
		 * @param pawns A bitboard with all pawn positions set
		 * @return The distance to the closest pawn:
		 *         0 = adjacent pawn,
		 *         1�6 = increasing distance,
		 *         0 also if no pawns are present
		 */
		static value_t minDistance(Square kingSquare, bitBoard_t pawns) {
			if (pawns == 0) {
				return 0;
			}
			const auto& masks = king_distance_masks[kingSquare];
			// The order is important: we must check the smallest distance masks first
			// to ensure we return the minimal distance to any pawn.
			if (pawns & masks[2]) {
				return (pawns & masks[0]) ? 0 : (pawns & masks[1]) ? 1 : 2;
			}
			else if (pawns & masks[4]) {
				return (pawns & masks[3]) ? 3 : 4;
			}
			else {
				return (pawns & masks[5]) ? 5 : 6;
			}
		}
	private:

		/**
		 * Evaluates Kings
		 */
		template<Piece COLOR, bool STORE_DETAILS>
		static EvalValue evalColor(const MoveGenerator& position, EvalResults& results, std::vector<PieceInfo>* details) {
			const Square kingSquare = position.getKingSquare<COLOR>();
			const value_t kingDistance = minDistance(kingSquare, position.getPieceBB(PAWN + COLOR));
			const EvalValue propertyValue = EvalValue(0, kingDistance * DISTANCE_PENALTY);
			if constexpr (STORE_DETAILS) {
				const auto materialValue = 0;
				const auto pstValue = PST::getValue(kingSquare, KING + COLOR);
				const auto mobility = 0;
				const auto property = COLOR == WHITE ? propertyValue : -propertyValue;
				details->push_back({ KING + COLOR, kingSquare, {}, "", property + pstValue });
			}
			return propertyValue;
		}

		static inline std::array<std::array<uint64_t, 6>, 64> king_distance_masks = [] {
			std::array<std::array<uint64_t, 6>, 64> result{};

			for (int king = 0; king < 64; ++king) {
				for (int sq = 0; sq < 64; ++sq) {
					int d = std::max(std::abs(king % 8 - sq % 8), std::abs(king / 8 - sq / 8));
					if (d > 0 && d <= 6)
						result[king][d - 1] |= 1ULL << sq;
				}
				result[king][2] |= result[king][0] | result[king][1];
				result[king][4] |= result[king][3];
			}

			return result;
		}();

		static const value_t DISTANCE_PENALTY = -10;
	};
}


