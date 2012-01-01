//
// Urho3D Engine
// Copyright (c) 2008-2012 Lasse ��rni
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Object.h"

/// %Time and frame counter subsystem.
class Time : public Object
{
    OBJECT(Time);
    
public:
    /// Construct.
    Time(Context* context);
    /// Destruct. Reset the low-resolution timer period if set.
    virtual ~Time();
    
    /// Begin new frame, with (last) frame duration in milliseconds and send frame start event.
    void BeginFrame(unsigned mSec);
    /// End frame. Increment total time and send frame end event.
    void EndFrame();
    /// %Set the low-resolution timer period in milliseconds. 0 resets to the default period.
    void SetTimerPeriod(unsigned mSec);
    
    /// Return frame number, starting from 1 once BeginFrame() is called for the first time.
    unsigned GetFrameNumber() const { return frameNumber_; }
    /// Return current frame timestep as seconds.
    float GetTimeStep() const { return timeStep_; }
    /// Return current frame timestep as milliseconds.
    unsigned GetTimeStepMSec() const { return timeStepMSec_; }
    /// Return total elapsed time of frames in milliseconds.
    unsigned GetTotalMSec() const { return totalMSec_; }
    /// Return current low-resolution timer period in milliseconds.
    unsigned GetTimerPeriod() const { return timerPeriod_; }
    
    /// Sleep for a number of milliseconds.
    static void Sleep(unsigned mSec);
    
private:
    /// Frame number.
    unsigned frameNumber_;
    /// Timestep in seconds.
    float timeStep_;
    /// Timestep in milliseconds.
    unsigned timeStepMSec_;
    /// Total elapsed time in milliseconds.
    unsigned totalMSec_;
    /// Low-resolution timer period.
    unsigned timerPeriod_;
};


/// Low-resolution operating system timer.
class Timer
{
public:
    /// Construct. Get the starting clock value.
    Timer();
    
    /// Return elapsed milliseconds and optionally reset.
    unsigned GetMSec(bool reset);
    /// Reset the timer.
    void Reset();
    
private:
    /// Starting clock value in milliseconds.
    unsigned startTime_;
};

/// High-resolution operating system timer used in profiling.
class HiresTimer
{
    friend class Time;
    
public:
    /// Construct. Get the starting high-resolution clock value.
    HiresTimer();
    
    /// Return elapsed microseconds and optionally reset.
    long long GetUSec(bool reset);
    /// Reset the timer.
    void Reset();
    
    /// Return if high-resolution timer is supported.
    static bool IsSupported() { return supported; }
    /// Return high-resolution timer frequency if supported.
    static long long GetFrequency() { return frequency; }
    
private:
    /// Starting clock value in CPU ticks.
    long long startTime_;
    
     /// High-resolution timer support flag.
    static bool supported;
    /// High-resolution timer frequency.
    static long long frequency;
};
