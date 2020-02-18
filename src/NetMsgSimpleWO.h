#pragma once

#include "NetMsg.h"
#include "Vector.h"

namespace Aftr {
	class NetMsgSimpleWO : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgSimpleWO);

		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		Vector pos;
		int id;
	protected:

	};
}// namespace Aftr