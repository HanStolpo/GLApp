#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <Graphics/Texture.h>
#include <Helpers/ResourceCache.h>

namespace fs = boost::filesystem;

namespace baselib 
{
	namespace graphics
	{
		/*! @brief TextureLoader creates and caches textures.
		 *
		 */
		class TextureLoader
		{
		public:
			//! Constructor.
			TextureLoader();
			//! Destructor.
			virtual ~TextureLoader();

			//! Loads a texture object from file.
			boost::shared_ptr<Texture> loadTexture(const fs::path& fsPath);

		private:
			ResourceCache<Texture> m_TextureCache; //!< Texture object cache.

		};
	}
}