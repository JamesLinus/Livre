/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Livre <https://github.com/BlueBrain/Livre>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <livre/lib/cache/TextureCache.h>
#include <livre/lib/cache/TextureDataCache.h>
#include <livre/lib/cache/TextureDataObject.h>
#include <livre/lib/cache/TextureObject.h>

#include <livre/core/data/LODNode.h>
#include <livre/core/data/VolumeDataSource.h>
#include <livre/core/render/Renderer.h>

#include <eq/gl.h>

namespace livre
{
struct TextureCache::Impl
{
    Impl( TextureDataCache& dataCache,
          const int internalTextureFormat )
        : _dataCache( dataCache )
    {
        const VolumeDataSource& dataSource = _dataCache.getDataSource();
        const VolumeInformation& info = dataSource.getVolumeInformation();

        uint32_t format;
        switch( info.compCount )
        {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            default:
                format = GL_RED;
                break;
        }

        _texturePool.reset( new TexturePool( info.maximumBlockSize,
                                             internalTextureFormat,
                                             format,
                                             dataCache.getTextureType( )));
    }

    TextureDataCache& _dataCache;
    std::unique_ptr< TexturePool > _texturePool;
};

TextureCache::TextureCache( TextureDataCache& dataCache,
                            const size_t maxMemBytes,
                            const GLint internalTextureFormat )
    : LRUCache( maxMemBytes )
    , _impl( new Impl( dataCache, internalTextureFormat ))
{
    _statistics->setName( "Texture cache GPU");
}

CacheObject* TextureCache::_generate( const CacheId& cacheId  )
{
    return new TextureObject( cacheId, *this );
}

TextureCache::~TextureCache()
{}

TexturePool& TextureCache::getTexturePool() const
{
    return *_impl->_texturePool.get();
}

TextureDataCache& TextureCache::getDataCache()
{
    return _impl->_dataCache;
}

const TextureDataCache& TextureCache::getDataCache() const
{
    return _impl->_dataCache;
}

}
