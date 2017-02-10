#include <windows.h>

static int m_FrameRate = 60;
static unsigned int m_CurrentFrameTime = 0;
static unsigned int m_FrameTime = 0;
static unsigned int m_OldTime = 0;

void frameRateControllerInitialize(const int frameRate)
{
	timeBeginPeriod(1);

	m_FrameRate = frameRate;
	m_FrameTime = 1000 / m_FrameRate;
}

void frameRateControllerRelease(void)
{
	timeEndPeriod(1);
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

int getCurrentFrameRate(void)
{
	int i = 0;
	if (m_CurrentFrameTime)
        i = 1000/m_CurrentFrameTime;
	return i;
}

unsigned int frameRateControllerStart(void)
{
	unsigned int delta = timeGetTime() - m_OldTime;
    m_OldTime = timeGetTime();

	return delta;
}

void frameRateControllerEnd(void)
{
	int delta = 0;

    m_CurrentFrameTime = timeGetTime() - m_OldTime;
	delta = m_FrameTime - m_CurrentFrameTime;

	/* Keep looping until we reach our target frame rate. */
	if (delta > 0) {
		m_OldTime = timeGetTime();
		while (((int)(timeGetTime() - m_OldTime)) < delta)
			;
        m_CurrentFrameTime = m_FrameTime;
	}
}
