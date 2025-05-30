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

#ifndef __KNIGHT_H
#define __KNIGHT_H

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


	class Knight {
	public:

		static EvalValue eval(const MoveGenerator& position, EvalResults& results) {
			return evalColor<WHITE, false>(position, results, nullptr) - evalColor<BLACK, false>(position, results, nullptr);
		}

		static EvalValue evalWithDetails(const MoveGenerator& position, EvalResults& results, std::vector<PieceInfo>& details) {
			return evalColor<WHITE, true>(position, results, &details) - evalColor<BLACK, true>(position, results, &details);
		}

		static IndexLookupMap getIndexLookup() {
			IndexLookupMap indexLookup;
			indexLookup["kMobility"] = std::vector<EvalValue>{ KNIGHT_MOBILITY_MAP.begin(), KNIGHT_MOBILITY_MAP.end()};
			indexLookup["kProperty"] = std::vector<EvalValue>{ KNIGHT_PROPERTY_MAP.begin() , KNIGHT_PROPERTY_MAP.end()};
			indexLookup["kPST"] = PST::getPSTLookup(KNIGHT);
			return indexLookup;
		}
	private:

		/**
		 * Evaluates Knights
		 */
		template<Piece COLOR, bool STORE_DETAILS>
		static EvalValue evalColor(const MoveGenerator& position, EvalResults& results, std::vector<PieceInfo>* details) {
			EvalValue value;
			results.knightAttack[COLOR] = 0;

			bitBoard_t knights = position.getPieceBB(KNIGHT + COLOR);
			bitBoard_t removeBB = ~position.getPiecesOfOneColorBB<COLOR>() & ~position.pawnAttack[opponentColor<COLOR>()];

			while (knights)
			{
				const Square knightSquare = popLSB(knights);
				uint32_t mobilityIndex = calcMobilityIndex<COLOR>(results, knightSquare, removeBB);
				uint32_t propertyIndex = calcKnightProperties<COLOR>(position, knightSquare);

				const auto mobilityValue = EvalValue(KNIGHT_MOBILITY_MAP[mobilityIndex]);
				// const auto mobilityValue = position.getEvalVersion() == 0 ? EvalValue(KNIGHT_MOBILITY_MAP[mobilityIndex]) : CandidateTrainer::getCurrentCandidate().getWeightVector(0)[mobilityIndex];

				EvalValue propertyValue = EvalValue(KNIGHT_PROPERTY_MAP[propertyIndex]);
				//const auto propertyValue = position.getEvalVersion() == 0 ? EvalValue(KNIGHT_PROPERTY_MAP[propertyIndex]) : CandidateTrainer::getCurrentCandidate().getWeightVector(0)[propertyIndex];

				value += mobilityValue;
				value += propertyValue;

				if constexpr (STORE_DETAILS) {
					const auto materialValue = position.getPieceValue(KNIGHT + COLOR);
					const auto pstValue = PST::getValue(knightSquare, KNIGHT + COLOR);
					const auto mobility = COLOR == WHITE ? mobilityValue : -mobilityValue;
					const auto property = COLOR == WHITE ? propertyValue : -propertyValue;
					IndexVector indexVector{ { "kMobility", mobilityIndex, COLOR },
						{ "kPST", uint32_t(switchSideToWhite<COLOR>(knightSquare)), COLOR },
						{ "material", KNIGHT, COLOR } };
					if (propertyIndex) {
						indexVector.push_back({ "kProperty", propertyIndex, COLOR });
					}
						
					const auto value = mobility + property + materialValue + pstValue;
					details->push_back({ KNIGHT + COLOR, knightSquare, indexVector, KNIGHT_PROPERTY_INFO[propertyIndex], value });
				}

			}
			return value;
		}

		/**
		 * Calculates the mobility index of a knight and adds the attacks of the knight to the attack bitboards in results
		 */
		template<Piece COLOR>
		static inline uint32_t calcMobilityIndex(EvalResults& results, Square square, bitBoard_t removeBB) {
			bitBoard_t attackBB = BitBoardMasks::knightMoves[square];
			results.knightAttack[COLOR] |= attackBB;
			results.piecesDoubleAttack[COLOR] |= results.piecesAttack[COLOR] & attackBB;
			results.piecesAttack[COLOR] |= attackBB;
			attackBB &= removeBB;
			return popCount(attackBB);
		}

		/**
		 * Checks, if a knight is an outpost - i.e. a Knight in the enemy teretory protected by a pawn
		 * and not attackable by a pawn
		 */
		template<Piece COLOR>
		static constexpr uint32_t isOutpost(Square square, bitBoard_t opponentPawnsBB, bitBoard_t pawnAttackBB) {
			bool result = false;
			bitBoard_t knightBB = squareToBB(square);
			bool isProtectedByPawnAndInOpponentArea =
				(knightBB & OUTPOST_BB[COLOR] & pawnAttackBB) != 0;
			if (isProtectedByPawnAndInOpponentArea) {
				bitBoard_t shift = COLOR == WHITE ? (square + NW) : (square + SOUTH + SW);
				bitBoard_t opponentPawnCheckBB = 0x505ULL << shift;
				result = (opponentPawnCheckBB & opponentPawnsBB) == 0;
			}
			return result;
		}

		/**
		 * Returns true, if the knight is pinned
		 */
		static constexpr uint32_t isPinned(bitBoard_t pinnedBB, Square square) {
			return (pinnedBB & squareToBB(square)) != 0;
		}

		/**
		 * Calculates properties and their Values for Knights
		 */
		template<Piece COLOR>
		static inline uint32_t calcKnightProperties(const MoveGenerator& position, Square knightSquare)
		{
			const bitBoard_t opponentPawnBB = position.getPieceBB(PAWN + opponentColor<COLOR>());
			uint32_t knightIndex = isOutpost<COLOR>(knightSquare, opponentPawnBB, position.pawnAttack[COLOR]) * OUTPOST;
			knightIndex += isPinned(position.pinnedMask[COLOR], knightSquare) * PINNED;
			return knightIndex;
		}

		// Mobility Map for knights
		static constexpr std::array<EvalValue, 9> KNIGHT_MOBILITY_MAP = { {
			{ -30, -30 }, { -20, -20 }, { -10, -10 }, { 0, 0 }, { 10, 10 },
			{ 20, 20 }, { 25, 25 }, { 25, 25 }, { 25, 25 }
		} };

		static const uint32_t OUTPOST = 1;
		static const uint32_t PINNED = 2;
		
		static constexpr std::array<EvalValue, 4> KNIGHT_PROPERTY_MAP = { {
			 {  0,   0}, { 20, 0}, { -43,  0}, {-23,  0}
		} };

		static inline std::string KNIGHT_PROPERTY_INFO[4] = {
			"", "<otp>", "<pin>", "<pin><otp>"
		};

		static constexpr bitBoard_t OUTPOST_BB[2] = { 0x003C3C3C00000000, 0x000000003C3C3C00 };
	};
}


#endif

