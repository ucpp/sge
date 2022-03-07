#pragma once
#include <stdint.h>

namespace sge
{
	//TODO:
	// + add smfbo
	// - add cascaded smfbo
	// - add dynamic cascade calculating
	// - add technique with simple shadow map
	// - add technique with PCF
	// - add technique with advanced PCF
	// - add technique with VSM
	// - add technique with VSM + gaussian blur
	// - add technique with ESM
	// - add technique with CSM

	class CascadedShadowMapFBO
	{
	public:
		CascadedShadowMapFBO();
		~CascadedShadowMapFBO();
		bool initialize(uint32_t width, uint32_t height);
		void bindForWriting(uint32_t index);
		void bindForReading();
		void destroy();
		inline uint32_t getTexId(uint32_t index) const { return ids[index]; };

	public:
		static const uint32_t count_maps = 3;

	private:
		uint32_t FBO;
		uint32_t ids[count_maps];
		uint32_t width;
		uint32_t height;
	};
}