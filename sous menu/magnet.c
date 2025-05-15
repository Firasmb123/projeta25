#include "magnet.h"
#include <math.h>
#include <stdio.h>
#define SNAP_DISTANCE 400

void MagnetizePieces(Puzzle* puzzle) {
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            PuzzlePiece* piece = &puzzle->pieces[i][j];
            if (piece->isPlaced) continue;
            int dx = piece->currentPos.x - piece->targetPos.x;
            int dy = piece->currentPos.y - piece->targetPos.y;
            float distance = sqrt(dx * dx + dy * dy);
            printf("Piece [%d][%d]: distance = %f, currentPos = (%d, %d), targetPos = (%d, %d)\n", 
                   i, j, distance, piece->currentPos.x, piece->currentPos.y, 
                   piece->targetPos.x, piece->targetPos.y);

            if (distance < SNAP_DISTANCE) {
                piece->currentPos.x = piece->targetPos.x;
                piece->currentPos.y = piece->targetPos.y;
                piece->isPlaced = 1;
                printf("Piece [%d][%d] placed at target position.\n", i, j);
            }
        }
    }
}
