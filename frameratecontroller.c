#include "platform.h"

static int m_FrameRate = 60;
static unsigned int m_CurrentFrameTime = 0;
static unsigned int m_FrameTime = 0;
static unsigned int m_OldTime = 0;
static int m_lock = 0;

void frameRateControllerInitialize(const int frameRate, const int lock)
{
  startWatchPrecision(1);

  m_FrameRate = frameRate;
  m_FrameTime = 1000 / m_FrameRate;
  m_lock = lock;
}

void frameRateControllerRelease(void)
{
  endWatchPrecision(1);
}

void setFrameRate(const int frameRate)
{
  m_FrameRate = frameRate;
  m_FrameTime = 1000 / m_FrameRate;
}

int getSpecifiedFrameRate(void)
{
  return m_FrameRate;
}

int getActualFrameRate(void)
{
  int i = 0;
  if (m_CurrentFrameTime)
    i = 1000 / m_CurrentFrameTime;
  return i;
}

unsigned int frameRateControllerStart(void)
{
  unsigned int delta = getSystemTime() - m_OldTime;
  m_OldTime = getSystemTime();

  return delta;
}

void frameRateControllerEnd(void)
{
  int delta = 0;

  m_CurrentFrameTime = getSystemTime() - m_OldTime;
  delta = m_FrameTime - m_CurrentFrameTime;

  // If framerate is locked, loop til we reach our target.
  // TODO: Sleep instead of looping.
  if (m_lock && delta > 0) {
    m_OldTime = getSystemTime();
    while (((int)(getSystemTime() - m_OldTime)) < delta)
      ;
    m_CurrentFrameTime = m_FrameTime;
  }
}

