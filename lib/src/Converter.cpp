/*
 * Copyright 2017 Aaron Barany
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Converter.h"
#include "S3tcConverter.h"
#include "StandardConverter.h"
#include <cuttlefish/Texture.h>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <thread>
#include <utility>

namespace cuttlefish
{

static std::unique_ptr<Converter> createConverter(const Texture& texture, const Image& image,
	Texture::Quality quality)
{
	(void)quality;
	switch (texture.format())
	{
		case Texture::Format::R4G4:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new R4G4Converter(image));
			return nullptr;
		}
		case Texture::Format::R4G4B4A4:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new R4G4B4A4Converter(image));
			return nullptr;
		}
		case Texture::Format::B4G4R4A4:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new B4G4R4A4Converter(image));
			return nullptr;
		}
		case Texture::Format::R5G6B5:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new R5G6B5Converter(image));
			return nullptr;
		}
		case Texture::Format::B5G6R5:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new B5G6R5Converter(image));
			return nullptr;
		}
		case Texture::Format::R5G5B5A1:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new R5G5B5A1Converter(image));
			return nullptr;
		}
		case Texture::Format::B5G5R5A1:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new B5G5R5A1Converter(image));
			return nullptr;
		}
		case Texture::Format::A1R5G5B5:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new A1R5G5B5Converter(image));
			return nullptr;
		}
		case Texture::Format::R8:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint8_t, 1>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int8_t, 1>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint8_t, 1>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int8_t, 1>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R8G8:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint8_t, 2>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int8_t, 2>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint8_t, 2>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int8_t, 2>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R8G8B8:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint8_t, 3>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int8_t, 3>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint8_t, 3>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int8_t, 3>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::B8G8R8:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new B8G8R8Converter(image));
			return nullptr;
		}
		case Texture::Format::R8G8B8A8:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint8_t, 4>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int8_t, 4>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint8_t, 4>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int8_t, 4>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::B8G8R8A8:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new B8G8R8A8Converter(image));
			return nullptr;
		}
		case Texture::Format::A8B8G8R8:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new B8G8R8A8Converter(image));
			return nullptr;
		}
		case Texture::Format::A2R10G10B10:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new A2R10G10B10UNormConverter(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new A2R10G10B10UIntConverter(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::A2B10G10R10:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new A2B10G10R10UNormConverter(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new A2B10G10R10UIntConverter(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R16:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint16_t, 1>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int16_t, 1>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint16_t, 1>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int16_t, 1>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<std::uint16_t, 1>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R16G16:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint16_t, 2>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int16_t, 2>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint16_t, 2>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int16_t, 2>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<std::uint16_t, 2>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R16G16B16:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint16_t, 3>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int16_t, 3>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint16_t, 3>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int16_t, 3>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<std::uint16_t, 3>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R16G16B16A16:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint16_t, 4>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int16_t, 4>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint16_t, 4>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int16_t, 4>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<std::uint16_t, 4>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R32:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint32_t, 1>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int32_t, 1>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint32_t, 1>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int32_t, 1>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<float, 1>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R32G32:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint32_t, 2>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int32_t, 2>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint32_t, 2>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int32_t, 2>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<float, 2>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R32G32B32:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint32_t, 3>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int32_t, 3>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint32_t, 3>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int32_t, 3>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<float, 3>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::R32G32B32A32:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new UNormConverter<std::uint32_t, 4>(image));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new SNormConverter<std::int32_t, 4>(image));
				case Texture::Type::UInt:
					return std::unique_ptr<Converter>(new IntConverter<std::uint32_t, 4>(image));
				case Texture::Type::Int:
					return std::unique_ptr<Converter>(new IntConverter<std::int32_t, 4>(image));
				case Texture::Type::Float:
					return std::unique_ptr<Converter>(new FloatConverter<float, 4>(image));
				default:
					return nullptr;
			}
		}
		case Texture::Format::B10G11R11_UFloat:
		{
			if (texture.type() == Texture::Type::UFloat)
				return std::unique_ptr<Converter>(new B10R11R11UFloatConverter(image));
			return nullptr;
		}
		case Texture::Format::E5B9G9R9_UFloat:
		{
			if (texture.type() == Texture::Type::UFloat)
				return std::unique_ptr<Converter>(new E5B9G9R9UFloatConverter(image));
			return nullptr;
		}
#if CUTTLEFISH_HAS_S3TC
		case Texture::Format::BC1_RGB:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new Bc1Converter(texture, image, quality));
			return nullptr;
		}
		case Texture::Format::BC1_RGBA:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new Bc1AConverter(texture, image, quality));
			return nullptr;
		}
		case Texture::Format::BC2:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new Bc2Converter(texture, image, quality));
			return nullptr;
		}
		case Texture::Format::BC3:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new Bc3Converter(texture, image, quality));
			return nullptr;
		}
		case Texture::Format::BC4:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new Bc4Converter(texture, image, quality,
						false));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new Bc4Converter(texture, image, quality,
						true));
				default:
					return nullptr;
			}
		}
		case Texture::Format::BC5:
		{
			switch (texture.type())
			{
				case Texture::Type::UNorm:
					return std::unique_ptr<Converter>(new Bc5Converter(texture, image, quality,
						false));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new Bc5Converter(texture, image, quality,
						true));
				default:
					return nullptr;
			}
		}
		case Texture::Format::BC6H:
		{
			switch (texture.type())
			{
				case Texture::Type::UFloat:
					return std::unique_ptr<Converter>(new Bc6HConverter(texture, image, quality,
						false));
				case Texture::Type::SNorm:
					return std::unique_ptr<Converter>(new Bc6HConverter(texture, image, quality,
						true));
				default:
					return nullptr;
			}
		}
		case Texture::Format::BC7:
		{
			if (texture.type() == Texture::Type::UNorm)
				return std::unique_ptr<Converter>(new Bc7Converter(texture, image, quality));
			return nullptr;
		}
#endif // CUTTLEFISH_HAS_S3TC
		default:
			return nullptr;
	}
}

bool Converter::convert(const Texture& texture, MipImageList& images, MipTextureList& textureData,
	Texture::Quality quality, unsigned int threadCount)
{
	std::atomic<unsigned int> curJob(0);
	std::vector<std::pair<unsigned int, unsigned int>> jobs;
	std::vector<std::thread> threads;
	if (threadCount > 1)
		threads.reserve(threadCount);

	textureData.resize(textureData.size());
	for (unsigned int mip = 0; mip < images.size(); ++mip)
	{
		textureData[mip].resize(images[mip].size());
		for (unsigned int d = 0; d < images[mip].size(); ++d)
		{
			textureData[mip][d].resize(images[mip][d].size());
			for (unsigned int f = 0; f < images[mip][d].size(); ++f)
			{
				auto converter = createConverter(texture, images[mip][d][f], quality);
				if (!converter)
				{
					// If the converter can't be created, should only do so for the first one.
					assert(mip == 0 && d == 0 && f == 0);
					textureData.clear();
					return false;
				}

				unsigned int jobsX = converter->jobsX();
				unsigned int jobsY = converter->jobsY();
				jobs.resize(jobsX*jobsY);
				assert(!jobs.empty());
				for (unsigned int y = 0; y < jobsY; ++y)
				{
					for (unsigned int x = 0; x < jobsX; ++x)
						jobs[y*jobsX + x] = std::make_pair(x, y);
				}

				unsigned int curThreads = std::min(jobsX*jobsY, threadCount);
				if (curThreads <= 1)
				{
					for (const std::pair<unsigned int, unsigned int>& job : jobs)
						converter->process(job.first, job.second);
				}
				else
				{
					for (unsigned int i = 0; i < curThreads; ++i)
					{
						threads.emplace_back([&curJob, &jobs, &converter]()
							{
								do
								{
									unsigned int thisJob = curJob++;
									if (thisJob >= jobs.size())
										return;

									converter->process(jobs[thisJob].first, jobs[thisJob].second);
								} while (true);
							});
					}

					for (std::thread& thread : threads)
						thread.join();
					threads.clear();
				}

				images[mip][d][f].reset();
				textureData[mip][d][f] = std::move(converter->data());
			}
		}
	}

	return true;
}

} // namespace cuttlefish
