#include "TextureUpdateHelper.h"

#include <Logging/Log.h>
#include <Helpers/NullPtr.h>
#include <Graphics/Renderer.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/StaticGeometry.h>
#include <Graphics/VertexList.h>
#include <Graphics/FrameBuffer.h>

#include <GL/glew.h>

namespace baselib { namespace graphics {

	boost::shared_ptr<TextureUpdateHelper> TextureUpdateHelper::create(const boost::shared_ptr<Renderer> spRenderer)
	{
		assert(spRenderer);
		return boost::shared_ptr<TextureUpdateHelper>(new TextureUpdateHelper(spRenderer));
	}

	TextureUpdateHelper::TextureUpdateHelper(const boost::shared_ptr<Renderer> spRenderer)
		: m_spRenderer(spRenderer)
	{
		LOG_VERBOSE << "TextureUpdateHelper constructor";
		init();
	}

	TextureUpdateHelper::~TextureUpdateHelper()
	{
		LOG_VERBOSE << "TextureUpdateHelper destructor";
	}

	void TextureUpdateHelper::updateRegion(const boost::shared_ptr<Texture> spInputTexture, const boost::shared_ptr<Texture> spTargetTexture, const boost::shared_ptr<Shader> spShader)
	{
		updateRegion(spInputTexture, Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f), spTargetTexture, spShader);
	}

	void TextureUpdateHelper::updateRegion(const boost::shared_ptr<Texture> spInputTexture, const Vec2& vMinTo, const Vec2& vMaxTo, const boost::shared_ptr<Texture> spTargetTexture, const boost::shared_ptr<Shader> spShader)
	{
		updateRegion(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f), spInputTexture, vMinTo, vMaxTo, spTargetTexture, spShader);
	}

	void TextureUpdateHelper::updateRegion(const Vec2& vMinFrom, const Vec2& vMaxFrom, const boost::shared_ptr<Texture> spInputTexture, const Vec2& vMinTo, const Vec2& vMaxTo, const boost::shared_ptr<Texture> spTargetTexture, const boost::shared_ptr<Shader> spShader)
	{
		m_spFrameBuffer->bind(spTargetTexture); // TODO: View port size needs to be set
		m_spRenderer->setViewportSize(Vec4(0, 0, spTargetTexture->getWidth(), spTargetTexture->getHeight()));
		spShader->bind();
		spShader->setUniform(spShader->getUniform("vUVOffset"), vMinFrom);
		spShader->setUniform(spShader->getUniform("vUVScale"), vMaxFrom - vMinFrom);
		spShader->setUniform(spShader->getUniform("vPosOffset"), vMinTo);
		spShader->setUniform(spShader->getUniform("vPosScale"), vMaxTo - vMinTo);
		spShader->setUniform(spShader->getUniform("sTexture"), 0); // TODO: Get active unit from texture - just using 0 for everything at the moment.
		spInputTexture->bind();
		m_spQuadGeometry->bind();
		m_spRenderer->drawIndexed(m_spQuadGeometry->getPrimitiveType(), m_spQuadGeometry->getVertexList()->getNumIndices(), 0);
	}

	namespace
	{
		struct VertexPosUV
		{
			VertexPosUV(const Vec3& _vPos, const Vec2& _vUV)
				: vPos(_vPos)
				, vUV(_vUV) {}

			Vec3 vPos;
			Vec2 vUV;
		};
	}

	void TextureUpdateHelper::init()
	{
		// Init frame buffer
		m_spFrameBuffer = FrameBuffer::createEmpty();
		m_spFrameBuffer->bind();
		m_spRenderer->clear();

		// Init quad geometry
		auto spVL = VertexLayout::create();
		spVL->add(VertexAttribute("position", 0, 3, TYPE_FLOAT, 0));
		spVL->add(VertexAttribute("texcoord", 1, 2, TYPE_FLOAT, 3*sizeof(float), true));

		auto spVertexList = boost::shared_ptr<VertexList<VertexPosUV>>(new VertexList<VertexPosUV>(spVL));
		spVertexList->addVertex(VertexPosUV(Vec3(0.0f, 0.0f, 0.0f), Vec2(0.0, 0.0)));
		spVertexList->addVertex(VertexPosUV(Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0, 0.0)));
		spVertexList->addVertex(VertexPosUV(Vec3(1.0f, 1.0f, 0.0f), Vec2(1.0, 1.0)));
		spVertexList->addVertex(VertexPosUV(Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0, 1.0)));
		spVertexList->addIndex(0);
		spVertexList->addIndex(1);
		spVertexList->addIndex(2);
		spVertexList->addIndex(0);
		spVertexList->addIndex(2);
		spVertexList->addIndex(3);

		m_spQuadGeometry = m_spRenderer->createStaticGeometry(spVertexList, Geometry::TRIANGLES);
	}

} }