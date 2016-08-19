#pragma once

class CBoxObject : public CObject {

public:

	CBoxObject() : CObject() {}
	virtual ~CBoxObject() = default;

	virtual void Update(float fTimeElapsed);
	void Rotate(float radians);

protected:

};



class CBoxObjectComponent : public CObjectComponent {

protected:

	using BoxShader = CBoxShader;
	using BoxMesh = CBoxMesh;

	using BoxObject = CBoxObject;

public:

	CBoxObjectComponent() = default;
	virtual ~CBoxObjectComponent() = default;

	void AddObject(XMVECTOR xmvec);

protected:


};
