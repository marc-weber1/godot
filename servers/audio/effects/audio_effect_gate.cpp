

#include "audio_effect_gate.h"

#include "core/math/math_funcs.h"
#include "servers/audio_server.h"

void AudioEffectGateInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {
    
    float mix_rate = AudioServer::get_singleton()->get_mix_rate();

    float signal_total_l_sqr = 0.f;
    float signal_total_r_sqr  = 0.f;
    for(int i=0; i<p_frame_count; i++){
        signal_total_l_sqr += p_src_frames[i].l * p_src_frames[i].l;
        signal_total_r_sqr += p_src_frames[i].r * p_src_frames[i].r;
    }

    float DB_RMS_L = Math::sqrt(signal_total_l_sqr / p_frame_count);
    float DB_RMS_R = Math::sqrt(signal_total_r_sqr / p_frame_count);

    float DB_RMS = DB_RMS_L > DB_RMS_R ? DB_RMS_L : DB_RMS_R; // Use the larger one

    bool now_below_threshhold = DB_RMS < base->threshhold_db;
    if(now_below_threshhold && !below_threshhold){
        // Start ducking envelope

        for(int i=0; i<p_frame_count; i++){
            p_dst_frames[i] = p_src_frames[i] * (1.f - ( 1.f * i / p_frame_count));
        }
    }
    else if(!now_below_threshhold && below_threshhold){
        // End ducking envelope

        for(int i=0; i<p_frame_count; i++){
            p_dst_frames[i] = p_src_frames[i] * ( 1.f * i / p_frame_count);
        }
    }
    else if(now_below_threshhold){
        // Still ducking, output silence

        for(int i=0; i<p_frame_count; i++){
            p_dst_frames[i] = AudioFrame(0.f, 0.f);
        }
    }
    else if(!now_below_threshhold){
        // Still not ducking, forward the signal

        for(int i=0; i<p_frame_count; i++){
            p_dst_frames[i] = p_src_frames[i];
        }
    }

    below_threshhold = now_below_threshhold;
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