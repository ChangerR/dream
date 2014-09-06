#include "CSpriteBank.h"
#include "IVideoDriver.h"
#include "ITexture.h"

CSpriteBank::CSpriteBank(IVideoDriver* driver) :Driver(driver)
{
	if (Driver)
		Driver->addRef();
}


CSpriteBank::~CSpriteBank()
{
	// drop textures
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i])
			Textures[i]->releaseRef();

	// drop video driver
	if (Driver)
		Driver->releaseRef();
}


array< rectangle<s32> >& CSpriteBank::getPositions()
{
	return Rectangles;
}


array< SSprite >& CSpriteBank::getSprites()
{
	return Sprites;
}


u32 CSpriteBank::getTextureCount() const
{
	return Textures.size();
}


ITexture* CSpriteBank::getTexture(u32 index) const
{
	if (index < Textures.size())
		return Textures[index];
	else
		return 0;
}


void CSpriteBank::addTexture(ITexture* texture)
{
	if (texture)
		texture->addRef();

	Textures.push_back(texture);
}


void CSpriteBank::setTexture(u32 index, ITexture* texture)
{
	while (index >= Textures.size())
		Textures.push_back(0);

	if (texture)
		texture->addRef();

	if (Textures[index])
		Textures[index]->releaseRef();

	Textures[index] = texture;
}


//! clear everything
void CSpriteBank::clear()
{
	// drop textures
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i])
			Textures[i]->releaseRef();
	Textures.clear();
	Sprites.clear();
	Rectangles.clear();
}

//! Add the texture and use it for a single non-animated sprite.
s32 CSpriteBank::addTextureAsSprite(ITexture* texture)
{
	if ( !texture )
		return -1;

	addTexture(texture);
	u32 textureIndex = getTextureCount() - 1;

	u32 rectangleIndex = Rectangles.size();
	Rectangles.push_back( rectangle<s32>(0,0, texture->getOriginalSize().Width, texture->getOriginalSize().Height) );

	SSprite sprite;
	sprite.frameTime = 0;

	SSpriteFrame frame;
	frame.textureNumber = textureIndex;
	frame.rectNumber = rectangleIndex;
	sprite.Frames.push_back( frame );

	Sprites.push_back( sprite );

	return Sprites.size() - 1;
}

//! draws a sprite in 2d with scale and color
void CSpriteBank::draw2DSprite(u32 index, const position2di& pos,
		const rectangle<s32>* clip, const SColor& color,
		u32 starttime, u32 currenttime, bool loop, bool center)
{
	if (index >= Sprites.size() || Sprites[index].Frames.empty() )
		return;

	// work out frame number
	u32 frame = 0;
	if (Sprites[index].frameTime)
	{
		u32 f = ((currenttime - starttime) / Sprites[index].frameTime);
		if (loop)
			frame = f % Sprites[index].Frames.size();
		else
			frame = (f >= Sprites[index].Frames.size()) ? Sprites[index].Frames.size()-1 : f;
	}

	const ITexture* tex = Textures[Sprites[index].Frames[frame].textureNumber];
	if (!tex)
		return;

	const u32 rn = Sprites[index].Frames[frame].rectNumber;
	if (rn >= Rectangles.size())
		return;

	const rectangle<s32>& r = Rectangles[rn];

	if (center)
	{
		position2di p = pos;
		p -= r.getSize() / 2;
		Driver->draw2DImage(tex, p, r, clip, color, true);
	}
	else
	{
		Driver->draw2DImage(tex, pos, r, clip, color, true);
	}
}


void CSpriteBank::draw2DSpriteBatch(	const array<u32>& indices,
										const array<position2di>& pos,
										const rectangle<s32>* clip,
										const SColor& color,
										u32 starttime, u32 currenttime,
										bool loop, bool center)
{
	const u32 drawCount = min_<u32>(indices.size(), pos.size());

	if( Textures.empty() )
		return;
	array<SDrawBatch> drawBatches(Textures.size());
	for(u32 i = 0;i < Textures.size();i++)
	{
		drawBatches.push_back(SDrawBatch());
		drawBatches[i].positions.reallocate(drawCount);
		drawBatches[i].sourceRects.reallocate(drawCount);
	}

	for(u32 i = 0;i < drawCount;i++)
	{
		const u32 index = indices[i];

		if (index >= Sprites.size() || Sprites[index].Frames.empty() )
			continue;

		// work out frame number
		u32 frame = 0;
		if (Sprites[index].frameTime)
		{
			u32 f = ((currenttime - starttime) / Sprites[index].frameTime);
			if (loop)
				frame = f % Sprites[index].Frames.size();
			else
				frame = (f >= Sprites[index].Frames.size()) ? Sprites[index].Frames.size()-1 : f;
		}

		const u32 texNum = Sprites[index].Frames[frame].textureNumber;

		SDrawBatch& currentBatch = drawBatches[texNum];

		const u32 rn = Sprites[index].Frames[frame].rectNumber;
		if (rn >= Rectangles.size())
			return;

		const rectangle<s32>& r = Rectangles[rn];

		if (center)
		{
			position2di p = pos[i];
			p -= r.getSize() / 2;

			currentBatch.positions.push_back(p);
			currentBatch.sourceRects.push_back(r);
		}
		else
		{
			currentBatch.positions.push_back(pos[i]);
			currentBatch.sourceRects.push_back(r);
		}
	}

	for(u32 i = 0;i < drawBatches.size();i++)
	{
		if(!drawBatches[i].positions.empty() && !drawBatches[i].sourceRects.empty())
			Driver->draw2DImageBatch(Textures[i], drawBatches[i].positions,
				drawBatches[i].sourceRects, clip, color, true);
	}
}