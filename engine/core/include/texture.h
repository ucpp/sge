#pragma once

#include <string>

namespace sge
{
	class Texture
	{
	public:
		Texture();
		void generate(const unsigned char* data, uint32_t width, uint32_t height);
		void bind() const;

		void enableAlpha();
		void setFormat(int channels, size_t pixel_size);
		uint32_t getWidth() const;
		uint32_t getHeight() const;

	public:
		uint32_t id;
		std::string type;

	private:
		uint32_t width;
		uint32_t height;
		uint32_t internal_format;
		uint32_t image_format;
		uint32_t wrap_format;
	};
}