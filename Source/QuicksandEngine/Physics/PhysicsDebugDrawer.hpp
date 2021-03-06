#ifndef QSE_PHYSICSDEBUGDRAWER_HPP
#define QSE_PHYSICSDEBUGDRAWER_HPP

#include "../../BulletPhysics/btBulletDynamicsCommon.h"

//////////////////////////////////////////////////////////////////////////////
// class BulletDebugDrawer						- Chapter 17, page 605
//
// Bullet uses this object to draw debug information.  This implementation
//   of btIDebugDraw uses direct3D lines to represent the current state
//   of the physics simulation 
//
#pragma warning(disable : 4275)
class BulletDebugDrawer : public btIDebugDraw
{
	DebugDrawModes               m_DebugModes;
public:
	// btIDebugDraw interface
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override;
	virtual void	reportErrorWarning(const char* warningString) override;
	virtual void	draw3dText(const btVector3& location,const char* textString) override;
	virtual void	setDebugMode(int debugMode) override;
	virtual int		getDebugMode() const override;
	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;

	// [mrmike] Added post press to read PlayerOptions.xml to turn on physics debug options.
	void ReadOptions(void);
};
#pragma warning(default : 4275)


#endif