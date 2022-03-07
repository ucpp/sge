#pragma once

#include <cstdint>

namespace sge
{
	class ShadowMapFBO
	{
	public:
		ShadowMapFBO();
		~ShadowMapFBO();
		bool initialize(uint32_t width, uint32_t height);
		void bindForReading(uint32_t texture_unit);
		void bindForWriting();
		void destroy();
		inline uint32_t getTexId() const { return id; };

	private:
		uint32_t FBO;
		uint32_t id;
		uint32_t width;
		uint32_t height;
	};
}