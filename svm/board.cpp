#include "board.h"

namespace svm
{
    Board::Board()
        : memory(),
          pic(),
          pit(pic),
          cpu(memory, pic),
          _working(false) { }

    Board::~Board() { }

    void Board::Start()
    {
        if (!_working) {
            _working = true;

            while (_working) {
                pit.Tick();
                cpu.Step();
            }
        }
    }

    void Board::Stop()
    {
        if (_working) {
            _working = false;
        }
    }
}
