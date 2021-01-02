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
 * @author Volker Böhm
 * @copyright Copyright (c) 2021 Volker Böhm
 * @Overview
 * Implements basic algorithms
 * doMove
 * undoMove
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "types.h"
#include "move.h"
#include "basicboard.h"
#include "boardstate.h"

namespace ChessBasics {

	class Board {
	public:
		Board();
		void doMove(Move move);
		void undoMove(Move move, BoardState boardState);
		void clear();
		inline auto getEP() { return basicBoard.getEP(); }
		inline auto operator[](Square square) { return basicBoard[square]; }
		inline auto isWhiteToMove() { return basicBoard.whiteToMove; }
		inline void setWhiteToMove(bool whiteToMove) { basicBoard.whiteToMove = whiteToMove; }

		/**
		 * Checks, if king side castling is allowed
		 */
		template <Piece COLOR>
		inline bool isKingSideCastleAllowed() {
			return basicBoard.isKingSideCastleAllowed<COLOR>();
		}

		/**
		 * Checks, if queen side castling is allowed
		 */
		template <Piece COLOR>
		inline bool isQueenSideCastleAllowed() {
			return basicBoard.isQueenSideCastleAllowed<COLOR>();
		}

		/**
		 * Enable/Disable castling right
		 */
		void setCastlingRight(Piece color, bool kingSide, bool allow) {
			basicBoard.setCastlingRight(color, kingSide, allow);
		}

		/**
		 * Sets a piece to the board adjusting all state variables
		 */
		void setPiece(Square square, Piece piece) {
			addPiece(square, piece);
			if (piece == WHITE_KING) {
				kingSquares[WHITE] = square;
			}
			if (piece == BLACK_KING) {
				kingSquares[BLACK] = square;
			}
		}

		BoardState getBoardState() { return basicBoard.boardState; }
		BasicBoard basicBoard;

	protected:
		array<Square, 2> kingSquares;

		array<bitBoard_t, PIECE_AMOUNT> bitBoardsPiece;
		array<bitBoard_t, 2> bitBoardAllPiecesOfOneColor;
		bitBoard_t bitBoardAllPieces;

	private:
		/**
		 * Clears the bitboards
		 */
		void clearBB() {
			bitBoardsPiece.fill(0);
			bitBoardAllPiecesOfOneColor.fill(0);
			bitBoardAllPieces = 0;
		}

		/**
		 * Moves a piece as part of a move
		 */
		void movePiece(Square departure, Square destination);

		/**
		 * Remove current piece as part of a move
	  	 */
		void removePiece(Square squareOfPiece);

		/**
		 * Adds a piece as part of a move (for example for promotions)
		 */
		void addPiece(Square squareOfPiece, Piece pieceToAdd);


		/**
		 * Removes a piece from the bitboards
		 */
		void removePieceBB(Square squareOfPiece, Piece pieceToRemove) {
			bitBoard_t clearMask = ~(1ULL << squareOfPiece);
			bitBoardsPiece[pieceToRemove] &= clearMask;
			bitBoardAllPiecesOfOneColor[getPieceColor(pieceToRemove)] &= clearMask;
			bitBoardAllPieces &= clearMask;
		}

		/**
		 * Adds a piece to the bitboards
		 */
		void addPieceBB(Square squareOfPiece, Piece pieceToAdd) {
			bitBoard_t setMask = (1ULL << squareOfPiece);
			bitBoardsPiece[pieceToAdd] |= setMask;
			bitBoardAllPiecesOfOneColor[getPieceColor(pieceToAdd)] |= setMask;
			bitBoardAllPieces |= setMask;
		}

		/**
		 * Moves a piece on the bitboards
		 */
		void movePieceBB(Square departure, Square destination, Piece pieceToMove) {
			bitBoard_t moveMask = (1ULL << destination) + (1ULL << departure);
			bitBoardsPiece[pieceToMove] ^= moveMask;
			bitBoardAllPiecesOfOneColor[getPieceColor(pieceToMove)] ^= moveMask;
			bitBoardAllPieces ^= moveMask;
		}

		/**
		 * handles EP, Castling, Promotion 
		 */
		void doMoveSpecialities(Move move);
		void undoMoveSpecialities(Move move);

	};
}

#endif __BOARD_H

