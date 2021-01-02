/**
 * @license
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @Overview
 * Implements an algorithm to select the right move based on an incomplete move notation
 */

#include "../basics/types.h"
#include "../basics/move.h"
#include "../movegenerator/movegenerator.h"

class MoveConverter {
private:
	MoveConverter() {}
public:
	/*
	static move_t convertMove(GenBoard& board, const char* moveStr) {
		MoveScanner scanner(moveStr);
		move_t move = Move::EMPTY_MOVE;

		if (scanner.isLegal()) {
			move = findMove(board, scanner.piece, scanner.fromCol, scanner.fromRow, scanner.toCol, scanner.toRow, scanner.promote);
		}
		return move;
	}
	*/

	static move_t findMove(GenBoard& board, char movingPiece, pos_t fromCol, pos_t fromRow, pos_t toCol, pos_t toRow, char promotePiece) {
		MoveList moveList;
		move_t move = Move::EMPTY_MOVE;

		uint16_t moveNoFound = 0;
		board.genMovesOfMovingColor(moveList);
		if (promotePiece >= 'a' && promotePiece <= 'z') {
			promotePiece -= 'a' - 'A';
		}
		
		for (uint16_t moveNo = 0; moveNo < moveList.moveAmount; moveNo++) {
			if ((movingPiece == 0 || Move::getMovingPiece(moveList[moveNo]) == charToPiece(board.whiteToMove, movingPiece)) &&
				(fromCol == -1 || Move::getStartPosCol(moveList[moveNo]) == fromCol) &&
				(fromRow == -1 || Move::getStartPosRow(moveList[moveNo]) == fromRow) &&
				(toCol == -1 || Move::getTargetPosCol(moveList[moveNo]) == toCol) &&
				(toRow == -1 || Move::getTargetPosRow(moveList[moveNo]) == toRow) &&
				(Move::getPromotePiece(moveList[moveNo]) == charToPiece(board.whiteToMove, promotePiece)))
			{
				if (move != Move::EMPTY_MOVE) {
					// more than one possible moves found
					move = Move::EMPTY_MOVE;
					break;
				}
				move = moveList[moveNo];
			}
		}
		return move;
	}

	static piece_t charToPiece(char pieceChar) {
		piece_t res = NO_PIECE;
		switch (pieceChar) {
		case 'P':res = WHITE_PAWN; break;
		case 'N':res = WHITE_KNIGHT; break;
		case 'B':res = WHITE_BISHOP; break;
		case 'K':res = WHITE_KING; break;
		case 'R':res = WHITE_ROOK; break;
		case 'Q':res = WHITE_QUEEN; break;
		case 'p':res = BLACK_PAWN; break;
		case 'n':res = BLACK_KNIGHT; break;
		case 'b':res = BLACK_BISHOP; break;
		case 'k':res = BLACK_KING; break;
		case 'r':res = BLACK_ROOK; break;
		case 'q':res = BLACK_QUEEN; break;
		case 'x':res = NO_PIECE; break;
		}
		return res;
	}


	static char pieceToChar(piece_t piece) {
		switch (piece) {
		case WHITE_PAWN: return 'P';
		case BLACK_PAWN: return 'p';
		case WHITE_KNIGHT: return 'N';
		case BLACK_KNIGHT: return 'n';
		case WHITE_BISHOP: return 'B';
		case BLACK_BISHOP: return 'b';
		case WHITE_ROOK: return 'R';
		case BLACK_ROOK: return 'r';
		case WHITE_QUEEN: return 'Q';
		case BLACK_QUEEN: return 'q';
		case WHITE_KING: return 'K';
		case BLACK_KING: return 'k';
		default: return '-';
		}
	}


private:

	static piece_t charToPiece(bool whiteToMove, char pieceChar) {
		piece_t res = NO_PIECE;
		/*
		if (pieceChar >= 'a' && pieceChar <= 'z') {
			pieceChar -= 'a';
			pieceChar += 'A';
		}
		*/
		if (whiteToMove) {
			switch (pieceChar) {
			case 'P':res = WHITE_PAWN; break;
			case 'N':res = WHITE_KNIGHT; break;
			case 'B':res = WHITE_BISHOP; break;
			case 'K':res = WHITE_KING; break;
			case 'R':res = WHITE_ROOK; break;
			case 'Q':res = WHITE_QUEEN; break;
			case 'x':res = NO_PIECE; break;
			}
		}
		else {
			switch (pieceChar) {
			case 'P':res = BLACK_PAWN; break;
			case 'N':res = BLACK_KNIGHT; break;
			case 'B':res = BLACK_BISHOP; break;
			case 'K':res = BLACK_KING; break;
			case 'R':res = BLACK_ROOK; break;
			case 'Q':res = BLACK_QUEEN; break;
			case 'x':res = NO_PIECE; break;
			}
		}
		return res;
	}



};