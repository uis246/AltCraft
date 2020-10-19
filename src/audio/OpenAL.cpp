#include "../Audio.hpp"
#include "../AssetManager.hpp"

#include <string>
#include <easylogging++.h>

#include "alc.h"

ALCdevice *device;
ALCcontext *context;

std::vector<ALuint> sources;

ALenum alCheckError_(const char *file, int line) noexcept;
#define alCheckError() alCheckError_(__FILE__, __LINE__)

ALCenum alcCheckError_(const char *file, int line) noexcept;
#define alcCheckError(dev) alcCheckError_(dev, __FILE__, __LINE__)

ALenum alCheckError_(const char *file, int line) noexcept {
	ALenum errorCode = alGetError();
	if (errorCode != AL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
			case AL_INVALID_NAME:
				error = "INVALID_NAME";
				break;
			case AL_INVALID_ENUM:
				error = "INVALID_ENUM";
				break;
			case AL_INVALID_VALUE:
				error = "INVALID_VALUE";
				break;
			case AL_INVALID_OPERATION:
				error = "INVALID_OPERATION";
				break;
			case AL_OUT_OF_MEMORY:
				error = "OUT_OF_MEMORY";
				break;
		}
		LOG(ERROR) << "OpenAL error: " << error << " at " << file << ":" << line;
	}
	return errorCode;
}

ALCenum alcCheckError_(ALCdevice *dev, const char *file, int line) noexcept {
	ALCenum errorCode = alcGetError(dev);
	if (errorCode != ALC_NO_ERROR) {
		std::string error;
		switch (errorCode) {
			case ALC_INVALID_DEVICE:
				error = "INVALID_DEVICE";
				break;
			case ALC_INVALID_CONTEXT:
				error = "INVALID_CONTEXT";
				break;
			case ALC_INVALID_ENUM:
				error = "INVALID_ENUM";
				break;
			case ALC_INVALID_VALUE:
				error = "INVALID_VALUE";
				break;
			case ALC_OUT_OF_MEMORY:
				error = "OUT_OF_MEMORY";
				break;
		}
		LOG(ERROR) << "OpenAL context error: " << error << " at " << file << ":" << line;
		assert(0);
	}
	return errorCode;
}

void Audio::Deinit() noexcept {
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void Audio::Init() {
	device = alcOpenDevice(NULL);
	alcCheckError(device);
	context = alcCreateContext(device, NULL);
	alcCheckError(device);
	if (!alcMakeContextCurrent(context))
		alcCheckError(device);
}

void Audio::UpdateListener(Vector3<float> direction, Vector3<float> up, Vector3<float> pos, Vector3<float> vel) noexcept {
	ALfloat listPos[6] = {direction.x, direction.y, direction.z, up.x, up.y, up.z};

	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
//	alCheckError();
	alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
//	alCheckError();
	alListenerfv(AL_ORIENTATION, listPos);
//	alCheckError();
}

void Audio::PlaySound(Vector3<float> pos, buffer_t buffer, bool relative, bool loop) noexcept {
	ALuint source;
	alGenSources(1, &source);
	if(alCheckError()==AL_OUT_OF_MEMORY)
		GCSounds();
	alSourcei(source, AL_SOURCE_RELATIVE, relative);
	alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
	alSourcei(source, AL_BUFFER, buffer);
	alSourcei(source, AL_LOOPING, loop);
	alSourcePlay(source);
	alCheckError();
}

//For background
//alSourcei(id, AL_DIRECT_CHANNELS_SOFT, 1)

std::vector<ALuint> toDelete;
void Audio::GCSounds() noexcept {
	toDelete.resize(sources.size());
	size_t cnt=0;
	for(ALuint source : sources) {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if(state==AL_STOPPED)
			toDelete[cnt++]=source;
	}
	alDeleteSources(cnt, toDelete.data());
}
