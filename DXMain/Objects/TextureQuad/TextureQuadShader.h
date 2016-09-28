#pragma once
#include "Objects\Base\Shader.h"

class CTextureQuadShader : public CShader {

public:

	CTextureQuadShader() : CShader() { DefineInputElementDescriptions(); }
	virtual ~CTextureQuadShader() = default;

	virtual bool DefineInputElementDescriptions();

protected:

};