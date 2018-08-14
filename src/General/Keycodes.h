#ifndef KEYCODES_H
#define KEYCODES_H

namespace Keycodes
{
    namespace GPKEY
    {
        enum VALUES
        {
            NONE = 0,
            A = 1,
            B = 2,
            X = 3,
            Y = 4,
            DOWN = 5,
            LEFT = 6,
            RIGHT = 7,
            UP = 8,
            SELECT = 20
        };
    }

    namespace KBKEY
    {
        enum VALUES
        {
            NONE = 0xFF,
            B = 0x42,
            C = 0x43,
            W = 0x57,
            S = 0x53,
            A = 0x41,
            D = 0x44,
            J = 0x4A,
            K = 0x49,
            L = 0x4C,
            Z = 0x5A,
            X = 0x58,
            V = 0x56,
            N = 0x4E,
            COMMA = 0xBC,
            DOT = 0xBE
        };
    }
}

#endif // !KEYCODES_H
