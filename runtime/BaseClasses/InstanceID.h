#pragma once

typedef SInt32 InstanceID;
enum { InstanceID_None = 0 };

inline InstanceID GetInstanceIDFrom(InstanceID id = InstanceID_None) { return id; }