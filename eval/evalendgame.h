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
 * Implements chess board evaluation for end games. 
 * Returns +100, if white is one pawn up
 */

#ifndef __EVALENDGAME_H
#define __EVALENDGAME_H

#include <string>
#include "../basics/move.h"
#include "../movegenerator/movegenerator.h"
//#include "BitBaseIndex.h"
//#include "BitBaseReader.h"
#include "evalpawn.h"

using namespace std;

namespace ChessEval {

	class EvalEndgame {

	public:

		/**
		 * Provides end game evaluation
		 */
		static value_t eval(MoveGenerator& board, value_t currentValue) {

			uint8_t functionNo = mapPieceSignatureToFunctionNo[board.getPiecesSignature()];
			if (functionNo < functionAmount) {
				currentValue = functionMap[functionNo](board, currentValue);
			}

			return currentValue;
		}

		/**
		 * Prints end game evaluation terms to stdout
		 */
		static value_t print(MoveGenerator& board, value_t currentValue) {
			value_t newValue = eval(board, currentValue);
			if (currentValue != newValue) {
				printf("Eval endgame mod    : %ld => %ld\n", currentValue, newValue);
			}
			return newValue;
		}


	private:
		typedef value_t evalFunction_t(MoveGenerator& board, value_t currentValue);

		/**
		 * Register an endgame evaluation funciton for a dedicated piece selection
		 */
		static void registerFunction(string pieces, evalFunction_t function, bool changeSide = false);

		/**
		 * Forces a draw value
		 */
		template <Piece COLOR>
		static value_t drawValue(MoveGenerator& board, value_t currentValue);

		/**
		 * Forces a winning value
		 */
		template <Piece COLOR>
		static value_t winningValue(MoveGenerator& board, value_t currentValue);

		/**
		 * Evaluate material balance and pawn structure
		 */
		static value_t materialAndPawnStructure(MoveGenerator& board);


		template <Piece COLOR>
		static value_t KQKR(MoveGenerator& board, value_t currentValue);

		template <Piece COLOR>
		static value_t KQPsKRPs(MoveGenerator& board, value_t currentValue);

		// template <Piece COLOR>
		// static value_t KPsK(MoveGenerator& board, value_t currentValue);

		static value_t KPsKPs(MoveGenerator& board, value_t currentValue);

		template <Piece COLOR>
		static value_t KBNK(MoveGenerator& board, value_t currentValue);

		template <Piece COLOR>
		static value_t KBBK(MoveGenerator& board, value_t currentValue);

		template <Piece COLOR>
		static value_t KBsPsK(MoveGenerator& board, value_t currentValue);

		template <Piece COLOR>
		static value_t KNPsK(MoveGenerator& board, value_t currentValue);

		/**
		 * Tries to trap the king in any corner
		 */
		template <Piece COLOR>
		static value_t forceToAnyCorner(MoveGenerator& board, value_t currentValue);

		/**
		 * Tries to trap the opponent king in a white or black corner
		 */
		template <Piece COLOR>
		static value_t forceToCorrectCorner(MoveGenerator& board, value_t currentValue, bool whiteCorner);


		template <Piece COLOR>
		static value_t ForceToCornerWithBonus(MoveGenerator& board, value_t currentValue);

		/**
		 * Check, if a bishop is able to attack the promotion field of a pawn
		 */
		template <Piece COLOR>
		static bool bishopIsAbleToAttacksPromotionField(Square column, bitBoard_t bishops);

		/**
		 * Checks, if a square is set in a bit board
		 */
		template <Piece COLOR>
		inline static bool isSquareInBB(Square position, bitBoard_t mask);

		// template <Piece COLOR>
		// static value_t getValueFromBitBase(uint64_t index, const BitBase& bitBase, value_t currentValue);

		/**
		 * Static loopup tables initializer
		 */
		static struct InitStatics {
			InitStatics();
		} _staticConstructor;

		/**
		 * Computes the distance between two squares
		 */
		static value_t computeDistance(Square pos1, Square pos2);

		/**
		 * Computes the distance between two kings
		 */
		static value_t computeKingDistance(MoveGenerator& board);

		/**
		 * Computes the distance to any border
		 */
		static value_t computeDistanceToBorder(Square kingPos);

		/**
		 * Computes the distance to any corner
		 */
		static value_t computeDistanceToAnyCorner(Square kingPos);

		/**
		 * Computes the distance to a white or black corner
		 */
		static value_t computeDistanceToCorrectCorner(Square kingPos, bool whiteCorner);

		static const bitBoard_t WHITE_FIELDS = 0x55AA55AA55AA55AAULL;
		static const bitBoard_t BLACK_FIELDS = 0xAA55AA55AA55AA55ULL;

		static constexpr value_t BONUS[COLOR_AMOUNT] = { WINNING_BONUS, -WINNING_BONUS };
		static constexpr Square UP[COLOR_AMOUNT] = { NORTH, SOUTH };
		static constexpr value_t COLOR_VALUE[COLOR_AMOUNT] = { 1, -1 };
		static constexpr value_t RUNNER_VALUE[NORTH] = { 0, 0, 100,  150, 200, 300, 500, 0 };
		static const value_t KING_RACED_PAWN_BONUS = 150;


		static const uint8_t MAX_EVAL_FUNCTIONS = 50;
		static evalFunction_t* functionMap[MAX_EVAL_FUNCTIONS];
		static uint8_t functionAmount;

		static uint8_t mapPieceSignatureToFunctionNo[PieceSignature::PIECE_SIGNATURE_SIZE];

	};

}

#endif