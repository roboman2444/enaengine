#include <ode/ode.h>

#include "globaldefs.h"
#include "console.h"
#include "matrixlib.h"
#include "physics.h"
#include "entitymanager.h"

int physicsOK;

dWorldID world;
dSpaceID space;
dJointGroupID contactgroup;

static void physics_nearCollide(void * data, void * gid1, void *gid2){
	dGeomID o1 = (dGeomID) gid1;
	dGeomID o2 = (dGeomID) gid2;
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	dContact contact;
	contact.surface.mode = dContactBounce | dContactSoftCFM;
	contact.surface.mu = dInfinity;
	contact.surface.bounce = 0.9;
	contact.surface.bounce_vel = 0.1;
	contact.surface.soft_cfm = 0.001;
	int numc;
	if((numc = dCollide(o1, o2, 1, &contact.geom, sizeof(dContact)))){
		dJointID c = dJointCreateContact(world, contactgroup, &contact);
		dJointAttach(c, b1, b2);
	}
}
void physics_nearCollideFCall(phystype_t ph1, phystype_t ph2){
	dGeomID o1 = (dGeomID) ph1.gid;
	dGeomID o2 = (dGeomID) ph2.gid;
	dBodyID b1 = (dBodyID) ph1.bid;
	dBodyID b2 = (dBodyID) ph2.bid;
	dContact contact;
	contact.surface.mode = dContactBounce | dContactSoftCFM;
	contact.surface.mu = dInfinity;
	contact.surface.bounce = 0.9;
	contact.surface.bounce_vel = 0.1;
	contact.surface.soft_cfm = 0.001;
	int numc;
	if((numc = dCollide(o1, o2, 1, &contact.geom, sizeof(dContact)))){
		dJointID c = dJointCreateContact(world, contactgroup, &contact);
		dJointAttach(c, b1, b2);
	}
}
//todo have a geom set per model, not per entity... faster to spam a lot of entities with the same model (especially if the collide is mesh based)
void physics_addPhystype(phystype_t *phy){
	dBodyID body = dBodyCreate(world);
	dGeomID geom;
	dMass m;
	switch(phy->collidetype){
		default:
		case ODESPHERE:
			geom = dCreateSphere(0, 1.0);
			dMassSetSphereTotal(&m, phy->mass, 1.0);
		break;
		case ODEBOX:
			geom = dCreateBox(0, 1.0, 1.0, 1.0);
			dMassSetBoxTotal(&m, phy->mass, 1.0, 1.0, 1.0);
		break;
		case ODECYLINDER:
			geom = dCreateCylinder(0, 1.0, 1.0);
			dMassSetCylinderTotal(&m, phy->mass, 0, 1.0, 1.0);
		break;
		case ODECAPSULE:
			geom = dCreateCapsule(0, 1.0, 1.0);
			dMassSetCapsuleTotal(&m, phy->mass, 0, 1.0, 1.0);
		break;
	}
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);
	dBodySetPosition(body, 0, 0, 0);
}
void physics_removePhystype(phystype_t *phy){
	dBodyDestroy(phy->bid);
	phy->bid = 0;
	dGeomDestroy(phy->gid);
	phy->gid = 0;
}
void physics_addEntity(entity_t *e){
	dBodyID body = dBodyCreate(world);
	phystype_t *phy = &e->phys;
	dGeomID geom;
	dMass m;
	switch(phy->collidetype){
		default:
		case ODESPHERE:
			geom = dCreateSphere(0, 1.0);
			dMassSetSphereTotal(&m, phy->mass, e->scale);
		break;
		case ODEBOX:
			geom = dCreateBox(0, 1.0, 1.0, 1.0);
			dMassSetBoxTotal(&m, phy->mass, 1.0, 1.0, 1.0);
		break;
		case ODECYLINDER:
			geom = dCreateCylinder(0, 1.0, 1.0);
			dMassSetCylinderTotal(&m, phy->mass, 0, 1.0, 1.0);
		break;
		case ODECAPSULE:
			geom = dCreateCapsule(0, 1.0, 1.0);
			dMassSetCapsuleTotal(&m, phy->mass, 0, 1.0, 1.0);
		break;
	}
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);
	dBodySetPosition(body, e->pos[0], e->pos[1], e->pos[2]);
	dBodySetLinearVel(body, e->vel[0], e->vel[1], e->vel[2]);
}
int physics_getEntD(void *ve){
	entity_t *e = (entity_t *) ve;
	int ret = FALSE;
	vec3_t epos = {e->pos[0], e->pos[1], e->pos[2]};
	/*
//	vec3_t npos = dGeomGetPosition((dGeomID)e->phys.gid);
	if(epos[0] != npos[0] || epos[1] != npos[1] || epos[2] != npos[2]){
		e->pos[0] = npos[0];
		e->pos[1] = npos[1];
		e->pos[2] = npos[2];
		ret = TRUE;
	}
	*/
	return ret;
}

int physics_init(void){
	physics_ok = FALSE;
	dInitODE();
	world = dWorldCreate();
	dWorldSetGravity(world, 0, 0, -0.2);
	dWorldSetCFM(world, 1e-5);
	contactgroup = dJointGroupCreate(0);
	physics_ok = TRUE;
	return TRUE;
}

void physics_odeTick(float time){
	dWorldQuickStep(world, time);
	dJointGroupEmpty(contactgroup);
}

int physics_destroy(void){
	dJointGroupDestroy(contactgroup);
	dWorldDestroy(world);
	dCloseODE();
	return TRUE;
}
