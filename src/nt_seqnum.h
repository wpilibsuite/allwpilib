/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_SEQNUM_H_
#define NT_SEQNUM_H_

namespace NtImpl {

class SequenceNumber
{
public:
    explicit SequenceNumber(unsigned int val) : m_val(val) {}
    unsigned int GetVal() const { return m_val; }

    SequenceNumber& operator++()
    {
        ++m_val;
        if (m_val > 0xffff)
            m_val = 0;
        return *this;
    }
    SequenceNumber operator++(int)
    {
        SequenceNumber tmp(*this);
        operator++();
        return tmp;
    }

    friend bool operator< (const SequenceNumber &lhs,
                           const SequenceNumber &rhs);
    friend bool operator> (const SequenceNumber &lhs,
                           const SequenceNumber &rhs);
    friend bool operator<= (const SequenceNumber &lhs,
                            const SequenceNumber &rhs);
    friend bool operator>= (const SequenceNumber &lhs,
                            const SequenceNumber &rhs);
    friend bool operator== (const SequenceNumber &lhs,
                            const SequenceNumber &rhs);
    friend bool operator!= (const SequenceNumber &lhs,
                            const SequenceNumber &rhs);
private:
    unsigned int m_val;
};

bool operator< (const SequenceNumber &lhs, const SequenceNumber &rhs);
bool operator> (const SequenceNumber &lhs, const SequenceNumber &rhs);

inline bool operator<= (const SequenceNumber &lhs, const SequenceNumber &rhs)
{
    return lhs == rhs || lhs < rhs;
}

inline bool operator>= (const SequenceNumber &lhs, const SequenceNumber &rhs)
{
    return lhs == rhs || lhs > rhs;
}

inline bool operator== (const SequenceNumber &lhs, const SequenceNumber &rhs)
{
    return lhs.m_val == rhs.m_val;
}

inline bool operator!= (const SequenceNumber &lhs, const SequenceNumber &rhs)
{
    return lhs.m_val != rhs.m_val;
}

} // namespace NtImpl

#endif /* NT_SEQNUM_H_ */
