#include "GameTimer.h"

GameTimer::GameTimer() {
	mDeltaTimeDuration = ZERO_DURATION;
	gameTimePoint startTime = gameClock::now();
	mBaseTime = startTime;
	mPausedTime = ZERO_DURATION;
	mStopTime = startTime;
	mPrevTime = startTime;
	mCurrTime = startTime;
	mStopped = false;
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
gameClockDuration GameTimer::TotalTimeDuration() const {
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime
	if (mStopped) {
		return mStopTime - mBaseTime - mPausedTime;
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime
	else {
		return mCurrTime - mBaseTime - mPausedTime;
	}
}

float GameTimer::TotalTime() const {
	return std::chrono::duration<float>(TotalTimeDuration()).count();
}

gameClockDuration GameTimer::DeltaTimeDuration() const {
	return mDeltaTimeDuration;
}

float GameTimer::DeltaTime() const {
	return std::chrono::duration<float>(mDeltaTimeDuration).count();
}

void GameTimer::Reset() {
	gameTimePoint startTime = gameClock::now();
	mBaseTime = startTime;
	mPrevTime = startTime;
	mStopTime = startTime;
	mStopped = false;
}

void GameTimer::Start() {
	gameTimePoint startTime = gameClock::now();

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     
	if (mStopped) {
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = startTime;
		mStopped = false;
	}
	else {
		Reset();
	}
}

void GameTimer::Stop() {
	if (!mStopped) {
		mStopTime = gameClock::now();
		mStopped = true;
	}
}

void GameTimer::Tick() {
	if (mStopped) {
		mDeltaTimeDuration = ZERO_DURATION;
		return;
	}

	mCurrTime = gameClock::now();

	// Time difference between this frame and the previous.
	mDeltaTimeDuration = mCurrTime - mPrevTime;

	// Prepare for next frame.
	mPrevTime = mCurrTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (mDeltaTimeDuration < ZERO_DURATION) {
		mDeltaTimeDuration = ZERO_DURATION;
	}
}