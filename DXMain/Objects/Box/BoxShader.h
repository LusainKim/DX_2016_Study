#pragma once

#include "Objects\Base\Shader.h"

class CBoxShader : public CShader {

public:
	
	CBoxShader() : CShader() { DefineInputElementDescriptions(); }
	virtual ~CBoxShader() = default;

	virtual bool DefineInputElementDescriptions();

protected:
	
};