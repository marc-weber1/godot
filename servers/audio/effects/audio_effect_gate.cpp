/**************************************************************************/
/*  audio_effect_gate.cpp                                                 */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "audio_effect_gate.h"

#include "core/math/math_funcs.h"

void AudioEffectGateInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {
	float signal_total_l_sqr = 0.f;
	float signal_total_r_sqr = 0.f;
	for (int i = 0; i < p_frame_count; i++) {
		signal_total_l_sqr += p_src_frames[i].l * p_src_frames[i].l;
		signal_total_r_sqr += p_src_frames[i].r * p_src_frames[i].r;
	}

	float RMS_L = Math::sqrt(signal_total_l_sqr / p_frame_count);
	float RMS_R = Math::sqrt(signal_total_r_sqr / p_frame_count);

	float RMS = RMS_L > RMS_R ? RMS_L : RMS_R; // Use the larger one
	float DB_RMS = Math::linear_to_db(RMS);

	bool now_below_threshold = DB_RMS < base->threshold_db;
	if (now_below_threshold && !below_threshold) {
		// Start ducking envelope

		for (int i = 0; i < p_frame_count; i++) {
			p_dst_frames[i] = p_src_frames[i] * (1.f - (1.f * i / p_frame_count));
		}
	} else if (!now_below_threshold && below_threshold) {
		// End ducking envelope

		for (int i = 0; i < p_frame_count; i++) {
			p_dst_frames[i] = p_src_frames[i] * (1.f * i / p_frame_count);
		}
	} else if (now_below_threshold) {
		// Still ducking, output silence

		for (int i = 0; i < p_frame_count; i++) {
			p_dst_frames[i] = AudioFrame(0.f, 0.f);
		}
	} else if (!now_below_threshold) {
		// Still not ducking, forward the signal

		for (int i = 0; i < p_frame_count; i++) {
			p_dst_frames[i] = p_src_frames[i];
		}
	}

	below_threshold = now_below_threshold;
}

Ref<AudioEffectInstance> AudioEffectGate::instantiate() {
	Ref<AudioEffectGateInstance> ins;
	ins.instantiate();
	ins->base = Ref<AudioEffectGate>(this);

	return ins;
}

void AudioEffectGate::set_threshold_db(float p_threshold_db) {
	threshold_db = p_threshold_db;
}

float AudioEffectGate::get_threshold_db() const {
	return threshold_db;
}

void AudioEffectGate::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_threshold_db", "threshold"), &AudioEffectGate::set_threshold_db);
	ClassDB::bind_method(D_METHOD("get_threshold_db"), &AudioEffectGate::get_threshold_db);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshold_db", PROPERTY_HINT_RANGE, "-100,0,0.01,suffix:dB"), "set_threshold_db", "get_threshold_db");
}