//
// Created by berke on 4/2/2026.
//

#ifndef CONWAY_CONFIG_H
#define CONWAY_CONFIG_H

#define CELL_SIZE 45

#define ROW 16
#define COL 16

#define SCREEN_WIDTH ROW * CELL_SIZE
#define SCREEN_HEIGHT ROW * CELL_SIZE

#define UI_HEIGHT 2

#define TOTAL_HEIGHT (SCREEN_WIDTH + UI_HEIGHT * CELL_SIZE)

#endif //CONWAY_CONFIG_H