#pragma once

#include "Vector.hpp"

#ifdef AC_AUDIO_OPENAL
#include "al.h"

//typedef ALuint sound_t;
typedef ALuint buffer_t;
#endif

namespace Audio {
	void Init();
	void Deinit() noexcept;

	void UpdateListener(Vector3<float> direction, Vector3<float> up, Vector3<float> pos, Vector3<float> speed) noexcept;

	void LoadOGG(void *mapping, int mlen, void **bufferPtr, int *channels, int *freq, size_t *blen) noexcept;

	void Play(Vector3<float> pos, buffer_t buffer, bool relative = false, bool loop = false) noexcept;

	void GCSounds() noexcept;
}
