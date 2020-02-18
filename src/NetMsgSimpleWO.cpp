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
	os << pos.x << pos.y << pos.z;
	return true;
}

bool NetMsgSimpleWO::fromStream(NetMessengerStreamBuffer & is)
{
	is >> pos.x >> pos.y >> pos.z;
	return true;
}

void NetMsgSimpleWO::onMessageArrived()
{
	WO* wo = ((GLViewNewModule*)ManagerGLView::getGLView())->getActorLst()->at(0);
	wo->setPosition(pos);
	std::cout << this->toString() << std::endl;
}

std::string NetMsgSimpleWO::toString() const
{
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "Payload: \n"
		<< "p:{" << pos.x << "," << pos.y << "," << pos.z << "}\n";
	return ss.str();
}
