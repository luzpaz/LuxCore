#include <string>
namespace luxrays { namespace ocl {
std::string KernelSource_motionsystem_funcs = 
"#line 2 \"motionsystem_funcs.cl\"\n"
"\n"
"/***************************************************************************\n"
" * Copyright 1998-2013 by authors (see AUTHORS.txt)                        *\n"
" *                                                                         *\n"
" *   This file is part of LuxRender.                                       *\n"
" *                                                                         *\n"
" * Licensed under the Apache License, Version 2.0 (the \"License\");         *\n"
" * you may not use this file except in compliance with the License.        *\n"
" * You may obtain a copy of the License at                                 *\n"
" *                                                                         *\n"
" *     http://www.apache.org/licenses/LICENSE-2.0                          *\n"
" *                                                                         *\n"
" * Unless required by applicable law or agreed to in writing, software     *\n"
" * distributed under the License is distributed on an \"AS IS\" BASIS,       *\n"
" * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*\n"
" * See the License for the specific language governing permissions and     *\n"
" * limitations under the License.                                          *\n"
" ***************************************************************************/\n"
"\n"
"void InterpolatedTransform_Sample(__global InterpolatedTransform *interpolatedTransform,\n"
"		const float time, Matrix4x4 *result) {\n"
"	if (!interpolatedTransform->isActive) {\n"
"		*result = interpolatedTransform->start.mInv;\n"
"		return;\n"
"	}\n"
"\n"
"	// Determine interpolation value\n"
"	if (time <= interpolatedTransform->startTime) {\n"
"		*result = interpolatedTransform->start.mInv;\n"
"		return;\n"
"	}\n"
"	if (time >= interpolatedTransform->endTime) {\n"
"		*result = interpolatedTransform->end.mInv;\n"
"		return;\n"
"	}\n"
"\n"
"	const float w = interpolatedTransform->endTime - interpolatedTransform->startTime;\n"
"	const float d = time - interpolatedTransform->startTime;\n"
"	const float le = d / w;\n"
"\n"
"	// if translation only, just modify start matrix\n"
"	if (interpolatedTransform->hasTranslation &&\n"
"			!(interpolatedTransform->hasScale || interpolatedTransform->hasRotation)) {\n"
"		*result = interpolatedTransform->start.m;\n"
"		if (interpolatedTransform->hasTranslationX)\n"
"			result->m[0][3] = mix(interpolatedTransform->startT.Tx, interpolatedTransform->endT.Tx, le);\n"
"		if (interpolatedTransform->hasTranslationY)\n"
"			result->m[1][3] = mix(interpolatedTransform->startT.Ty, interpolatedTransform->endT.Ty, le);\n"
"		if (interpolatedTransform->hasTranslationZ)\n"
"			result->m[2][3] = mix(interpolatedTransform->startT.Tz, interpolatedTransform->endT.Tz, le);\n"
"\n"
"		return;\n"
"	}\n"
"\n"
"	if (interpolatedTransform->hasRotation) {\n"
"		// Quaternion interpolation of rotation\n"
"		const float4 startQ = VLOAD4F(&interpolatedTransform->startQ.w);\n"
"		const float4 endQ = VLOAD4F(&interpolatedTransform->endQ.w);\n"
"		const float4 interQ = Quaternion_Slerp(le, startQ, endQ);\n"
"		Quaternion_ToMatrix(interQ, result);\n"
"	} else\n"
"		*result = interpolatedTransform->startT.R;\n"
"\n"
"	if (interpolatedTransform->hasScale) {\n"
"		const float Sx = mix(interpolatedTransform->startT.Sx, interpolatedTransform->endT.Sx, le);\n"
"		const float Sy = mix(interpolatedTransform->startT.Sy, interpolatedTransform->endT.Sy, le); \n"
"		const float Sz = mix(interpolatedTransform->startT.Sz, interpolatedTransform->endT.Sz, le);\n"
"\n"
"		// T * S * R\n"
"		for (uint j = 0; j < 3; ++j) {\n"
"			result->m[0][j] = Sx * result->m[0][j];\n"
"			result->m[1][j] = Sy * result->m[1][j];\n"
"			result->m[2][j] = Sz * result->m[2][j];\n"
"		}\n"
"	} else {\n"
"		for (uint j = 0; j < 3; ++j) {\n"
"			result->m[0][j] = interpolatedTransform->startT.Sx * result->m[0][j];\n"
"			result->m[1][j] = interpolatedTransform->startT.Sy * result->m[1][j];\n"
"			result->m[2][j] = interpolatedTransform->startT.Sz * result->m[2][j];\n"
"		}\n"
"	}\n"
"\n"
"	if (interpolatedTransform->hasTranslationX)\n"
"		result->m[0][3] = mix(interpolatedTransform->startT.Tx, interpolatedTransform->endT.Tx, le);\n"
"	else\n"
"		result->m[0][3] = interpolatedTransform->startT.Tx;\n"
"\n"
"	if (interpolatedTransform->hasTranslationY)\n"
"		result->m[1][3] = mix(interpolatedTransform->startT.Ty, interpolatedTransform->endT.Ty, le);\n"
"	else\n"
"		result->m[1][3] = interpolatedTransform->startT.Ty;\n"
"\n"
"	if (interpolatedTransform->hasTranslationZ)\n"
"		result->m[2][3] = mix(interpolatedTransform->startT.Tz, interpolatedTransform->endT.Tz, le);\n"
"	else\n"
"		result->m[2][3] = interpolatedTransform->startT.Tz;\n"
"}\n"
"\n"
"void MotionSystem_Sample(__global MotionSystem *motionSystem, const float time,\n"
"		__global InterpolatedTransform *interpolatedTransforms, Matrix4x4 *result) {\n"
"	const uint interpolatedTransformFirstIndex = motionSystem->interpolatedTransformFirstIndex;\n"
"	const uint interpolatedTransformLastIndex = motionSystem->interpolatedTransformLastIndex;\n"
"\n"
"	// Pick the right InterpolatedTransform\n"
"	uint index = interpolatedTransformLastIndex;\n"
"	for (uint i = interpolatedTransformFirstIndex; i <= interpolatedTransformLastIndex; ++i) {\n"
"		if (time < interpolatedTransforms[i].endTime) {\n"
"			index = i;\n"
"			break;\n"
"		}\n"
"	}\n"
"\n"
"	InterpolatedTransform_Sample(&interpolatedTransforms[index], time, result);\n"
"}\n"
; } }