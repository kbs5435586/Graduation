#include "Value.hlsl"
#include "Function.hlsl"
RWStructuredBuffer<tParticle> tRWData : register(u0);
RWStructuredBuffer<tParticleShared> tRWSharedData : register(u1);

