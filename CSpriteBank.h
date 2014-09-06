#ifndef __DREAM_CSPRITEBANK_INCLUDE_
#define __DREAM_CSPRITEBANK_INCLUDE_
#include "IGUISpriteBank.h"
//! Sprite bank interface.
class CSpriteBank : public ISpriteBank
{
public:

	CSpriteBank(IVideoDriver* driver);
	virtual ~CSpriteBank();

	virtual array<rectangle<s32> >& getPositions();
	virtual array< SSprite >& getSprites();

	virtual u32 getTextureCount() const;
	virtual ITexture* getTexture(u32 index) const;
	virtual void addTexture(ITexture* texture);
	virtual void setTexture(u32 index, ITexture* texture);

	//! Add the texture and use it for a single non-animated sprite.
	virtual s32 addTextureAsSprite(ITexture* texture);

	//! clears sprites, rectangles and textures
	virtual void clear();

	//! Draws a sprite in 2d with position and color
	virtual void draw2DSprite(u32 index, const position2di& pos, const rectangle<s32>* clip=0,
				const SColor& color= SColor(255,255,255,255),
				u32 starttime=0, u32 currenttime=0, bool loop=true, bool center=false);

	//! Draws a sprite batch in 2d using an array of positions and a color
	virtual void draw2DSpriteBatch(const array<u32>& indices, const array<position2di>& pos,
			const rectangle<s32>* clip=0,
			const SColor& color= SColor(255,255,255,255),
			u32 starttime=0, u32 currenttime=0,
			bool loop=true, bool center=false);

protected:

	struct SDrawBatch
	{
		array<position2di> positions;
		array<rectanglei> sourceRects;
		u32 textureNumber;
	};

	array<SSprite> Sprites;
	array<rectangle<s32> > Rectangles;
	array<ITexture*> Textures;
	IVideoDriver* Driver;
};
#endif