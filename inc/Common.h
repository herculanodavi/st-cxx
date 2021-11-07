#pragma once

enum class Status { kOk, kInvalidArgument, kError };

inline void DoNothing() { __asm volatile("nop"); }