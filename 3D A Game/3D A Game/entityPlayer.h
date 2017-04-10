#pragma once
#include "entity.h"
#include "engine.h"
#include "HUtils.h"
#include "globals.h"
#include "entityManager.h"
#include "entityBouncyBall.h"
#include "input.h"
#include "cameraController.h"
#include "entityInterop.h"
#include "entityWorld.h"
#include "collider.h"
#include "entityPhysicsObject.h"
#include <iomanip>
struct entPlayer : entPhysicsObject
{
	entPlayer()
	{

	}
	entPlayer(HUtils::XYZ pos_)
	{
		pos = pos_;
	}
	void simulatePhysics()
	{
		float horizontalFriction = 1.008;
		
		float airFriction = 1.0005;
		float gravity = -globals->gravity * globals->timeDelta;
		onGround = 0;

		trace ground = collider->findCollision(pos, HUtils::XYZ(0, -1, 0), dynamic_cast<entWorld*>(entityInterop->getWorld())->mesh);
		
		float prevYVel = vel.y; //calculate the difference in y velocity before and after we hit the floor
		if (ground.didHit && !ground.traceFailed)
		{
			float groundAngle = abs(90 - (asin(ground.hitNormal.y) * 180.0f / 3.141592));
			//cout << "groundAngle: " << std::fixed <<  groundAngle << "\r\n";

			if (groundAngle > 2 && groundAngle < 40 && ground.distance < playerHeight + 0.1) /*Standard slope we can walk up*/
			{
				pos.y = ground.hitPos.y + playerHeight;
				onGround = 1;
				vel.y = 0;
			}
			if (ground.distance <= playerHeight + 0.01 && groundAngle >= 45) /*slide off*/
			{
				gravity /= 5.0f;
				vel.x += ground.hitNormal.x * gravity * -3.6;
				vel.z += ground.hitNormal.z * gravity * -3.6;
				pos.y = ground.hitPos.y + playerHeight;
			}
			if (ground.distance <= playerHeight)
			{
				vel.y = 0;
				pos.y = ground.hitPos.y + playerHeight;
				onGround = 1;
			}
		}
		camera->pitchAdditive -= (prevYVel - vel.y) * 250; //apply a pitch additive based on how much our velocity changed to shake our crosshair

		//float potentialFovAdd = vel.magnitude() * 1000.6; /*The faster the player is going, the more zoomed out he is*/
		//if (potentialFovAdd > camera->fovAdditive)
		//	camera->fovAdditive = potentialFovAdd;

		trace wall = collider->findCollision(pos + HUtils::XYZ(0,0.1,0), vel.normalized(), dynamic_cast<entWorld*>(entityInterop->getWorld())->mesh);

		if (wall.didHit && !wall.traceFailed && wall.distance < 0.5)
		{
			HUtils::XYZ hozVel = HUtils::XYZ(vel.x, 0, vel.z);
			vel.x +=  wall.hitNormal.x * hozVel.magnitude();
			vel.y += wall.hitNormal.y * hozVel.magnitude();
			vel.z += wall.hitNormal.z * hozVel.magnitude();
		}
		
		if (!onGround)
		{
			horizontalFriction = airFriction;
		}
		else
		{
			if (input->keyboard->keyDown(VK_SPACE))
			{
				pos.y += 0.1;
				vel.y += 0.005;
			}
		}
		vel += HUtils::XYZ(0, gravity, 0);
		
		float maxSpeed = 0.003;

		/*If we're moving don't apply the same amount of ground friction*/
		if (movVel.magnitude() > 0.001)
		{
			horizontalFriction /= 3;
		}

		if (vel.magnitude() > maxSpeed && movVel.magnitude() >= 0.00001)
		{
			movVel = movVel.normalized() * maxSpeed * -movVel.magnitude();
		}
		
		vel += movVel;

		movVel *= 0;

		vel.x /= horizontalFriction;
		vel.z /= horizontalFriction;

		pos += vel;
	}
	void tick()
	{
		speedMultiplier = globals->timeDelta / 100;
		float acceleration = 0.0062;

		if (!onGround)
		{
			acceleration = 0.00034;
		}

		simulatePhysics();
		if (input->keyboard->keyDown('W'))
		{
			//pos = pos + (camera->normalizedLookDir * HUtils::XYZ(1,0,1)).normalized() * HUtils::XYZ(speedMultiplier, 0, speedMultiplier);
			movVel += (camera->normalizedLookDir * HUtils::XYZ(1, 0, 1)).normalized() * HUtils::XYZ(speedMultiplier, 0, speedMultiplier) * acceleration;
		}
		if (input->keyboard->keyDown('S'))
		{
			movVel += (camera->normalizedLookDir * HUtils::XYZ(1, 0, 1)).normalized() * HUtils::XYZ(-speedMultiplier, 0, -speedMultiplier) * acceleration;
		}
		if (input->keyboard->keyDown('A'))
		{
			//pos = pos + yRotate(camera->normalizedLookDir, 270) * HUtils::XYZ(speedMultiplier, speedMultiplier, speedMultiplier);
			movVel += (yRotate(camera->normalizedLookDir, 270) * HUtils::XYZ(1, 0, 1)).normalized() * HUtils::XYZ(speedMultiplier, 0, speedMultiplier) * acceleration;
		}
		if (input->keyboard->keyDown('D'))
		{
			movVel += (yRotate(camera->normalizedLookDir, 90) * HUtils::XYZ(1, 0, 1)).normalized() * HUtils::XYZ(speedMultiplier, 0, speedMultiplier) * acceleration;
		}
		if (movVel.magnitude() > 0.001)
		{
			float prevMag = movVel.magnitude();
			movVel.normalize();
			movVel *= prevMag;
		}

		if (input->keyboard->keyDownEvent('E'))
		{
			exit(0);
		}
		if (input->keyboard->keyDownEvent('F'))
		{
			for(int i = 0; i < 1; i++)
				entityManager->addEntity(new entBounceyBall(HUtils::XYZ(0,0,0), HUtils::randVec()*0.1));
		}
		if (input->keyboard->keyDownEvent('G'))
		{
			int s = entityInterop->getEntityCount();
			for (int i = 0; i < s; i++)
			{
				cout << i << ": " << entityInterop->findEntity(i)->getClass() << endl;
			}
			Sleep(1000);
		}
		if (input->keyboard->keyDownEvent('Q'))
			camera->lockMouse = !camera->lockMouse;

		if (input->keyboard->keyDown(VK_LBUTTON))
		{
			vel += camera->normalizedLookDir.normalized() * -0.00004;
		}
		if (input->keyboard->keyDown(VK_CONTROL)) //crouched
		{
			crouched = 1;
			targetPlayerHeight = 0.8;
		}
		else
		{
			crouched = 0;
			targetPlayerHeight = 1.4;
		}

		playerHeight += ((targetPlayerHeight - playerHeight) / 100.0f) * globals->timeDelta;

		globals->cX = pos.x;
		globals->cY = pos.y;
		globals->cZ = pos.z;

		globals->cVX = vel.x;
		globals->cVY = vel.y;
		globals->cVZ = vel .z;

		//pos.print();
	}
	void render()
	{

	}
	virtual std::string getClass()
	{
		return "entPlayer";
	}
	float speedMultiplier = 0.0006;
	float playerHeight = 1.4;
	float standingPlayerHeight = 1.4;
	float targetPlayerHeight = 1;
	bool onGround = 0;
	bool crouched = 0;
	HUtils::XYZ movVel;
};