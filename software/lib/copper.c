#include "copper.h"

#include <stddef.h>

#include "assert.h"

// volatile is strictly correct here considering a coprocessor is reading this
// in practice it might not matter by the time the function returns
static volatile uint16_t * const COP_RAM = (uint16_t *)0x50000;
static const size_t COP_RAM_SIZE = 0x1000 / sizeof(uint16_t);

static uint16_t current_address = 0;

// 2bit ops

static const uint8_t OP_SHIFT = 13;

typedef enum {
    SET_TARGET = 0,
    WAIT_TARGET = 1,
    WRITE_REG = 2,
    JUMP = 3
} Op;

static void cop_target(uint16_t target, bool is_y, bool wait);

void cop_ram_seek(uint16_t address) {
    assert(address < COP_RAM_SIZE);

    current_address = address;
}

void cop_set_target_x(uint16_t target_x) {
    cop_target(target_x, false, false);
}

void cop_wait_target_x(uint16_t target_x) {
    cop_target(target_x, false, true);
}

static void cop_target(uint16_t target, bool is_y, bool wait) {
    uint16_t op_word = (wait ? WAIT_TARGET : SET_TARGET) << OP_SHIFT;
    op_word |= is_y ? 1 << 11 : 0;
    op_word |= wait ? 1 << 12 : 0;
    op_word |= target;
    COP_RAM[current_address++] = op_word;
}

void cop_wait_target_y(uint16_t target_y) {
    cop_target(target_y, true, true);
}

void cop_write(uint8_t reg, uint16_t data) {
    uint16_t op_word = WRITE_REG << OP_SHIFT;
    op_word |= reg;
    COP_RAM[current_address++] = op_word;
    COP_RAM[current_address++] = data;
}

void cop_jump(uint16_t address) {
    assert(address < COP_RAM_SIZE);

    uint16_t op_word = JUMP << OP_SHIFT;
    op_word |= address;
    COP_RAM[current_address++] = op_word;
}
