#pragma once

class PlayerLoopController
{
public:
    bool IsPaused() const { return m_IsPaused; }
private:
    bool m_IsPaused;
};