#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"

//If we want to use way points, we need to include this.
#include "NewModuleWayPoints.h"
//NetMsg
#include "NetMessengerClient.h"
#include "NetMsgSimpleWO.h"

using namespace Aftr;

GLViewNewModule* GLViewNewModule::New(const std::vector< std::string >& args)
{
	GLViewNewModule* glv = new GLViewNewModule(args);
	glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
	glv->onCreate();
	return glv;
}


GLViewNewModule::GLViewNewModule(const std::vector< std::string >& args) : GLView(args)
{
	//Initialize any member variables that need to be used inside of LoadMap() here.
	//Note: At this point, the Managers are not yet initialized. The Engine initialization
	//occurs immediately after this method returns (see GLViewNewModule::New() for
	//reference). Then the engine invoke's GLView::loadMap() for this module.
	//After loadMap() returns, GLView::onCreate is finally invoked.

	//The order of execution of a module startup:
	//GLView::New() is invoked:
	//    calls GLView::init()
	//       calls GLView::loadMap() (as well as initializing the engine's Managers)
	//    calls GLView::onCreate()

	//GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
	//GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
	//At this point, all the managers are initialized. That is, the engine is fully initialized.

	if (this->pe != NULL)
	{
		//optionally, change gravity direction and magnitude here
		//The user could load these values from the module's aftr.conf
		this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
		this->pe->setGravityScalar(Aftr::GRAVITY);
	}
	this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
	//this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1

	// play the background sound
	SoundManager::init();
	SoundManager::playBackGround("../../../shared/mm/sounds/space.ogg", true, false, true);
	SoundManager::sound2D.at(0)->setVolume(0.5f);

	if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "8889") {
		client = NetMessengerClient::New("127.0.0.1", "8888");
	}
	else {
		client = NetMessengerClient::New("127.0.0.1", "8889");
	}
}


GLViewNewModule::~GLViewNewModule()
{
	//Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
	GLView::updateWorld(); //Just call the parent's update world first.
						   //If you want to add additional functionality, do it after
						   //this call.
	// update cam position
	SoundManager::setCamPosition(this->cam->getPosition(), this->cam->getLookDirection(), Vector(0, 0, 0), this->cam->getNormalDirection());

	// pos update // it works but not useful, too much waste.
	//WO* wo = actorLst->at(0);
	//NetMsgSimpleWO msg;
	//msg.pos = wo->getPosition();
	//msg.id = 0;
	//client->sendNetMsgSynchronousTCP(msg);

}


