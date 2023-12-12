

#include "audio_effect_gate.h"

void AudioEffectGateInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {
    
    // Just write silence for now
    for(int i=0; i<p_frame_count; i++){
        p_dst_frames[i] = AudioFrame(0.f, 0.f);
    }
}

Ref<AudioEffectInstance> AudioEffectGate::instantiate() {
	Ref<AudioEffectGateInstance> ins;
	ins.instantiate();
	ins->base = Ref<AudioEffectGate>(this);

    return ins;
}

void AudioEffectGate::set_threshhold_db(float p_threshhold_db) {
	threshhold_db = p_threshhold_db;
}

float AudioEffectGate::get_threshhold_db() const {
	return threshhold_db;
}

void AudioEffectGate::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_threshhold_db", "threshhold"), &AudioEffectGate::set_threshhold_db);
	ClassDB::bind_method(D_METHOD("get_threshhold_db"), &AudioEffectGate::get_threshhold_db);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshhold_db", PROPERTY_HINT_RANGE, "-60,0,0.01,suffix:dB"), "set_threshhold_db", "get_threshhold_db");
}