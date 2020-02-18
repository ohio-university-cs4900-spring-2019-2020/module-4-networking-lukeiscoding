#include "NetMsgSimpleWO.h"
#include <sstream>
#include <iostream>
#include "GLViewNewModule.h"
#include "ManagerGLView.h"
#include "WorldContainer.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMsgSimpleWO);

bool NetMsgSimpleWO::toStream(NetMessengerStreamBuffer & os) const
{
	os << pos.x << pos.y << pos.z << id;
	return true;
}

bool NetMsgSimpleWO::fromStream(NetMessengerStreamBuffer & is)
{
	is >> pos.x >> pos.y >> pos.z >> id;
	return true;
}

void NetMsgSimpleWO::onMessageArrived()
{
	WO* wo = ((GLViewNewModule*)ManagerGLView::getGLView())->getActorLst()->at(id);
	wo->setPosition(pos);
	std::cout << this->toString() << std::endl;
	std::cout << "jet " << id + 1 << " is taking off!" << std::endl;
}

std::string NetMsgSimpleWO::toString() const
{
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "Payload: \n"
		<< "p:{" << pos.x << "," << pos.y << "," << pos.z << "} id:"<<id<<"\n";
	return ss.str();
}
