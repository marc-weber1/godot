

#ifndef AUDIO_EFFECT_GATE_H
#define AUDIO_EFFECT_GATE_H

#include "servers/audio/audio_effect.h"

class AudioEffectGate;

class AudioEffectGateInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectGateInstance, AudioEffectInstance);
	friend class AudioEffectGate;

	Ref<AudioEffectGate> base;

    bool below_threshhold = false;

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) override;
};

class AudioEffectGate : public AudioEffect {
	GDCLASS(AudioEffectGate, AudioEffect);
    friend class AudioEffectGateInstance;

protected:
	static void _bind_methods();

    float threshhold_db = -30.f;

public:
    Ref<AudioEffectInstance> instantiate() override;

    void set_threshhold_db(float p_threshhold_db);
	float get_threshhold_db() const;
};

#endif // AUDIO_EFFECT_GATE_H