void GLViewNewModule::onResizeWindow(GLsizei width, GLsizei height)
{
	GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewNewModule::onMouseDown(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseDown(e);
}


void GLViewNewModule::onMouseUp(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseUp(e);
}


void GLViewNewModule::onMouseMove(const SDL_MouseMotionEvent& e)
{
	GLView::onMouseMove(e);
}

static int speech_check = 1;

void GLViewNewModule::onKeyDown(const SDL_KeyboardEvent& key)
{
	GLView::onKeyDown(key);
	if (key.keysym.sym == SDLK_0)
		this->setNumPhysicsStepsPerRender(1);

	// press 's' to open your sound radar to determine where the enermy is
	// the enermy will say different things. Listen carefully
	if (key.keysym.sym == SDLK_s) 
	{
		if (speech_check == 1) 
		{
			cout << "Enermy voice detected!!!" << endl;
			Vector cur_cam_pos = this->cam->getPosition();
			SoundManager::play3DSound("../../../shared/mm/sounds/voice/WOHuman Your Teacher Looks Funny.wav", Vector(0, 0, 5), false, false, true);
			SoundManager::sound3D.back()->setVolume(0.9f);
			SoundManager::sound3D.back()->setMinDistance(10.0f);
			speech_check = 0;
		}
		else
		{
			cout << "Enermy is talking !!!" << endl;
			Vector cur_cam_pos = this->cam->getPosition();
			SoundManager::play3DSound("../../../shared/mm/sounds/voice/WOHuman Making Me Laugh.wav", Vector(0, 0, 5), false, false, true);
			SoundManager::sound3D.back()->setVolume(0.9f);
			SoundManager::sound3D.back()->setMinDistance(10.0f);
			speech_check = 1;
		}
	}

	// press '1' to take off the jet 1
	if (key.keysym.sym == SDLK_1)
	{
		WO* wo = actorLst->at(0);
		Vector v = wo->getPosition();
		if (v.x > 30) {
			wo->moveRelative(Vector(0.5, 0.5, 0.5));
			wo->resetJoints();
			wo->changeOrientationWRTparentDeltaRoll(10);
		}
		else {
			wo->moveRelative(Vector(0.5, 0.5, 0));
		}
		printf("jet 1 taking off!\n");
		// update position
		NetMsgSimpleWO msg;
		msg.pos = wo->getPosition();
		msg.id = 0;
		client->sendNetMsgSynchronousTCP(msg);
	}
	// press '1' to take off the jet 2
	if (key.keysym.sym == SDLK_2)
	{
		WO* wo = actorLst->at(1);
		Vector v = wo->getPosition();
		if (v.x > 30) {
			wo->moveRelative(Vector(0.5, -0.5, 0.6));
			wo->resetJoints();
			wo->changeOrientationWRTparentDeltaRoll(10);
		}
		else {
			wo->moveRelative(Vector(0.5, -0.5, 0));
		}
		printf("jet 2 taking off!\n");
		// update position
		NetMsgSimpleWO msg;
		msg.pos = wo->getPosition();
		msg.id = 1;
		client->sendNetMsgSynchronousTCP(msg);
	}
	// press '1' to take off the jet 3
	if (key.keysym.sym == SDLK_3)
	{
		WO* wo = actorLst->at(2);
		Vector v = wo->getPosition();
		if (v.x > 30) {
			wo->moveRelative(Vector(1, 0, 1));
			wo->resetJoints();
			wo->changeOrientationWRTparentDeltaRoll(10);
		}
		else {
			wo->moveRelative(Vector(1, 0, 0));
		}
		printf("jet 3 taking off!\n");
		// update position
		NetMsgSimpleWO msg;
		msg.pos = wo->getPosition();
		msg.id = 2;
		client->sendNetMsgSynchronousTCP(msg);
	}

	// press 'e' to change sound effct
	// it will crash!!!

	//if (key.keysym.sym == SDLK_e)
	//{
	//	SoundManager::changeEffect();
	//}

	// press 'c' to show the car
	// the enermy will say different things. Listen carefully
	//if (key.keysym.sym == SDLK_c)
	//{
	//	//std::string racecar("../mm/models/spider.obj");
	//	std::string racecar("../mm/models/formula_1/Formula_1_mesh.obj");

	//	cout << "A Ferrari Occoured!!!!" << endl;
	//	WO* wo = WO::New(racecar, Vector(0.025, 0.025, 0.025), MESH_SHADING_TYPE::mstFLAT);
	//	wo->setPosition(Vector(35, 0, 1.5));
	//	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//	wo->setLabel("racecar");
	//	worldLst->push_back(wo);
	//}
}


void GLViewNewModule::onKeyUp(const SDL_KeyboardEvent& key)
{
	GLView::onKeyUp(key);
}


void Aftr::GLViewNewModule::loadMap()
{
	this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
	this->actorLst = new WorldList();
	this->netLst = new WorldList();

	ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
	ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
	ManagerOpenGLState::enableFrustumCulling = false;
	Axes::isVisible = true;
	this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

	this->cam->setPosition(15, 15, 10);

	std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
	std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
	std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
	std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");
	std::string jet(ManagerEnvironmentConfiguration::getSMM() + "/models/jet_wheels_down_PP.wrl");

	//SkyBox Textures readily available
	std::vector< std::string > skyBoxImageNames; //vector to store texture paths
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
	skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg");
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

	float ga = 0.1f; //Global Ambient Light level for this module
	ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
	WOLight* light = WOLight::New();
	light->isDirectionalLight(true);
	light->setPosition(Vector(0, 0, 100));
	//Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
	//for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
	light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
	light->setLabel("Light");
	worldLst->push_back(light);

	//Create the SkyBox
	WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
	wo->setPosition(Vector(0, 0, 0));
	wo->setLabel("Sky Box");
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	worldLst->push_back(wo);

	////Create the infinite grass plane (the floor)
	wo = WO::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wo->setPosition(Vector(0, 0, 0));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
	grassSkin.getMultiTextureSet().at(0)->setTextureRepeats(5.0f);
	grassSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
	grassSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
	grassSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
	grassSkin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
	wo->setLabel("Grass");
	worldLst->push_back(wo);

	//Create my object
	wo = WO::New(jet, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wo->setPosition(Vector(0, 0, 5));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	wo->setLabel("jet1");
	worldLst->push_back(wo);

	wo = WO::New(jet, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wo->setPosition(Vector(0, 10, 5));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	wo->setLabel("jet2");
	actorLst->push_back(wo);
	worldLst->push_back(wo);

	wo = WO::New(jet, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wo->setPosition(Vector(0, -10, 5));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	wo->setLabel("jet3");
	actorLst->push_back(wo);
	worldLst->push_back(wo);

	wo = WO::New(jet, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wo->setPosition(Vector(15, 0, 5));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	wo->setLabel("jet4");
	actorLst->push_back(wo);
	worldLst->push_back(wo);



	////Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
	//wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
	//wo->setPosition( Vector(0,0,0) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	////Create the infinite grass plane that uses NVIDIAPhysX(the floor)
	//wo = WONVStaticPlane::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//wo->setPosition( Vector(0,0,0) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	////Create the infinite grass plane (the floor)
	//wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//wo->setPosition( Vector(0,0,50.0f) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	//wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//wo->setPosition( Vector(0,0.5f,75.0f) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	//wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue("sharedmultimediapath") + "/models/sphereRp5.wrl", Vector(1.0f, 1.0f, 1.0f), mstSMOOTH );
	//wo->setPosition( 0,0,100.0f );
	//wo->setLabel( "Sphere" );
	//this->worldLst->push_back( wo );

	//wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
	//((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
	//((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
	//wo->setPosition( Vector(20,20,20) );
	//wo->setLabel( "Paladin" );
	//worldLst->push_back( wo );
	//actorLst->push_back( wo );
	//netLst->push_back( wo );
	//this->setActor( wo );
	//
	//wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
	//wo->setPosition( Vector(20,10,20) );
	//wo->isVisible = false; //hide the WOHuman's bounding box
	//((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
	//wo->setLabel( "Human Cyborg" );
	//worldLst->push_back( wo );
	//actorLst->push_back( wo ); //Push the WOHuman as an actor
	//netLst->push_back( wo );
	//this->setActor( wo ); //Start module where human is the actor

	////Create and insert the WOWheeledVehicle
	//std::vector< std::string > wheels;
	//std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
	//wheels.push_back( wheelStr );
	//wheels.push_back( wheelStr );
	//wheels.push_back( wheelStr );
	//wheels.push_back( wheelStr );
	//wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
	//wo->setPosition( Vector( 5, -15, 20 ) );
	//wo->setLabel( "Car 1970s Beater" );
	//((WOODE*)wo)->mass = 200;
	//worldLst->push_back( wo );
	//actorLst->push_back( wo );
	//this->setActor( wo );
	//netLst->push_back( wo );

	createNewModuleWayPoints();
}


void GLViewNewModule::createNewModuleWayPoints()
{
	// Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
	WayPointParametersBase params(this);
	params.frequency = 5000;
	params.useCamera = true;
	params.visible = true;
	WOWayPointSpherical* wayPt = WOWP1::New(params, 3);
	wayPt->setPosition(Vector(50, 0, 3));
	worldLst->push_back(wayPt);
}
