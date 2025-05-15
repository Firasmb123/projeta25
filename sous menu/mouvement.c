#include "mouvement.h"
void InitMovementManager(MovementManager* manager) {
    manager->selectedPiece = NULL;
}

void HandleMouseDown(MovementManager* manager, Puzzle* puzzle, int mouseX, int mouseY) {
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            PuzzlePiece* piece = &puzzle->pieces[i][j];
            if (mouseX >= piece->currentPos.x && mouseX < piece->currentPos.x + piece->currentPos.w &&
                mouseY >= piece->currentPos.y && mouseY < piece->currentPos.y + piece->currentPos.h) {
                manager->selectedPiece = piece;
                manager->offsetX = mouseX - piece->currentPos.x;
                manager->offsetY = mouseY - piece->currentPos.y;
                break;
            }
        }
    }
}

void HandleMouseMove(MovementManager* manager, int mouseX, int mouseY) {
    if (manager->selectedPiece) {
        manager->selectedPiece->currentPos.x = mouseX - manager->offsetX;
        manager->selectedPiece->currentPos.y = mouseY - manager->offsetY;
    }
}

void HandleMouseUp(MovementManager* manager) {
    manager->selectedPiece = NULL;
}
