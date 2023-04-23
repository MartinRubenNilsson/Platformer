#pragma once
#include <entt/entt.hpp>


class EntityCopier
{
public:
	EntityCopier(entt::registry& aDstReg, const entt::registry& aSrcReg);
	EntityCopier(entt::registry& aDstAndSrcReg);

	entt::entity ShallowCopy(entt::entity aSrcEntity); // Does not copy children
	entt::entity DeepCopy(entt::entity aSrcEntity); // Copies children

private:
	entt::registry& myDstReg;
	const entt::registry& mySrcReg;
};